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
 * @file platform/driver/lcd/LCD1bpp.hpp
 *
 * Declares the touchfgx::LCD1bpp and touchgfx::LCD1DebugPrinter classes.
 */
#ifndef LCD1BPP_HPP
#define LCD1BPP_HPP

#include <stdarg.h>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/Font.hpp>
#include <touchgfx/TextProvider.hpp>
#include <touchgfx/Unicode.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/lcd/LCD.hpp>

namespace touchgfx
{
#undef LCD

/**
 * This class contains the various low-level drawing routines for drawing bitmaps, texts and
 * rectangles on 1 bits per pixel displays.
 *
 * @see LCD
 *
 * @note All coordinates are expected to be in absolute coordinates!
 */
class LCD1bpp : public LCD
{
public:
    virtual void drawPartialBitmap(const Bitmap& bitmap, int16_t x, int16_t y, const Rect& rect, uint8_t alpha = 255, bool useOptimized = true);

    virtual void blitCopy(const uint16_t* sourceData, const Rect& source, const Rect& blitRect, uint8_t alpha, bool hasTransparentPixels);

    virtual void blitCopy(const uint8_t* sourceData, Bitmap::BitmapFormat sourceFormat, const Rect& source, const Rect& blitRect, uint8_t alpha, bool hasTransparentPixels);

    virtual uint16_t* copyFrameBufferRegionToMemory(const Rect& visRegion, const Rect& absRegion, const BitmapId bitmapId);

    virtual void fillRect(const Rect& rect, colortype color, uint8_t alpha = 255);

    virtual uint8_t bitDepth() const
    {
        return 1;
    }

    virtual Bitmap::BitmapFormat framebufferFormat() const
    {
        return Bitmap::BW;
    }

    virtual uint16_t framebufferStride() const
    {
        return getFramebufferStride();
    }

    /**
     * Framebuffer stride in bytes. The distance (in bytes) from the start of one
     * framebuffer row, to the next.
     *
     * @return The number of bytes in one framebuffer row.
     */
    FORCE_INLINE_FUNCTION static uint16_t getFramebufferStride()
    {
        assert(HAL::FRAME_BUFFER_WIDTH > 0 && "HAL has not been initialized yet");
        return (HAL::FRAME_BUFFER_WIDTH + 7) / 8;
    }

    virtual colortype getColorFrom24BitRGB(uint8_t red, uint8_t green, uint8_t blue) const
    {
        return getColorFromRGB(red, green, blue);
    }

    /**
     * Generates a color representation to be used on the LCD, based on 24 bit RGB values.
     *
     * @param  red   Value of the red part (0-255).
     * @param  green Value of the green part (0-255).
     * @param  blue  Value of the blue part (0-255).
     *
     * @return The color representation depending on LCD color format.
     */
    FORCE_INLINE_FUNCTION static colortype getColorFromRGB(uint8_t red, uint8_t green, uint8_t blue)
    {
        // Find the GRAY value (http://en.wikipedia.org/wiki/Luma_%28video%29) rounded to nearest integer
        return (red * 54 + green * 183 + blue * 19) >> 15;
    }

    virtual uint8_t getRedColor(colortype color) const
    {
        return getRedFromColor(color);
    }

    /**
     * Gets red from color.
     *
     * @param  color The color.
     *
     * @return The red from color.
     */
    FORCE_INLINE_FUNCTION static uint8_t getRedFromColor(colortype color)
    {
        return (color & 0x1) * 0xFF;
    }

    virtual uint8_t getGreenColor(colortype color) const
    {
        return getGreenFromColor(color);
    }

    /**
     * Gets green from color.
     *
     * @param  color The color.
     *
     * @return The green from color.
     */
    FORCE_INLINE_FUNCTION static uint8_t getGreenFromColor(colortype color)
    {
        return (color & 0x1) * 0xFF;
    }

    virtual uint8_t getBlueColor(colortype color) const
    {
        return getBlueFromColor(color);
    }

    /**
     * Gets blue from color.
     *
     * @param  color The color.
     *
     * @return The blue from color.
     */
    FORCE_INLINE_FUNCTION static uint8_t getBlueFromColor(colortype color)
    {
        return (color & 0x1) * 0xFF;
    }

    /**
     * Enables the texture mappers for all image formats. Currently texture mapping is not
     * supported on 1bpp displays, so this function does not do anything. It is merely
     * included to allow function enableTextureMapperAll() to be called on any subclass of
     * LCD.
     */
    void enableTextureMapperAll();

protected:
    virtual void drawTextureMapScanLine(const DrawingSurface& dest, const Gradients& gradients, const Edge* leftEdge, const Edge* rightEdge, const TextureSurface& texture, const Rect& absoluteRect, const Rect& dirtyAreaAbsolute, RenderingVariant renderVariant, uint8_t alpha, uint16_t subDivisionSize)
    {
        assert(0 && "Texture mapping not supported for 1bpp");
    }

    /**
     * Find out how much to advance in the display buffer to get to the next pixel.
     *
     * @param  rotatedDisplay Is the display running in portrait mode?
     * @param  textRotation   Rotation to perform.
     *
     * @return How much to advance to get to the next pixel.
     */
    static int nextPixel(bool rotatedDisplay, TextRotation textRotation);

    /**
     * Find out how much to advance in the display buffer to get to the next line.
     *
     * @param  rotatedDisplay Is the display running in portrait mode?
     * @param  textRotation   Rotation to perform.
     *
     * @return How much to advance to get to the next line.
     */
    static int nextLine(bool rotatedDisplay, TextRotation textRotation);

