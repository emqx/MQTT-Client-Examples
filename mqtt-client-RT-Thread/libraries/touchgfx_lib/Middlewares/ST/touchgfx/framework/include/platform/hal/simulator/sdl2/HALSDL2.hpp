/**
  ******************************************************************************
  * This file is part of the TouchGFX 4.15.0 distribution.
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/**
 * @file platform/hal/simulator/sdl2/HALSDL2.hpp
 *
 * Declares the touchgfx::HALSDL2 class.
 */
#ifndef HALSDL2_HPP
#define HALSDL2_HPP

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <platform/driver/touch/TouchController.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>

namespace touchgfx
{
/** Simulator enable stdio. */
void simulator_enable_stdio();

/**
 * Simulator printf.
 *
 * @param  format Describes the format to use.
 * @param  pArg   The argument list.
 */
void simulator_printf(const char* format, va_list pArg);

/**
 * HAL implementation for the TouchGFX simulator. This particular simulator HAL implementation
 * uses SDL2 to show the content of the framebuffer in a window.
 *
 * @see HAL
 */
class HALSDL2 : public HAL
{
public:
    /**
     * Initializes a new instance of the HALSDL2 class.
     *
     * @param [in] dma       Reference to DMA interface.
     * @param [in] lcd       Reference to the LCD.
     * @param [in] touchCtrl Reference to Touch Controller driver.
     * @param      width     Width of the display.
     * @param      height    Height of the display.
     */
    HALSDL2(DMA_Interface& dma, LCD& lcd, TouchController& touchCtrl, uint16_t width, uint16_t height)
        : HAL(dma, lcd, touchCtrl, width, height),
          portraitSkin(),
          landscapeSkin(),
          currentSkin(0),
          isSkinActive(true),
          isWindowBorderless(false),
          isWindowVisible(true),
          windowDrag(false)
    {
        setVsyncInterval(30.f); // Simulate 20Hz per default for backward compatibility
        updateCurrentSkin();
    }

    /**
     * Main event loop. Will wait for VSYNC signal, and then process next frame. Call this
     * function from your GUI task.
     *
     * @note This function never returns!
     */
    virtual void taskEntry();

    /**
     * Sample key event from keyboard.
     *
     * @param [out] key Output parameter that will be set to the key value if a key press was
     *                  detected.
     *
     * @return True if a key press was detected and the "key" parameter is set to a value.
     */
    virtual bool sampleKey(uint8_t& key);

    /**
     * This function is called whenever the framework has performed a complete draw.
     *
     * On some platforms, a local framebuffer needs to be pushed to the display through a
     * SPI channel or similar. Implement that functionality here. This function is called
     * whenever the framework has performed a complete draw.
     */
    virtual void flushFrameBuffer();

    /**
     * This function is called whenever the framework has performed a partial draw.
     *
     * @param  rect The area of the screen that has been drawn, expressed in absolute coordinates.
     */
    virtual void flushFrameBuffer(const Rect& rect);

    /**
     * This function performs a platform-specific memcpy, if supported by the hardware.
     *
     * @param [out] dest     Pointer to destination memory.
     * @param       src      Pointer to source memory.
     * @param       numBytes Number of bytes to copy.
     *
     * @return true if the copy succeeded, false if copy was not performed.
     */
    virtual bool blockCopy(void* RESTRICT dest, const void* RESTRICT src, uint32_t numBytes);

    ///@cond
    /**
     * If Blit-operations are supported, transparency-keying support is implicitly assumed.
     *
     * @param  key The "transparent" color value.
     *
     * @deprecated Transparancy key is no longer supported.
     */
    TOUCHGFX_DEPRECATED(
        "Transparancy key is no longer supported.",
        virtual void blitSetTransparencyKey(uint16_t key));
    ///@endcond

    /**
     * Sets vsync interval for simulating same tick speed as the real hardware. Due to
     * limitations in the granularity of SDL, the generated ticks in the simulator might not
     * occur at the exact time, but accumulated over several ticks, the precision is very
     * good.
     *
     * @param  ms The milliseconds between ticks.
     *
     * @note That you can also use HAL::setFrameRateCompensation() in the simulator. The effect of
     *       this can easily be demonstrated by dragging the console output window of the
     *       simulator (when running from Visual Studio) as this will pause the SDL and
     *       generate a lot of ticks when the console window is released. Beware that
     *       since the missed vsyncs are accumulated in an 8 bit counter, only up to 255
     *       ticks may be missed, so at VsyncInterval = 16.6667, dragging the windows for
     *       more than 255 * 16.6667ms = 4250ms
     *       = 4.25s will not generate all the ticks that were actually missed. This
     *       situation is, however, not very realistic, as normally just a couple of
     *       vsyncs are skipped.
     */
    void setVsyncInterval(float ms);

