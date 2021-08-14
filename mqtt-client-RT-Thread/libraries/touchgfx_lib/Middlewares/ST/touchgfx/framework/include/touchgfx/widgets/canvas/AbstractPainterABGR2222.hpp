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
 * @file touchgfx/widgets/canvas/AbstractPainterABGR2222.hpp
 *
 * Declares the touchgfx::AbstractPainterABGR2222 class.
 */
#ifndef ABSTRACTPAINTERABGR2222_HPP
#define ABSTRACTPAINTERABGR2222_HPP

#include <assert.h>
#include <platform/driver/lcd/LCD8bpp_ABGR2222.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>

namespace touchgfx
{
/**
 * The AbstractPainterABGR2222 class is an abstract class for creating a painter to draw on a
 * ABGR2222 display using CanvasWidgetRenderer.
 *
 * @see AbstractPainter
 */
class AbstractPainterABGR2222 : public AbstractPainter
{
public:
    AbstractPainterABGR2222()
        : AbstractPainter(), currentX(0), currentY(0)
    {
        assert(compatibleFramebuffer(Bitmap::ABGR2222) && "The chosen painter only works with ABGR2222 displays");
    }

    virtual void render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers);

    /**
     * @copydoc AbstractPainterRGB565::mixColors(uint16_t,uint16_t,uint8_t)
     */
    FORCE_INLINE_FUNCTION uint8_t mixColors(uint8_t newpix, uint8_t bufpix, uint8_t alpha)
    {
        return mixColors(LCD8bpp_ABGR2222::getRedFromColor((colortype)newpix),
                         LCD8bpp_ABGR2222::getGreenFromColor((colortype)newpix),
                         LCD8bpp_ABGR2222::getBlueFromColor((colortype)newpix), bufpix, alpha);
    }

    /**
     * @copybrief AbstractPainterRGB565::mixColors(uint16_t,uint16_t,uint16_t,uint16_t,uint8_t)
     *
     * @param  R      The red color.
     * @param  G      The green color.
     * @param  B      The blue color.
     * @param  bufpix The buffer pixel value.
     * @param  alpha  The alpha of the R,G,B.
     *
     * @return The result of blending the two colors into a new color.
     */
    FORCE_INLINE_FUNCTION uint8_t mixColors(uint8_t R, uint8_t G, uint8_t B, uint8_t bufpix, uint8_t alpha)
    {
        uint8_t ialpha = 0xFF - alpha;
        uint8_t p_red = LCD8bpp_ABGR2222::getRedFromColor((colortype)bufpix);
        uint8_t p_green = LCD8bpp_ABGR2222::getGreenFromColor((colortype)bufpix);
        uint8_t p_blue = LCD8bpp_ABGR2222::getBlueFromColor((colortype)bufpix);
        uint8_t red = LCD::div255(R * alpha + p_red * ialpha);
        uint8_t green = LCD::div255(G * alpha + p_green * ialpha);
        uint8_t blue = LCD::div255(B * alpha + p_blue * ialpha);
        return LCD8bpp_ABGR2222::getColorFromRGB(red, green, blue);
    }

protected:
    /**
     * @copydoc AbstractPainterRGB565::renderInit
     */
    virtual bool renderInit()
    {
        return true;
    }

    /**
     * @copydoc AbstractPainterRGB565::renderNext
     */
    virtual bool renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha) = 0;

    /**
     * @copydoc AbstractPainterRGB565::renderPixel
     */
    virtual void renderPixel(uint8_t* p, uint8_t red, uint8_t green, uint8_t blue);

    int currentX; ///< Current x coordinate relative to the widget
    int currentY; ///< Current y coordinate relative to the widget
};

} // namespace touchgfx

#endif // ABSTRACTPAINTERABGR2222_HPP
