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

#include <platform/hal/simulator/sdl2/HALSDL2.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_shape.h>
#include <SDL2/SDL_syswm.h>
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>
#include <vector>
#include <touchgfx/Utils.hpp>
#include <touchgfx/Version.hpp>

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#elif defined(__GNUC__)
#include <sys/stat.h>
#include <sys/types.h>
#endif

#ifdef __GNUC__
#define sprintf_s snprintf
#define freopen_s(pFile, filename, mode, pStream) (((*(pFile)) = freopen((filename), (mode), (pStream))) == NULL)
#define localtime_s(timeinfo, rawtime) memcpy(timeinfo, localtime(rawtime), sizeof(tm))
#define strncpy_s(dst, dstsize, src, srcsize) strncpy(dst, src, dstsize < srcsize ? dstsize : srcsize)
#define wcstombs_s(result, dst, dstsize, src, srcsize) *result = wcstombs(dst, src, dstsize < srcsize ? dstsize : srcsize)
#define memcpy_s(dst, dstsize, src, srcsize) memcpy(dst, src, dstsize < srcsize ? dstsize : srcsize)
#endif

namespace touchgfx
{
static bool isAlive = true;
bool sdl_initialized = false;
static int screenshotcount = 0;
static uint8_t* rotated = NULL;
static uint8_t* tft24bpp = NULL;
static SDL_Window* simulatorWindow = 0;
static SDL_Renderer* simulatorRenderer = 0;

static uint16_t tft_width = 0;
static uint16_t tft_height = 0;
static uint16_t* tft = NULL;
static uint16_t* double_buf = NULL;
static uint16_t* anim_store = NULL;

#if defined(WIN32) || defined(_WIN32)
static DWORD mainThreadHandle;
#endif

static int transferThreadFunc(void* ptr);
static SDL_sem* sem_transfer_ready = 0;
static SDL_sem* sem_transfer_done = 0;

bool HALSDL2::flashInvalidatedRect = false;

void HALSDL2::renderLCD_FrameBufferToMemory(const Rect& _rectToUpdate, uint8_t* frameBuffer)
{
    Rect rectToUpdate = _rectToUpdate;

    if (isSkinActive && currentSkin != 0 && (currentSkin->isOpaque || currentSkin->hasSemiTransparency))
    {
        // Opaque skin must be drawn before the framebuffer
        SDL_Texture* currentSkinTexture = SDL_CreateTextureFromSurface(simulatorRenderer, currentSkin->surface);
        SDL_RenderClear(simulatorRenderer);
        SDL_RenderCopy(simulatorRenderer, currentSkinTexture, NULL, NULL);
        // The skin will overwrite everything, so expand the rect to the entire framebuffer
        rectToUpdate.x = 0;
        rectToUpdate.y = 0;
        rectToUpdate.width = DISPLAY_WIDTH;
        rectToUpdate.height = DISPLAY_HEIGHT;
        SDL_DestroyTexture(currentSkinTexture);
    }

    // This is a hack because SDL2 does not redraw its framebuffer on screen after screensaver
    rectToUpdate.x = 0;
    rectToUpdate.y = 0;
    rectToUpdate.width = DISPLAY_WIDTH;
    rectToUpdate.height = DISPLAY_HEIGHT;

    if (flashInvalidatedRect)
    {
        SDL_Delay(1);
    }

    // Now draw the requested area
    SDL_Surface* framebufferSurface = SDL_CreateRGBSurfaceFrom((void*)frameBuffer, DISPLAY_WIDTH, DISPLAY_HEIGHT, 24, 3 * DISPLAY_WIDTH, 0, 0, 0, 0);
    SDL_Texture* framebufferTexture = SDL_CreateTextureFromSurface(simulatorRenderer, framebufferSurface);

    SDL_Rect srcRect;
    srcRect.x = rectToUpdate.x;
    srcRect.y = rectToUpdate.y;
    srcRect.h = rectToUpdate.height;
    srcRect.w = rectToUpdate.width;

    SDL_Rect dstRect = srcRect;
    dstRect.x = rectToUpdate.x + getCurrentSkinX();
    dstRect.y = rectToUpdate.y + getCurrentSkinY();

    SDL_RenderCopy(simulatorRenderer, framebufferTexture, &srcRect, &dstRect);

    if (isSkinActive && currentSkin != 0 && !(currentSkin->isOpaque || currentSkin->hasSemiTransparency))
    {
        // Non-opaque skin must be drawn last
        SDL_Texture* currentSkinTexture = SDL_CreateTextureFromSurface(simulatorRenderer, currentSkin->surface);
        SDL_RenderCopy(simulatorRenderer, currentSkinTexture, NULL, NULL);
        SDL_DestroyTexture(currentSkinTexture);
    }

    SDL_RenderPresent(simulatorRenderer);
    SDL_DestroyTexture(framebufferTexture);
    SDL_FreeSurface(framebufferSurface);
}

static void sdlCleanup2()
{
#if defined(WIN32) || defined(_WIN32)
    if (mainThreadHandle == GetCurrentThreadId())
#endif
    {
        if (sdl_initialized)
        {
            sdl_initialized = false; // Make sure we don't get in here again
            SDL_DestroyRenderer(simulatorRenderer);
            SDL_DestroyWindow(simulatorWindow);
            SDL_VideoQuit();
            SDL_Quit();
        }
    }
}

Uint32 myTimerCallback2(Uint32 interval, void* param);

Uint32 myTimerCallback2(Uint32 interval, void* /*param*/)
{
    SDL_Event event;
    SDL_UserEvent userevent;

    /* In this example, our callback pushes an SDL_USEREVENT event
     * into the queue, and causes ourself to be called again at the
     * same interval: */

    userevent.type = static_cast<Uint32>(SDL_USEREVENT);
    userevent.code = 0;
    userevent.data1 = 0;
    userevent.data2 = 0;

    event.type = static_cast<Uint32>(SDL_USEREVENT);
    event.user = userevent;

    SDL_PushEvent(&event);

    return interval;
}

bool HALSDL2::sdl_init(int /*argcount*/, char** args)
{
    if (sdl_initialized)
    {
        touchgfx_printf("SDL already initialized\n");
        return false;
    }

#if defined(SIMULATOR) && !defined(__linux__)
    if (getWindowVisible())
    {
        // Create window and hide it ASAP to prevent problems with files being closed when stdio is redirected to the new console
        touchgfx_enable_stdio();        // This will create a console window
        HWND hwnd = GetConsoleWindow(); // Get a handle to the console window
        if (hwnd)                       // If the console exists, quickly hide it
        {
            ShowWindow(hwnd, SW_HIDE);
        }
    }
#endif

#if defined(WIN32) || defined(_WIN32)
    strncpy_s(programPath, sizeof(programPath), args[0], strlen(args[0]));
    char* filenamePos = strrchr(programPath, '\\');
    if (filenamePos)
    {
        filenamePos++; // Skip path separator
    }
    else
    {
        filenamePos = programPath;
    }
    *filenamePos = '\0';
#else
    strncpy_s(programPath, sizeof(programPath), args[0], strlen(args[0]));
    char* filenamePos = strrchr(programPath, '/');
    if (filenamePos)
    {
        filenamePos++; // Skip path separator
    }
    else
    {
        filenamePos = programPath;
    }
    *filenamePos = '\0';
#endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        touchgfx_printf("Unable to init SDL: %s\n", SDL_GetError());
        return false;
    }