    /**
     * Samples the position of the mouse cursor.
     *
     * @param [out] x The x coordinate.
     * @param [out] y The y coordinate.
     *
     * @return True if touch detected, false otherwise.
     */
    bool doSampleTouch(int32_t& x, int32_t& y) const;

    /**
     * Initializes SDL.
     *
     * @param      argcount Number of arguments.
     * @param [in] args     Arguments.
     *
     * @return True if init went well, false otherwise.
     */
    virtual bool sdl_init(int argcount, char** args);

    /**
     * Sets window title.
     *
     * Sets window title of the TouchGFX simulator.
     *
     * @param  title The title, if null the original "TouchGFX simulator" will be used.
     *
     * @see getWindowTitle
     */
    static void setWindowTitle(const char* title);

    /**
     * Gets window title.
     *
     * @return null "TouchGFX simulator" unless set to something else using setWindowTitle().
     *
     * @see setWindowTitle
     */
    static const char* getWindowTitle();

    /**
     * Loads a skin for a given display orientation that will be rendered in the simulator
     * window with the the TouchGFX framebuffer placed inside the bitmap at the given
     * coordinates. Different bitmaps can be loaded in landscape and portrait mode. If the
     * provided bitmap cannot be loaded, the TouchGFX framebuffer will be displayed as
     * normal. If the png files contain areas with alpha
     * < 255, this will be used to create a shaped window.
     *
     * @param  orientation The orientation.
     * @param  x           The x coordinate.
     * @param  y           The y coordinate.
     *
     * @note The skins must be named "portrait.png" and "landscape.png" and placed inside the
     *       "simulator/" folder. The build process of the simulator will automatically
     *       copy the skins to the folder where the executable simulator is generated.
     * @note When as skin is set, the entire framebuffer is rendered through SDL whenever there is
     *       a change. Without a skin, only the areas with changes is rendered through SDL.
     */
    void loadSkin(touchgfx::DisplayOrientation orientation, int x, int y);

    /** Saves a screenshot to the default folder and default filename. */
    void saveScreenshot();

    /**
     * Copy the next N screenshots to disk. On each screen update, the new screen is saved
     * to disk.
     *
     * @param  n Number of screenshots to save. These are added to any ongoing amount of
     *           screenshots in queue.
     */
    virtual void saveNextScreenshots(int n);

    /**
     * Saves a screenshot.
     *
     * @param [in] folder   Folder name to place the screenshot in.
     * @param [in] filename Filename to save the screenshot to.
     */
    virtual void saveScreenshot(char* folder, char* filename);

    /** Copies the screenshot to clipboard. */
    virtual void copyScreenshotToClipboard();

    /**
     * Single stepping enable/disable. When single stepping is enabled, F10 will execute one
     * tick and F9 will disable single stepping.
     *
     * @param  singleStepping (Optional) True to pause the simulation and start single stepping.
     *
     * @see isSingleStepping
     */
    static void setSingleStepping(bool singleStepping = true);

    /**
     * Is single stepping.
     *
     * @return True if single stepping, false if not.
     *
     * @see setSingleStepping
     */
    static bool isSingleStepping();

    /**
     * Single step a number of steps. Only works if single stepping is already enabled.
     *
     * @param  steps (Optional) The steps Default is 1 step.
     *
     * @see setSingleStepping, isSingleStepping
     */
    static void singleStep(uint16_t steps = 1);

#ifndef __linux__
    /**
     * Gets the argc and argv for a Windows program.
     *
     * @param [in,out] argc Pointer to where to store number of arguments.
     *
     * @return The argv list of arguments.
     */
    static char** getArgv(int* argc);
#endif

    /**
     * Scale framebuffer to 24bpp. The format of the framebuffer (src) is given in parameter
     * format. The result is placed in the pre-allocated memory pointed to by parameter dst.
     * If the frambebuffer is in format Bitmap::RGB888, parameter dst is not used and the
     * parameter src is simply returned.
     *
     * @param [out] dst    Destination for the framebuffer. must be non-null unless format is
     *                     Bitmap::RGB888.
     * @param [in]  src    The framebuffer.
     * @param       format Describes the format of the framebuffer (lcd().framebufferFormat()).
     *
     * @return Null if it fails, else a pointer to an uint8_t.
     */
    static uint8_t* scaleTo24bpp(uint8_t* dst, uint16_t* src, Bitmap::BitmapFormat format);

