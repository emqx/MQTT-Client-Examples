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
 * @file touchgfx/widgets/canvas/AbstractPainterRGB565.hpp
 *
 * Declares the touchgfx::AbstractPainterRGB565 class.
 */
#ifndef ABSTRACTPAINTERRGB565_HPP
#define ABSTRACTPAINTERRGB565_HPP

#include <assert.h>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>

namespace touchgfx
{
/**
 * The AbstractPainterRGB565 class is an abstract class for creating a painter to draw on a
 * RGB565 display using CanvasWidgetRenderer.
 *
 * @see AbstractPainter
 */
class AbstractPainterRGB565 : public AbstractPainter
{
public:
    static const uint16_t RMASK = 0xF800; ///< Mask for red   (1111100000000000)
    static const uint16_t GMASK = 0x07E0; ///< Mask for green (0000011111100000)
    static const uint16_t BMASK = 0x001F; ///< Mask for blue  (0000000000011111)

    AbstractPainterRGB565()
        : AbstractPainter(), currentX(0), currentY(0)
    {
        assert(compatibleFramebuffer(Bitmap::RGB565) && "The chosen painter only works with RGB565 displays");
    }

    virtual void render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers);

    /**
     * Mix colors from a new pixel and a buffer pixel with the given alpha applied to the
     * new pixel, and the inverse alpha applied to the buffer pixel.
     *
     * @param  newpix The new pixel value.
     * @param  bufpix The buffer pixel value.
     * @param  alpha  The alpha to apply to the new pixel.
     *
     * @return The result of blending the two colors into a new color.
     */
    FORCE_INLINE_FUNCTION uint16_t mixColors(uint16_t newpix, uint16_t bufpix, uint8_t alpha)
    {
        return mixColors(newpix & RMASK, newpix & GMASK, newpix & BMASK, bufpix, alpha);
    }

    /**
     * Mix colors from a new pixel and a buffer pixel with the given alpha applied to the
     * new pixel, and the inverse alpha applied to the buffer pixel.
     *
     * @param  R      The red color (0-31 shifted into #RMASK).
     * @param  G      The green color (0-63 shifted into #GMASK).
     * @param  B      The blue color (0-31 shifted into #BMASK).
     * @param  bufpix The buffer pixel value.
     * @param  alpha  The alpha of the R,G,B.
     *
     * @return The result of blending the two colors into a new color.
     */
    FORCE_INLINE_FUNCTION uint16_t mixColors(uint16_t R, uint16_t G, uint16_t B, uint16_t bufpix, uint8_t alpha)
    {
        uint8_t ialpha = 0xFF - alpha;
        return (((R * alpha + (bufpix & RMASK) * ialpha) / 255) & RMASK)
               | (((G * alpha + (bufpix & GMASK) * ialpha) / 255) & GMASK)
               | (((B * alpha + (bufpix & BMASK) * ialpha) / 255) & BMASK);
    }

protected:
    /**
     * Initialize rendering of a single scan line of pixels for the render. If renderInit
     * returns false, the scanline will not be rendered.
     *
     * @return true if it succeeds, false if it fails.
     */
    virtual bool renderInit()
    {
        return true;
    }

    /**
     * Get the color of the next pixel in the scan line to blend into the framebuffer.
     *
     * @param [out] red   The red.
     * @param [out] green The green.
     * @param [out] blue  The blue.
     * @param [out] alpha The alpha.
     *
     * @return true if the pixel should be painted, false otherwise.
     */
    virtual bool renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha) = 0;

    /**
     * Renders (writes) the specified color into the framebuffer.
     *
     * @param [in] p     pointer into the framebuffer where the given color should be written.
     * @param      red   The red color.
     * @param      green The green color.
     * @param      blue  The blue color.
     */
    virtual void renderPixel(uint16_t* p, uint8_t red, uint8_t green, uint8_t blue);

    int currentX; ///< Current x coordinate relative to the widget
    int currentY; ///< Current y coordinate relative to the widget
};

} // namespace touchgfx

#endif // ABSTRACTPAINTERRGB565_HPP
