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
 * @file touchgfx/widgets/canvas/AbstractPainter.hpp
 *
 * Declares the touchgfx::AbstractPainter class.
 */
#ifndef ABSTRACTPAINTER_HPP
#define ABSTRACTPAINTER_HPP

#include <stdint.h>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/HAL.hpp>

namespace touchgfx
{
/**
 * An abstract class for creating painter classes for drawing canvas widgets. All canvas widgets
 * need a painter to fill the shape drawn with a CanvasWidgetRenderer. The painter must
 * provide the color of a pixel on a given coordinate, which will the be blended into
 * the framebuffer depending on the position of the canvas widget and the transparancy
 * of the given pixel.
 *
 * The AbstractPainter also implements a function which will blend each pixel in a
 * scanline snippet into the framebuffer, but for better performance, the function
 * should be reimplemented in each painter.
 */
class AbstractPainter
{
public:
    /** Initializes a new instance of the AbstractPainter class. */
    AbstractPainter()
        : areaOffsetX(0),
          areaOffsetY(0),
          widgetAlpha(255),
          painterAlpha(255)
    {
    }

    /** Finalizes an instance of the AbstractPainter class. */
    virtual ~AbstractPainter()
    {
    }

    /**
     * Sets the offset of the area being drawn. This allows render() to calculate the x, y
     * relative to the widget, and not just relative to the invalidated area.
     *
     * @param  offsetX The offset x coordinate of the invalidated area relative to the
     *                 widget.
     * @param  offsetY The offset y coordinate of the invalidated area relative to the
     *                 widget.
     *
     * @note Used by CanvasWidgetRenderer - should not be overwritten.
     */
    void setOffset(uint16_t offsetX, uint16_t offsetY)
    {
        areaOffsetX = offsetX;
        areaOffsetY = offsetY;
    }

    /**
     * Paint a designated part of the RenderingBuffer with respect to the amount of coverage
     * of each pixel given by the parameter covers. The cover is the alpha for each pixel,
     * which is what makes it possible to have smooth anti-aliased edges on the shapes drawn
     * with CanvasWidgetRenderer.
     *
     * @param [in] ptr     Pointer to the row in the RenderingBuffer.
     * @param      x       The x coordinate.
     * @param      xAdjust The minor adjustment of x (used when a pixel is smaller than a byte
     *                     to specify that the \a ptr should have been advanced
     *                     "xAdjust" pixels futher into the byte).
     * @param      y       The y coordinate.
     * @param      count   Number of pixels to fill.
     * @param      covers  The coverage in of each pixel.
     *
     * @note The implementation of render() in the AbstractPainter classes is a generic (i.e. slow)
     *       implementation that should be completely implemented in subclasses of
     *       AbstractPainter for better performance.
     */
    virtual void render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers) = 0;

    /** @copydoc Image::setAlpha */
    virtual void setAlpha(uint8_t newAlpha)
    {
        painterAlpha = newAlpha;
    }

    /** @copydoc Image::getAlpha */
    virtual uint8_t getAlpha() const
    {
        return painterAlpha;
    }

protected:
    int16_t areaOffsetX;  ///< The offset x coordinate of the area being drawn.
    int16_t areaOffsetY;  ///< The offset y coordinate of the area being drawn.
    uint8_t widgetAlpha;  ///< The alpha of the widget using the painter.
    uint8_t painterAlpha; ///< The alpha value for the painter

    /**
     * Sets the widget alpha to allow an entire canvas widget to easily be faded without
     * changing the painter of the widget.
     *
     * @param  alpha The alpha.
     *
     * @note Used internally by CanvasWidgetRenderer.
     */
    void setWidgetAlpha(const uint8_t alpha)
    {
        widgetAlpha = alpha;
    }

    /**
     * Helper function to check if the provided bitmap format matches the current
     * framebuffer format.
     *
     * @param  format A bitmap format.
     *
     * @return True if the format matches the framebuffer format, false otherwise.
     */
    FORCE_INLINE_FUNCTION static bool compatibleFramebuffer(Bitmap::BitmapFormat format)
    {
        bool compat = HAL::lcd().framebufferFormat() == format;
        if (HAL::getInstance()->getAuxiliaryLCD())
        {
            compat |= HAL::getInstance()->getAuxiliaryLCD()->framebufferFormat() == format;
        }
        return compat;
    }

    friend class Canvas;
};

} // namespace touchgfx

#endif // ABSTRACTPAINTER_HPP