    virtual void drawGlyph(uint16_t* wbuf16, Rect widgetArea, int16_t x, int16_t y, uint16_t offsetX, uint16_t offsetY, const Rect& invalidatedArea, const GlyphNode* glyph, const uint8_t* glyphData, uint8_t byteAlignRow, colortype color, uint8_t bitsPerPixel, uint8_t alpha, TextRotation rotation);

    /**
     * Fill memory efficiently. Try to get 32bit aligned or 16bit aligned and then copy as
     * quickly as possible.
     *
     * @param [out] dst         Pointer to memory to fill.
     * @param       color       Color to write to memory, either 0 => 0x00000000 or 1 =>
     *                          0xFFFFFFFF.
     * @param       bytesToFill Number of bytes to fill.
     */
    static void fillMemory(void* RESTRICT dst, colortype color, uint16_t bytesToFill);

    /**
     * Blits a run-length encoded2D source-array to the framebuffer if alpha &gt; zero.
     *
     * @param  _sourceData The source-array pointer (points to the beginning of the data). Data
     *                     stored in RLE format, where each byte indicates number of
     *                     pixels with certain color, alternating between black and
     *                     white. First byte represents black.
     * @param  source      The location and dimensions of the source.
     * @param  blitRect    A rectangle describing what region is to be drawn.
     * @param  alpha       The alpha value to use for blending (0 = invisible, otherwise solid).
     */
    virtual void blitCopyRLE(const uint16_t* _sourceData, const Rect& source, const Rect& blitRect, uint8_t alpha);

    /**
     * Copies a rectangular area from the framebuffer til a givene memory address, which is
     * typically in the animation storage or a dynamic bitmap.
     *
     * @param      srcAddress     Source address (byte address).
     * @param      srcStride      Source stride (number of bytes to advance to next line).
     * @param      srcPixelOffset Source pixel offset (first pixel in first source byte).
     * @param [in] dstAddress     If destination address (byte address).
     * @param      dstStride      Destination stride (number of bytes to advance to next line).
     * @param      dstPixelOffset Destination pixel offset (first pixel in destination byte).
     * @param      width          The width of area (in pixels).
     * @param      height         The height of area (in pixels).
     */
    void copyRect(const uint8_t* srcAddress, uint16_t srcStride, uint8_t srcPixelOffset, uint8_t* RESTRICT dstAddress, uint16_t dstStride, uint8_t dstPixelOffset, uint16_t width, uint16_t height) const;

private:
    class bwRLEdata
    {
    public:
        bwRLEdata(const uint8_t* src = 0)
            : data(src), thisHalfByte(0), nextHalfByte(0), rleByte(0), firstHalfByte(true), color(0), length(0)
        {
            init(src);
        }
        void init(const uint8_t* src)
        {
            data = src;
            rleByte = 0;
            firstHalfByte = true;
            color = ~0; // Will be flipped to 0 by first call to getNextLength() below
            if (src != 0)
            {
                // Read two half-bytes ahead
                thisHalfByte = getNextHalfByte();
                nextHalfByte = getNextHalfByte();
                getNextLength();
            }
        }
        void skipNext(uint32_t skip)
        {
            for (;;)
            {
                if (length > skip) // is the current length enough?
                {
                    length -= skip; // Reduce the length
                    skip = 0;       // No more to skip
                    break;          // Done!
                }
                else
                {
                    skip -= length;  // Skip the entire run
                    getNextLength(); // Swap colors and Read length of next run
                }
            }
        }
        uint8_t getColor() const
        {
            return color;
        }
        uint32_t getLength() const
        {
            return length;
        }

    private:
        void getNextLength()
        {
            length = thisHalfByte; // Length is the next byte
            // update read ahead buffer
            thisHalfByte = nextHalfByte;
            nextHalfByte = getNextHalfByte();
            color = ~color; // Update the color of next run
            // If number after 'length' is 0
            while (thisHalfByte == 0)
            {
                length <<= 4;           // Multiply length by 16 and
                length += nextHalfByte; // add the number after 0
                // We have used the next two half bytes, read two new ones
                thisHalfByte = getNextHalfByte();
                nextHalfByte = getNextHalfByte();
            }
            if (length == 0)
            {
                getNextLength();
            }
        }
        uint8_t getNextHalfByte()
        {
            if (firstHalfByte) // Start of new byte, read data from BW_RLE stream
            {
                rleByte = *data++;
            }
            uint8_t length = rleByte & 0xF; // Read lower half
            rleByte >>= 4;                  // Shift upper half down to make it ready
            firstHalfByte = !firstHalfByte; // Toggle 'start of byte'
            return length;
        }
        const uint8_t* data;  // Pointer to compressed data (BW_RLE)
        uint8_t thisHalfByte; // The next half byte from the input
        uint8_t nextHalfByte; // The next half byte after 'thisHalfByte'
        uint8_t rleByte;      // Byte read from compressed data
        bool firstHalfByte;   // Are we about to process first half byte of rleByte?
        uint8_t color;        // Current color
        uint32_t length;      // Number of pixels with the given color
    };

    friend class PainterBWBitmap;
};

/**
 * The class LCD1DebugPrinter implements the DebugPrinter interface for printing debug messages
 * on top of 24bit framebuffer.
 *
 * @see DebugPrinter
 */
class LCD1DebugPrinter : public DebugPrinter
{
public:
    virtual void draw(const Rect& rect) const;
};

} // namespace touchgfx

#endif // LCD1BPP_HPP
