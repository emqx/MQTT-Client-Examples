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
 * @file common/TouchGFXInit.hpp
 *
 * Declares the touch graphics generic initialization function.
 */
#ifndef TOUCHGFXINIT_HPP
#define TOUCHGFXINIT_HPP

#include <platform/driver/touch/TouchController.hpp>
#include <touchgfx/hal/DMA.hpp>
#include <touchgfx/hal/HAL.hpp>

#include <BitmapDatabase.hpp>
#include <fonts/ApplicationFontProvider.hpp>
#include <texts/TypedTextDatabase.hpp>
#include <gui/common/FrontendHeap.hpp>

static ApplicationFontProvider fontProvider; ///< The font provider

/**
 * The global touchgfx namespace. All TouchGFX framework classes and global functions are placed in this namespace.
 */
namespace touchgfx
{
/// @cond

static Texts texts; ///< The texts

template <class T>
HAL& getHAL(DMA_Interface& dma, LCD& display, TouchController& tc, int16_t width, int16_t height)
{
    static T hal(dma, display, tc, width, height);
    return hal;
}
/// @endcond

/**
 * @globalfn
 */

/**
 * TouchGFX generic initialize.
 *
 * @tparam HALType The class type of the HAL subclass used for this port.
 * @param [in] dma                    Reference to the DMA implementation object to use. Can be of
 *                                    type NoDMA to disable the use of DMA for rendering.
 * @param [in] display                Reference to the LCD renderer implementation (subclass of
 *                                    LCD). Could be either LCD16bpp for RGB565 UIs, or
 *                                    LCD1bpp for monochrome UIs or LCD24bpp for 24bit
 *                                    displays using RGB888 UIs.
 * @param [in] tc                     Reference to the touch controller driver (or
 *                                    NoTouchController to disable touch input).
 * @param      width                  The \a native display width of the actual display, in pixels.
 *                                    This value is irrespective of whether the concrete UI
 *                                    should be portrait or landscape mode. It must match
 *                                    what the display itself is configured as.
 * @param      height                 The \a native display height of the actual display, in
 *                                    pixels. This value is irrespective of whether the
 *                                    concrete UI should be portrait or landscape mode. It
 *                                    must match what the display itself is configured as.
 * @param [in] bitmapCache            Optional pointer to starting address of a memory region in
 *                                    which to place the bitmap cache. Usually in external
 *                                    RAM. Pass 0 if bitmap caching is not used.
 * @param      bitmapCacheSize        Size of bitmap cache in bytes. Pass 0 if bitmap cache is not
 *                                    used.
 * @param      numberOfDynamicBitmaps (Optional) Number of dynamic bitmaps.
 *
 * @return A reference to the allocated (and initialized) HAL object.
 */
template <class HALType>
HAL& touchgfx_generic_init(DMA_Interface& dma, LCD& display, TouchController& tc, int16_t width, int16_t height,
                           uint16_t* bitmapCache, uint32_t bitmapCacheSize, uint32_t numberOfDynamicBitmaps = 0)
{
    HAL& hal = getHAL<HALType>(dma, display, tc, width, height);
    hal.initialize();

    Bitmap::registerBitmapDatabase(BitmapDatabase::getInstance(),
                                   BitmapDatabase::getInstanceSize(),
                                   bitmapCache,
                                   bitmapCacheSize,
                                   numberOfDynamicBitmaps);

    TypedText::registerTexts(&texts);
    Texts::setLanguage(0);

    FontManager::setFontProvider(&fontProvider);

    FrontendHeap& heap = FrontendHeap::getInstance();
    (void)heap; // we need to obtain the reference above to initialize the frontend heap.

    hal.registerEventListener(*(Application::getInstance()));

    return hal;
}

} // namespace touchgfx

#endif // TOUCHGFXINIT_HPP