    // Allocate framebuffers
    uint32_t bufferSizeInWords = (lcd().framebufferStride() * FRAME_BUFFER_HEIGHT + 1) / 2;
    tft = new uint16_t[bufferSizeInWords];
    tft_width = FRAME_BUFFER_WIDTH;
    tft_height = FRAME_BUFFER_HEIGHT;

    double_buf = new uint16_t[bufferSizeInWords];
    anim_store = new uint16_t[bufferSizeInWords];
    if (lcd().framebufferFormat() == Bitmap::RGB888)
    {
        // RGB888 is already 24bpp, do not allocate a new buffer
        tft24bpp = NULL;
    }
    else
    {
        tft24bpp = new uint8_t[FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT * 3];
    }
    setFrameBufferStartAddresses(tft, double_buf, anim_store);
    rotated = new uint8_t[FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT * 3]; // rotated id 24bpp, hence *3

    recreateWindow(false);
    if (simulatorWindow == NULL)
    {
        touchgfx_printf("Unable to set video mode: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetWindowTitle(simulatorWindow, getWindowTitle());

    SDL_Surface* iconSurface = SDL_CreateRGBSurfaceFrom(icon, 32, 32, 16, 32 * 2, 0xf000, 0x0f00, 0x00f0, 0x000f);
    SDL_SetWindowIcon(simulatorWindow, iconSurface);
    SDL_FreeSurface(iconSurface);

#if defined(WIN32) || defined(_WIN32)
    FILE* stream;
    //sdl has hijacked output and error on windows
    const char* confile = "CONOUT$";
    // ignore error codes from calling freopen_s
    if (!freopen_s(&stream, confile, "w", stdout))
    {
    }
    if (!freopen_s(&stream, confile, "w", stderr))
    {
    }
#endif

    lockDMAToFrontPorch(false);
    atexit(sdlCleanup2);
    sdl_initialized = true;

    if (HAL::getInstance()->getFrameRefreshStrategy() == HAL::REFRESH_STRATEGY_PARTIAL_FRAMEBUFFER)
    {
        assert(HAL::getInstance()->getFrameBufferAllocator() && "Framebuffer allocator must be provided when using REFRESH_STRATEGY_PARTIAL_FRAMEBUFFER");

        sem_transfer_ready = SDL_CreateSemaphore(0);
        sem_transfer_done = SDL_CreateSemaphore(0);
        SDL_Thread* transfer_thread = SDL_CreateThread(transferThreadFunc, "FB TransferThread", (void*)NULL);
        assert(transfer_thread);
    }

    return true;
}

const char* HALSDL2::customTitle = 0;

void HALSDL2::setWindowTitle(const char* title)
{
    customTitle = title;
}

const char* HALSDL2::getWindowTitle()
{
    if (customTitle != 0)
    {
        return customTitle;
    }
    static char title[100];
    sprintf_s(title, 100, "TouchGFX simulator v%d.%d.%d", TOUCHGFX_VERSION_MAJOR, TOUCHGFX_VERSION_MINOR, TOUCHGFX_VERSION_PATCH);
    return title;
}

void HALSDL2::loadSkin(touchgfx::DisplayOrientation orientation, int x, int y)
{
    char path[300];

    assert(sdl_initialized && "Please call sdl_init() before loading a skin");

    SkinInfo* skin;
    const char* name;

    if (orientation == ORIENTATION_PORTRAIT)
    {
        skin = &portraitSkin;
        name = "portrait";
    }
    else
    {
        skin = &landscapeSkin;
        name = "landscape";
    }
    skin->offsetX = 0;
    skin->offsetY = 0;
    sprintf_s(path, 300, "%s%s%s", programPath, name, ".png");
    skin->surface = IMG_Load(path);
    if (skin->surface == 0)
    {
        touchgfx_printf("Unable to load skin image from %s\n", programPath);
    }
    else
    {
        skin->offsetX = x;
        skin->offsetY = y;
        alphaChannelCheck(skin->surface, skin->isOpaque, skin->hasSemiTransparency);
    }
    if (getDisplayOrientation() == orientation)
    {
        updateCurrentSkin();
        recreateWindow(false);
    }
}

void HALSDL2::performDisplayOrientationChange()
{
    HAL::performDisplayOrientationChange();
    updateCurrentSkin();
    recreateWindow(false);
}

void HALSDL2::updateCurrentSkin()
{
    currentSkin = 0;
    if (getDisplayOrientation() == ORIENTATION_PORTRAIT)
    {
        if (portraitSkin.surface != 0)
        {
            currentSkin = &portraitSkin;
        }
    }
    else
    {
        if (landscapeSkin.surface != 0)
        {
            currentSkin = &landscapeSkin;
        }
    }
}

int HALSDL2::getCurrentSkinX() const
{
    return (isSkinActive && currentSkin != 0 && currentSkin->surface != 0) ? currentSkin->offsetX : 0;
}

int HALSDL2::getCurrentSkinY() const
{
    return (isSkinActive && currentSkin != 0 && currentSkin->surface != 0) ? currentSkin->offsetY : 0;
}

int32_t HALSDL2::_xMouse = 0;
int32_t HALSDL2::_yMouse = 0;
int32_t HALSDL2::_x = 0;
int32_t HALSDL2::_y = 0;
bool HALSDL2::isWindowBeingDragged = false;
int HALSDL2::initialWindowX;
int HALSDL2::initialWindowY;
int HALSDL2::initialMouseX;
int HALSDL2::initialMouseY;
bool HALSDL2::_lastTouch = false;
bool HALSDL2::_touches[5] = { false, false, false, false, false };
int HALSDL2::_numTouches = 0;

void HALSDL2::pushTouch(bool down) const
{
    if (_numTouches == 0)
    {
        // Save touch
        _touches[_numTouches++] = down;
    }
    else if ((_numTouches < 4) && (_touches[_numTouches - 1] ^ down)) //lint !e514
    {
        // Only save touch if is different from the last one recorded
        _touches[_numTouches++] = down;
    }
}

bool HALSDL2::popTouch() const
{
    if (_numTouches < 1)
    {
        // Keep returning the same state
        return _lastTouch;
    }
    // Get first item in queue
    _lastTouch = _touches[0];
    // Move items in queue
    for (int i = 0; i < 4; i++)
    {
        _touches[i] = _touches[i + 1];
    }
    _numTouches--;
    return _lastTouch;
}

bool HALSDL2::debugInfoEnabled = false;

void HALSDL2::updateTitle(int32_t x, int32_t y)
{
    char title[500];
    int length = sprintf_s(title, 500, "%s", getWindowTitle());
    if (debugInfoEnabled)
    {
        length += sprintf_s(title + length, 500 - length, " @ %d,%d", x, y);
    }
    if (flashInvalidatedRect)
    {
        length += sprintf_s(title + length, 500 - length, " (flash)");
    }
    if (isSingleStepping())
    {
        length += sprintf_s(title + length, 500 - length, " (step)");
    }
    SDL_SetWindowTitle(simulatorWindow, title);
}

void HALSDL2::alphaChannelCheck(SDL_Surface* surface, bool& isOpaque, bool& hasSemiTransparency)
{
    isOpaque = true;
    hasSemiTransparency = false;
    if (surface->format->BitsPerPixel < 32 || surface->format->BytesPerPixel < 4)
    {
        return;
    }
    uint32_t alpha = surface->format->Amask;
    uint8_t* data = (uint8_t*)surface->pixels;
    for (int y = 0; y < surface->h; y++)
    {
        for (int x = 0; x < surface->w; x++)
        {
            uint32_t a = alpha & *(uint32_t*)(data + y * surface->pitch + x * 4);
            if (a == 0)
            {
                isOpaque = false;
                if (hasSemiTransparency)
                {
                    return;
                }
            }
            else if (a < alpha)
            {
                hasSemiTransparency = true;
                if (!isOpaque)
                {
                    return;
                }
            }
        }
    }
    return;
}

bool HALSDL2::doSampleTouch(int32_t& x, int32_t& y) const
{
    x = _x - getCurrentSkinX();
    y = _y - getCurrentSkinY();

    if (HAL::DISPLAY_ROTATION == rotate90)
    {
        int32_t tmp = x;
        x = y;
        y = DISPLAY_WIDTH - tmp;
    }
    return popTouch();
}

uint8_t HALSDL2::keyPressed = 0;

bool HALSDL2::sampleKey(uint8_t& key)
{
    if (keyPressed)
    {
        key = keyPressed;
        keyPressed = 0;
        return true;
    }
    return false;
}

bool HALSDL2::singleSteppingEnabled = false;
uint16_t HALSDL2::singleSteppingSteps = 0;

void HALSDL2::taskEntry()
{
    uint32_t lastTick = SDL_GetTicks();
    SDL_AddTimer(1, myTimerCallback2, 0); // Start timer

    SDL_Event event;
    while (SDL_WaitEvent(&event) && isAlive)
    {
        switch (event.type)
        {
        case SDL_USEREVENT:
            {
                uint32_t thisTick = SDL_GetTicks();
                int msSinceLastTick = thisTick - lastTick;
                lastTick = thisTick;

                msPassed += msSinceLastTick;
                if (msPassed >= msBetweenTicks)
                {
                    if (singleSteppingEnabled && singleSteppingSteps == 0)
                    {
                        // Eat up extra ms when waiting for next step
                        while (msPassed >= msBetweenTicks)
                        {
                            msPassed -= msBetweenTicks;
                        }
                    }
                    else
                    {
                        while (msPassed >= msBetweenTicks)
                        {
                            msPassed -= msBetweenTicks;
                            vSync();
                        }
                        backPorchExited();
                        frontPorchEntered();
                        if (screenshotcount > 0)
                        {
                            screenshotcount--;
                            saveScreenshot();
                        }
                    }
                    if (singleSteppingEnabled && singleSteppingSteps > 0)
                    {
                        singleSteppingSteps--;
                    }
                }
                break;
            }

        case SDL_MOUSEMOTION:
            {
                _xMouse = event.motion.x;
                _yMouse = event.motion.y;
                if (debugInfoEnabled)
                {
                    updateTitle(_xMouse, _yMouse);
                }
                if ((event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0) //lint !e778 !e845
                {
                    _x = _xMouse;
                    _y = _yMouse;
                    pushTouch(true);
                }
                if (isWindowBeingDragged)
                {
                    int newMouseX;
                    int newMouseY;
                    SDL_GetGlobalMouseState(&newMouseX, &newMouseY);
                    SDL_SetWindowPosition(simulatorWindow, initialWindowX + (newMouseX - initialMouseX), initialWindowY + (newMouseY - initialMouseY));
                }
                break;
            }

        case SDL_MOUSEBUTTONDOWN:
            {
                SDL_CaptureMouse(SDL_TRUE);
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    _x = event.motion.x;
                    _y = event.motion.y;
                    pushTouch(true);
                }
                isWindowBeingDragged = (event.button.button == SDL_BUTTON_RIGHT);
                if (isWindowBeingDragged)
                {
                    SDL_GetWindowPosition(simulatorWindow, &initialWindowX, &initialWindowY);
                    SDL_GetGlobalMouseState(&initialMouseX, &initialMouseY);
                }
                break;
            }

        case SDL_MOUSEBUTTONUP:
            {
                SDL_CaptureMouse(SDL_FALSE);
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    pushTouch(false);
                }
                if (isWindowBeingDragged)
                {
                    int newMouseX;
                    int newMouseY;
                    SDL_GetGlobalMouseState(&newMouseX, &newMouseY);
                    SDL_SetWindowPosition(simulatorWindow, initialWindowX + (newMouseX - initialMouseX), initialWindowY + (newMouseY - initialMouseY));
                    isWindowBeingDragged = false;
                }
                break;
            }

        case SDL_TEXTINPUT:
            if (strlen(event.text.text) == 1)
            {
                keyPressed = (uint8_t)(event.text.text[0]);
            }
            break;

        case SDL_KEYUP:
            {
                if (event.key.keysym.sym == SDLK_F1)
                {
                    debugInfoEnabled = !debugInfoEnabled;
                    updateTitle(_xMouse, _yMouse);
                }
                else if (event.key.keysym.sym == SDLK_F2)
                {
                    flashInvalidatedRect = !flashInvalidatedRect;
                    updateTitle(_xMouse, _yMouse);
                }
                else if (event.key.keysym.sym == SDLK_F3)
                {
                    if (event.key.keysym.mod & KMOD_CTRL)
                    {
                        // Repeat
                        saveNextScreenshots(50);
                    }
                    else if (event.key.keysym.mod & KMOD_SHIFT)
                    {
                        // clipboard
                        copyScreenshotToClipboard();
                    }
                    else if (event.key.keysym.mod & KMOD_ALT)
                    {
                        // Do nothing
                    }
                    else if (event.key.keysym.mod & KMOD_GUI)
                    {
                        // Do nothing
                    }
                    else
                    {
                        // No modifiers
                        saveScreenshot();
                    }
                }
                else if (event.key.keysym.sym == SDLK_F4)
                {
                    if (currentSkin != 0 && currentSkin->surface)
                    {
                        isSkinActive = !isSkinActive;
                    }
                    else
                    {
                        isWindowBorderless = !isWindowBorderless;
                    }
                    recreateWindow();
                }
                else if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    isAlive = false;
                }
                else if (event.key.keysym.sym == SDLK_F9)
                {
                    setSingleStepping(!singleSteppingEnabled);
                }
                else if (event.key.keysym.sym == SDLK_F10)
                {
                    if (singleSteppingEnabled)
                    {
                        singleSteppingSteps++;
                    }
                }
                break;
            }

        case SDL_QUIT:
            {
                isAlive = false;
                break;
            }

        default:
            break;
        }
    }
}

