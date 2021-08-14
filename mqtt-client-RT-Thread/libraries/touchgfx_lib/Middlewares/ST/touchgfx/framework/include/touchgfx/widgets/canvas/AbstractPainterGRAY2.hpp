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
 * @file touchgfx/widgets/canvas/AbstractPainterGRAY2.hpp
 *
 * Declares the touchgfx::AbstractPainterGRAY2 class.
 */
#ifndef ABSTRACTPAINTERGRAY2_HPP
#define ABSTRACTPAINTERGRAY2_HPP

#include <assert.h>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>

namespace touchgfx
{
/**
 * The AbstractPainterGRAY2 class is an abstract class for creating a painter to draw on a
 * GRAY2 display using CanvasWidgetRenderer.
 *
 * @see AbstractPainter
 */
class AbstractPainterGRAY2 : public AbstractPainter
{
public:
    AbstractPainterGRAY2()
        : AbstractPainter(), currentX(0), currentY(0)
    {
        assert(compatibleFramebuffer(Bitmap::GRAY2) && "The chosen painter only works with GRAY2 displays");
    }

    virtual void render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers);

protected:
    /**
     * @copydoc AbstractPainterRGB565::renderInit()
     */
    virtual bool renderInit()
    {
        return true;
    }

    /**
     * Get the color of the next pixel in the scan line to blend into the framebuffer.
     *
     * @param [out] gray  The gray color (0-3).
     * @param [out] alpha The alpha.
     *
     * @return true if the pixel should be painted, false otherwise.
     */
    virtual bool renderNext(uint8_t& gray, uint8_t& alpha) = 0;

    /**
     * Renders (writes) the specified color into the framebuffer.
     *
     * @param [in] p      pointer into the framebuffer where the given color should be written.
     * @param      offset The offset to the pixel from the given pointer.
     * @param      gray   The gray color.
     */
    virtual void renderPixel(uint8_t* p, uint16_t offset, uint8_t gray);

    int currentX; ///< Current x coordinate relative to the widget
    int currentY; ///< Current y coordinate relative to the widget
};

} // namespace touchgfx

#endif // ABSTRACTPAINTERGRAY2_HPP
