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
 * @file touchgfx/widgets/canvas/AbstractPainterBW.hpp
 *
 * Declares the touchgfx::AbstractPainterBW class.
 */
#ifndef ABSTRACTPAINTERBW_HPP
#define ABSTRACTPAINTERBW_HPP

#include <assert.h>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>

namespace touchgfx
{
/**
 * The AbstractPainterBW class is an abstract class for creating a painter to draw on a BW
 * display using CanvasWidgetRenderer. Pixels are either set or removed, alpha blending
 * (and transparancy) is not supported.
 *
 * @see AbstractPainter
 */
class AbstractPainterBW : public AbstractPainter
{
public:
    AbstractPainterBW()
        : currentX(0), currentY(0)
    {
        assert(compatibleFramebuffer(Bitmap::BW) && "The chosen painter only works with BW displays");
    }

    virtual void render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers);

protected:
    /**
     * @copydoc AbstractPainterABGR2222::renderInit()
     */
    virtual bool renderInit()
    {
        return true;
    }

    /**
     * Get the color of the next pixel in the scan line to blend into the framebuffer.
     *
     * @param [out] color The color (0 or 1).
     *
     * @return true if the pixel should be painted, false otherwise.
     */
    virtual bool renderNext(uint8_t& color) = 0;

    uint16_t currentX; ///< Current x coordinate relative to the widget
    uint16_t currentY; ///< Current y coordinate relative to the widget
};

} // namespace touchgfx

#endif // ABSTRACTPAINTERBW_HPP