void HALSDL2::recreateWindow(bool updateContent /*= true*/)
{
#if defined(WIN32) || defined(_WIN32)
    mainThreadHandle = GetCurrentThreadId();
#endif

    int windowX = SDL_WINDOWPOS_UNDEFINED;
    int windowY = SDL_WINDOWPOS_UNDEFINED;
    if (simulatorWindow != 0)
    {
        // Save previous coordinates
        SDL_GetWindowPosition(simulatorWindow, &windowX, &windowY);
        SDL_DestroyRenderer(simulatorRenderer);
        SDL_DestroyWindow(simulatorWindow);
    }
    int width = HAL::DISPLAY_WIDTH;
    int height = HAL::DISPLAY_HEIGHT;
    if (isSkinActive && currentSkin != 0)
    {
        width = currentSkin->surface->w;
        height = currentSkin->surface->h;
    }
    if (isSkinActive && currentSkin != 0)
    {
        simulatorWindow = SDL_CreateShapedWindow(getWindowTitle(), windowX, windowY, width, height, SDL_WINDOW_BORDERLESS | (isWindowVisible ? 0 : SDL_WINDOW_HIDDEN));
        SDL_WindowShapeMode mode;
        mode.mode = ShapeModeBinarizeAlpha;
        mode.parameters.binarizationCutoff = 255;
        SDL_SetWindowShape(simulatorWindow, currentSkin->surface, &mode);
        SDL_SetWindowSize(simulatorWindow, width, height);
        SDL_SetWindowPosition(simulatorWindow, windowX, windowY);
    }
    else
    {
        simulatorWindow = SDL_CreateWindow(getWindowTitle(), windowX, windowY, width, height, (isWindowBorderless ? SDL_WINDOW_BORDERLESS : 0) | (isWindowVisible ? 0 : SDL_WINDOW_HIDDEN));
    }
    simulatorRenderer = SDL_CreateRenderer(simulatorWindow, -1, 0);
    SDL_SetRenderDrawBlendMode(simulatorRenderer, SDL_BLENDMODE_ADD);
    if (updateContent)
    {
        Rect display(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
        renderLCD_FrameBufferToMemory(display, doRotate(rotated, scaleTo24bpp(tft24bpp, getTFTFrameBuffer(), lcd().framebufferFormat())));
    }
    updateTitle(_xMouse, _yMouse);
    // Re-add window icon in case
    SDL_Surface* iconSurface = SDL_CreateRGBSurfaceFrom(icon, 32, 32, 16, 32 * 2, 0xf000, 0x0f00, 0x00f0, 0x000f);
    SDL_SetWindowIcon(simulatorWindow, iconSurface);
    SDL_FreeSurface(iconSurface);
}

uint16_t* HALSDL2::getTFTFrameBuffer() const
{
    return tft;
}

static Rect dirty(0, 0, 0, 0);

uint8_t* HALSDL2::scaleTo24bpp(uint8_t* dst, uint16_t* src, Bitmap::BitmapFormat format)
{
    if (format == Bitmap::RGB888)
    {
        return reinterpret_cast<uint8_t*>(src);
    }

    const int width = tft_width;
    const int height = tft_height;

    assert(dst != NULL && "Output buffer for TFT not allocated");
    uint8_t* buffer = reinterpret_cast<uint8_t*>(src);
    uint8_t* orig_dst = dst;
    switch (format)
    {
    case Bitmap::BW:
        for (int srcY = 0; srcY < height; srcY++)
        {
            for (int srcXbyte = 0; srcXbyte < width / 8; srcXbyte++)
            {
                uint8_t bufbyte = *buffer++;
                for (int srcXpixel = 0; srcXpixel < 8; srcXpixel++)
                {
                    uint8_t pixel = ((bufbyte << srcXpixel) & 0xFF) >> 7;
                    uint8_t pixelByte = pixel * 0xFF;
                    *dst++ = pixelByte;
                    *dst++ = pixelByte;
                    *dst++ = pixelByte;
                }
            }
            // Check if there is a partial byte left
            if (width % 8 != 0)
            {
                uint8_t bufbyte = *buffer++;
                for (int srcXpixel = 0; srcXpixel < width % 8; srcXpixel++)
                {
                    uint8_t pixel = ((bufbyte << srcXpixel) & 0xFF) >> 7;
                    uint8_t pixelByte = pixel * 0xFF;
                    *dst++ = pixelByte;
                    *dst++ = pixelByte;
                    *dst++ = pixelByte;
                }
            }
        }
        break;

    case Bitmap::GRAY2:
        for (int srcY = 0; srcY < height; srcY++)
        {
            for (int srcXbyte = 0; srcXbyte < (width * 2) / 8; srcXbyte++)
            {
                uint8_t bufbyte = *buffer++;
                for (int srcXpixel = 0; srcXpixel < 4; srcXpixel++)
                {
                    uint8_t pixel = bufbyte & 3;
                    bufbyte >>= 2;
                    uint8_t pixelByte = pixel * 0x55;
                    *dst++ = pixelByte;
                    *dst++ = pixelByte;
                    *dst++ = pixelByte;
                }
            }
            // Check if there is a partial byte left
            if ((width * 2) % 8 != 0)
            {
                uint8_t bufbyte = *buffer++;
                for (int srcXpixel = 0; srcXpixel < ((width * 2) % 8) / 2; srcXpixel++)
                {
                    uint8_t pixel = bufbyte & 3;
                    bufbyte >>= 2;
                    uint8_t pixelByte = pixel * 0x55;
                    *dst++ = pixelByte;
                    *dst++ = pixelByte;
                    *dst++ = pixelByte;
                }
            }
        }
        break;

    case Bitmap::GRAY4:
        for (int srcY = 0; srcY < height; srcY++)
        {
            for (int srcXbyte = 0; srcXbyte < (width * 4) / 8; srcXbyte++)
            {
                uint8_t bufbyte = *buffer++;
                for (int srcXpixel = 0; srcXpixel < 2; srcXpixel++)
                {
                    uint8_t pixel = bufbyte & 0xF;
                    bufbyte >>= 4;
                    uint8_t pixelByte = pixel * 0x11;
                    *dst++ = pixelByte;
                    *dst++ = pixelByte;
                    *dst++ = pixelByte;
                }
            }
            // Check if there is a partial byte left
            if ((width * 4) % 8 != 0)
            {
                uint8_t bufbyte = *buffer++;
                for (int srcXpixel = 0; srcXpixel < ((width * 4) % 8) / 4; srcXpixel++)
                {
                    uint8_t pixel = bufbyte & 0xF;
                    bufbyte >>= 4;
                    uint8_t pixelByte = pixel * 0x11;
                    *dst++ = pixelByte;
                    *dst++ = pixelByte;
                    *dst++ = pixelByte;
                }
            }
        }
        break;

    case Bitmap::ARGB2222:
        for (int srcY = 0; srcY < height; srcY++)
        {
            for (int srcX = 0; srcX < width; srcX++)
            {
                uint16_t bufword = *buffer++;
                uint8_t r = (bufword >> 4) & 0x3;
                uint8_t g = (bufword >> 2) & 0x3;
                uint8_t b = bufword & 0x3;
                *dst++ = b * 0x55;
                *dst++ = g * 0x55;
                *dst++ = r * 0x55;
            }
        }
        break;

    case Bitmap::ABGR2222:
        for (int srcY = 0; srcY < height; srcY++)
        {
            for (int srcX = 0; srcX < width; srcX++)
            {
                uint16_t bufword = *buffer++;
                uint8_t r = bufword & 0x3;
                uint8_t g = (bufword >> 2) & 0x3;
                uint8_t b = (bufword >> 4) & 0x3;
                *dst++ = b * 0x55;
                *dst++ = g * 0x55;
                *dst++ = r * 0x55;
            }
        }
        break;

    case Bitmap::RGBA2222:
        for (int srcY = 0; srcY < height; srcY++)
        {
            for (int srcX = 0; srcX < width; srcX++)
            {
                uint16_t bufword = *buffer++;
                uint8_t r = (bufword >> 6) & 0x3;
                uint8_t g = (bufword >> 4) & 0x3;
                uint8_t b = (bufword >> 2) & 0x3;
                *dst++ = b * 0x55;
                *dst++ = g * 0x55;
                *dst++ = r * 0x55;
            }
        }
        break;

    case Bitmap::BGRA2222:
        for (int srcY = 0; srcY < height; srcY++)
        {
            for (int srcX = 0; srcX < width; srcX++)
            {
                uint16_t bufword = *buffer++;
                uint8_t r = (bufword >> 2) & 0x3;
                uint8_t g = (bufword >> 4) & 0x3;
                uint8_t b = (bufword >> 6) & 0x3;
                *dst++ = b * 0x55;
                *dst++ = g * 0x55;
                *dst++ = r * 0x55;
            }
        }
        break;

    case Bitmap::RGB565:
        for (int srcY = 0; srcY < height; srcY++)
        {
            for (int srcX = 0; srcX < width; srcX++)
            {
                uint16_t bufword = *src++;
                uint8_t r = (bufword >> 8) & 0xF8;
                uint8_t g = (bufword >> 3) & 0xFC;
                uint8_t b = (bufword << 3) & 0xF8;
                *dst++ = b | (b >> 5);
                *dst++ = g | (g >> 6);
                *dst++ = r | (r >> 5);
            }
        }
        break;

    case Bitmap::ARGB8888:
        {
            uint32_t* src32 = reinterpret_cast<uint32_t*>(src);
            for (int srcY = 0; srcY < height; srcY++)
            {
                for (int srcX = 0; srcX < width; srcX++)
                {
                    uint32_t pixel = *src32++;
                    uint8_t b = pixel & 0xFF;
                    uint8_t g = (pixel >> 8) & 0xFF;
                    uint8_t r = (pixel >> 16) & 0xFF;
                    *dst++ = b;
                    *dst++ = g;
                    *dst++ = r;
                }
            }
        }
        break;
    case Bitmap::BW_RLE:
    case Bitmap::RGB888:
    case Bitmap::L8:
    default:
        assert(0 && "unsupported screen depth");
        break;
    }

    return orig_dst;
}

uint8_t* HALSDL2::doRotate(uint8_t* dst, uint8_t* src)
{
    switch (HAL::DISPLAY_ROTATION)
    {
    case rotate0:
        return src;
    case rotate90:
        for (int srcX = 0; srcX < HAL::DISPLAY_HEIGHT; srcX++)
        {
            for (int srcY = 0; srcY < HAL::DISPLAY_WIDTH; srcY++)
            {
                int dstX = HAL::DISPLAY_WIDTH - (srcY + 1);
                int dstY = srcX;
                for (int i = 0; i < 3; i++)
                {
                    dst[(dstX + dstY * HAL::DISPLAY_WIDTH) * 3 + i] = src[(srcX + srcY * HAL::DISPLAY_HEIGHT) * 3 + i];
                }
            }
        }
        return dst;
    }
    return 0;
}

void HALSDL2::setTFTFrameBuffer(uint16_t* adr)
{
    if (HAL::getInstance()->getFrameRefreshStrategy() != HAL::REFRESH_STRATEGY_PARTIAL_FRAMEBUFFER)
    {
        //save current framebuffer address
        tft = adr;
        renderLCD_FrameBufferToMemory(dirty, doRotate(rotated, scaleTo24bpp(tft24bpp, adr, lcd().framebufferFormat())));
    }
    else
    {
        //wait for transfers to complete
        while (frameBufferAllocator->hasBlockReadyForTransfer())
        {
            FrameBufferAllocatorWaitOnTransfer();
        }

        //always use the original tft buffer as screen memory GRAM
        renderLCD_FrameBufferToMemory(dirty, doRotate(rotated, scaleTo24bpp(tft24bpp, tft, lcd().framebufferFormat())));
    }
    dirty = Rect(0, 0, 0, 0);
}

void HALSDL2::flushFrameBuffer()
{
    Rect display(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    flushFrameBuffer(display);
}

static int transferThreadFunc(void* ptr)
{
    FrameBufferAllocator* fbAllocator = HAL::getInstance()->getFrameBufferAllocator();
    Bitmap::BitmapFormat framebufferFormat = HAL::getInstance()->lcd().framebufferFormat();
    while (1)
    {
        //wait for blocks to transfer
        SDL_SemWait(sem_transfer_ready);

        while (fbAllocator->hasBlockReadyForTransfer())
        {
            Rect transfer_rect;
            const uint8_t* src = fbAllocator->getBlockForTransfer(transfer_rect);
            // touchgfx_printf("transfer: (%d,%d w%d,h%d)\n", transfer_rect.x, transfer_rect.y, transfer_rect.width, transfer_rect.height);

            switch (framebufferFormat)
            {
            case Bitmap::RGB565:
                {
                    const uint16_t* src16 = (const uint16_t*)src;
                    uint16_t* dst16 = tft + transfer_rect.y * tft_width + transfer_rect.x;
                    for (int srcY = 0; srcY < transfer_rect.height; srcY++)
                    {
                        for (int srcX = 0; srcX < transfer_rect.width; srcX++)
                        {
                            *dst16 = *src16;
                            dst16++;
                            src16++;
                        }
                        dst16 += (tft_width /* HAL::DISPLAY_WIDTH*/ - transfer_rect.width);
                    }

                    break;
                }
            case Bitmap::RGB888:
                {
                    uint8_t* dst = (uint8_t*)tft + (transfer_rect.y * tft_width + transfer_rect.x) * 3;
                    for (int srcY = 0; srcY < transfer_rect.height; srcY++)
                    {
                        for (int srcX = 0; srcX < transfer_rect.width; srcX++)
                        {
                            *dst++ = *src++;
                            *dst++ = *src++;
                            *dst++ = *src++;
                        }
                        dst += (tft_width - transfer_rect.width) * 3;
                    }

                    break;
                }
            default:
                assert(!"HALSDL2::REFRESH_STRATEGY_PARTIAL_FRAMEBUFFER only supports 16bit or 24bit framebuffer");
                break;
            }
            fbAllocator->freeBlockAfterTransfer();
        }

        //signal drawing part
        if (SDL_SemValue(sem_transfer_done) == 0)
        {
            SDL_SemPost(sem_transfer_done);
        }
    }
}

void FrameBufferAllocatorWaitOnTransfer()
{
    SDL_SemWait(sem_transfer_done);
}

void FrameBufferAllocatorSignalBlockDrawn()
{
    //signal transfer part
    if (SDL_SemValue(sem_transfer_ready) == 0)
    {
        SDL_SemPost(sem_transfer_ready);
    }
}

void HALSDL2::flushFrameBuffer(const Rect& rect)
{
    if (flashInvalidatedRect)
    {
        SDL_Rect flashRect;
        flashRect.x = rect.x + getCurrentSkinX();
        flashRect.y = rect.y + getCurrentSkinY();
        flashRect.w = rect.width;
        flashRect.h = rect.height;

        SDL_SetRenderDrawBlendMode(simulatorRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(simulatorRenderer, 0, 0, 0, 127);
        SDL_RenderFillRect(simulatorRenderer, &flashRect);
        SDL_RenderPresent(simulatorRenderer);
        SDL_SetRenderDrawBlendMode(simulatorRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(simulatorRenderer, 255, 255, 255, 127);
        SDL_RenderFillRect(simulatorRenderer, &flashRect);
        SDL_RenderPresent(simulatorRenderer);
    }

    if (HAL::getInstance()->getFrameRefreshStrategy() != HAL::REFRESH_STRATEGY_PARTIAL_FRAMEBUFFER)
    {
        dirty.expandToFit(rect);
    }
    else
    {
        frameBufferAllocator->markBlockReadyForTransfer();
        //for testing during transfers.
        //renderLCD_FrameBufferToMemory(dirty, doRotate(rotated, scaleTo24bpp(tft24bpp, tft, lcd().framebufferFormat())));
    }
    HAL::flushFrameBuffer(rect);
}

bool HALSDL2::blockCopy(void* RESTRICT dest, const void* RESTRICT src, uint32_t numBytes)
{
    return HAL::blockCopy(dest, src, numBytes);
}

void HALSDL2::blitSetTransparencyKey(uint16_t /*key*/)
{
}

void HALSDL2::setVsyncInterval(float ms)
{
    msBetweenTicks = ms;
    msPassed = 0.0f;
}

void HALSDL2::saveScreenshot(char* folder, char* filename)
{
    const char* dir = "screenshots";
#if defined(WIN32) || defined(_WIN32)
    CreateDirectory(dir, 0);
#elif defined(__GNUC__)
    mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif

    char fullPathAndName[100];
    if (folder)
    {
        sprintf_s(fullPathAndName, sizeof(fullPathAndName), "%s/%s", dir, folder);
#if defined(WIN32) || defined(_WIN32)
        CreateDirectory(fullPathAndName, 0);
#elif defined(__GNUC__)
        mkdir(fullPathAndName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
        sprintf_s(fullPathAndName, sizeof(fullPathAndName), "%s/%s/%s", dir, folder, filename);
    }
    else
    {
        sprintf_s(fullPathAndName, sizeof(fullPathAndName), "%s/%s", dir, filename);
    }

    int width;
    int height;
    if (SDL_GetRendererOutputSize(simulatorRenderer, &width, &height) == 0)
    {
        // Create an empty surface that will be used to create the screenshot bmp file
        SDL_Surface* windowSurface = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
        if (windowSurface != 0)
        {
            // Read the pixels from the current render target and save them onto the surface
            SDL_RenderReadPixels(simulatorRenderer, NULL, SDL_GetWindowPixelFormat(simulatorWindow), windowSurface->pixels, windowSurface->pitch);

            // Create the bmp screenshot file
            SDL_SaveBMP(windowSurface, fullPathAndName);

            // Destroy the screenshot surface
            SDL_FreeSurface(windowSurface);
        }
    }
}

void HALSDL2::saveScreenshot()
{
    static char lastBaseName[100] = { 0 };
    static int counter = 0;

    // current date/time based on current system
    time_t t = time(0);
    tm localt;
    localtime_s(&localt, &t);

    char baseName[100];
    sprintf_s(baseName, sizeof(baseName), "img_%04d%02d%02d_%02d%02d%02d",
              1900 + localt.tm_year, localt.tm_mon + 1, localt.tm_mday,
              localt.tm_hour, localt.tm_min, localt.tm_sec);

    if (strncmp(baseName, lastBaseName, sizeof(baseName)) == 0)
    {
        // Same as previous time stamp. Add counter.
        counter++;
        sprintf_s(baseName, sizeof(baseName), "%s_%d.bmp", lastBaseName, counter);
    }
    else
    {
        // New time stamp. Save it and clear counter.
        strncpy_s(lastBaseName, sizeof(lastBaseName), baseName, sizeof(baseName));
        counter = 0;
        sprintf_s(baseName, sizeof(baseName), "%s.bmp", lastBaseName);
    }

    saveScreenshot(0, baseName);
}

void HALSDL2::saveNextScreenshots(int n)
{
    screenshotcount += n;
}

void HALSDL2::copyScreenshotToClipboard()
{
#ifdef __linux__
    touchgfx_printf("Copying to clipboard has not been implemented for Linux\n");
#else
    if (!OpenClipboard(NULL))
    {
        touchgfx_printf("Unable to OpenClipboard\n");
        return;
    }

    if (!EmptyClipboard())
    {
        touchgfx_printf("Unable to EmptyClipboard\n");
        return;
    }

    uint8_t* buffer24 = doRotate(rotated, scaleTo24bpp(tft24bpp, getTFTFrameBuffer(), lcd().framebufferFormat()));
    DWORD size_pixels = DISPLAY_WIDTH * DISPLAY_HEIGHT * 3;

    HGLOBAL hMem = GlobalAlloc(GHND, sizeof(BITMAPV5HEADER) + size_pixels);
    if (!hMem)
    {
        touchgfx_printf("Error allocating memory for bitmap data");
        return;
    }

    BITMAPV5HEADER* hdr = (BITMAPV5HEADER*)GlobalLock(hMem);
    if (!hdr)
    {
        touchgfx_printf("Error locking memory for bitmap data");
        GlobalFree(hMem);
        return;
    }

    memset(hdr, 0, sizeof(BITMAPV5HEADER));

    hdr->bV5Size = sizeof(BITMAPV5HEADER);
    hdr->bV5Width = DISPLAY_WIDTH;
    hdr->bV5Height = -DISPLAY_HEIGHT;
    hdr->bV5Planes = 1;
    hdr->bV5BitCount = 24;
    hdr->bV5Compression = BI_RGB;
    hdr->bV5SizeImage = size_pixels;
    hdr->bV5Intent = LCS_GM_GRAPHICS;
    hdr->bV5CSType = 0x57696E20;

    CopyMemory(hdr + 1, buffer24, size_pixels);
    GlobalUnlock(hMem);

    if (!SetClipboardData(CF_DIBV5, hMem))
    {
        touchgfx_printf("Unable to SetClipboardData\n");
    }

    CloseClipboard();
#endif
}

void HALSDL2::setSingleStepping(bool singleStepping /*= true*/)
{
    singleSteppingEnabled = singleStepping;
    singleSteppingSteps = 0;
    updateTitle(_xMouse, _yMouse);
}

bool HALSDL2::isSingleStepping()
{
    return singleSteppingEnabled;
}

void HALSDL2::singleStep(uint16_t steps /*= 1*/)
{
    if (singleSteppingEnabled)
    {
        singleSteppingSteps += steps;
    }
}

#ifdef __linux__
void simulator_printf(const char* format, va_list pArg)
{
    vprintf(format, pArg);
}
#else
char** HALSDL2::getArgv(int* argc)
{
    LPWSTR cmdline = GetCommandLineW();
    LPWSTR* argvw = CommandLineToArgvW(cmdline, argc);
    char** argv = new char* [*argc];
    for (int i = 0; i < *argc; i++)
    {
        char buffer[1000];
        size_t numChars = wcslen(argvw[0]) + 1;
        wcstombs_s(&numChars, buffer, sizeof(buffer), argvw[i], numChars);
        argv[i] = new char[numChars + 1];
        memcpy_s(argv[i], numChars, buffer, numChars);
        argv[i][numChars] = '\0';
    }
    return argv;
}

void simulator_enable_stdio()
{
#ifdef __GNUC__
#define freopen_s(pFile, filename, mode, pStream) (((*(pFile)) = freopen((filename), (mode), (pStream))) == NULL)
#endif
    touchgfx::HAL* hal = touchgfx::HAL::getInstance();
    if (static_cast<touchgfx::HALSDL2*>(hal)->getWindowVisible())
    {
        HWND consoleHwnd = GetConsoleWindow(); // Get handle of console window
        if (!consoleHwnd)                      // No console window yet?
        {
            HWND activeHwnd = GetActiveWindow(); // Remember which window is active

            AllocConsole();                   // Allocate a new console
            consoleHwnd = GetConsoleWindow(); // Get handle of console window

            FILE* dummy;
            freopen_s(&dummy, "CONIN$", "r", stdin); // Redirect stdin/stdout/stderr to the new console
            freopen_s(&dummy, "CONOUT$", "w", stdout);
            freopen_s(&dummy, "CONOUT$", "w", stderr);

            SwitchToThisWindow(activeHwnd, true); // Switch back to the originally active window
        }
        if (consoleHwnd)
        {
            ShowWindow(consoleHwnd, SW_SHOW); // Show/hide it!
        }
    }
    else
    {
        HWND consoleHwnd = GetConsoleWindow(); // Get handle of console window
        if (consoleHwnd)
        {
            ShowWindow(consoleHwnd, SW_HIDE);
        }
    }
}

void simulator_printf(const char* format, va_list pArg)
{
    // Create a console window, if window is visible.
    simulator_enable_stdio();
    if (GetConsoleWindow()) // Only print if we have a console window
    {
        vprintf(format, pArg);
    }
}
#endif

} // namespace touchgfx