    /**
     * Rotates a framebuffer if the display is rotated.
     *
     * @param [out] dst Destination for the rotated framebuffer. must be non-null if the
     *                  screen is rotated.
     * @param [in]  src The framebuffer.
     *
     * @return Null if it fails, else a pointer to an uint8_t.
     */
    static uint8_t* doRotate(uint8_t* dst, uint8_t* src);

    /**
     * Change visibility of window (hidden vs. shown).
     *
     * @param  visible      Should the window be visible?
     * @param  redrawWindow (Optional) Should the window be redrawn? Default is true.
     */
    void setWindowVisible(bool visible, bool redrawWindow = true)
    {
        isWindowVisible = visible;
        if (redrawWindow)
        {
            recreateWindow();
            touchgfx::simulator_enable_stdio();
        }
    }

    /**
     * Are windows visible?
     *
     * @return True if it is visible, false if it is hidden.
     */
    bool getWindowVisible() const
    {
        return isWindowVisible;
    }

protected:
    /**
     * Gets TFT framebuffer.
     *
     * @return null if it fails, else the TFT framebuffer.
     */
    virtual uint16_t* getTFTFrameBuffer() const;

    /**
     * Sets TFT framebuffer.
     *
     * @param [in] addr The address of the TFT framebuffer.
     */
    void setTFTFrameBuffer(uint16_t* addr);

    /**
     * Update framebuffer using an SDL Surface.
     *
     * @param      _rectToUpdate Area to update.
     * @param [in] frameBuffer   Target framebuffer.
     */
    virtual void renderLCD_FrameBufferToMemory(const Rect& _rectToUpdate, uint8_t* frameBuffer);

    /** Disables the DMA and LCD interrupts. */
    virtual void disableInterrupts()
    {
    }

    /** Enables the DMA and LCD interrupts. */
    virtual void enableInterrupts()
    {
    }

    /** Configures LCD interrupt. */
    virtual void configureLCDInterrupt()
    {
    }

    /** Enables the LCD interrupt. */
    virtual void enableLCDControllerInterrupt()
    {
    }

    /**
     * Configures the interrupts relevant for TouchGFX. This primarily entails setting the
     * interrupt priorities for the DMA and LCD interrupts.
     */
    virtual void configureInterrupts()
    {
    }

    /** Perform the actual display orientation change. */
    void performDisplayOrientationChange();

private:
    void recreateWindow(bool updateContent = true);
    void pushTouch(bool down) const;
    bool popTouch() const;
    static void updateTitle(int32_t x, int32_t y);
    void alphaChannelCheck(SDL_Surface* surface, bool& isOpaque, bool& hasSemiTransparency);
    void updateCurrentSkin();
    int getCurrentSkinX() const;
    int getCurrentSkinY() const;

    static bool debugInfoEnabled;

    float msBetweenTicks;
    float msPassed;

    static uint16_t icon[];
    static const char* customTitle;

    class SkinInfo
    {
    public:
        SDL_Surface* surface;
        bool isOpaque;
        bool hasSemiTransparency;
        int offsetX;
        int offsetY;
        SkinInfo()
            : surface(0), isOpaque(true), hasSemiTransparency(false), offsetX(0), offsetY(0)
        {
        }
        virtual ~SkinInfo()
        {
        }
    };

    char programPath[300];
    SkinInfo portraitSkin;
    SkinInfo landscapeSkin;
    SkinInfo* currentSkin;
    bool isSkinActive;
    bool isWindowBorderless;
    bool isWindowVisible;
    static bool flashInvalidatedRect;

    bool windowDrag;
    int windowDragX;
    int windowDragY;

    static int32_t _xMouse;
    static int32_t _yMouse;
    static int32_t _x;
    static int32_t _y;
    static bool isWindowBeingDragged;
    static int initialWindowX;
    static int initialWindowY;
    static int initialMouseX;
    static int initialMouseY;
    static bool _lastTouch;
    static bool _touches[5];
    static int _numTouches;

    static uint8_t keyPressed;

    static bool singleSteppingEnabled;
    static uint16_t singleSteppingSteps;
};

} // namespace touchgfx

#endif // HALSDL2_HPP
