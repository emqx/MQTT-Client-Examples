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
 * @file touchgfx/widgets/canvas/CanvasWidget.hpp
 *
 * Declares the touchgfx::CanvasWidget class.
 */
#ifndef CANVASWIDGET_HPP
#define CANVASWIDGET_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/canvas_widget_renderer/Rasterizer.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>
#include <touchgfx/widgets/canvas/CWRUtil.hpp>

namespace touchgfx
{
/**
 * Class for drawing complex polygons on the display using CanvasWidgetRenderer. The
 * CanvasWidget is used by passing it to a Canvas object, drawing the outline of the
 * object and then having CanvasWidget render the outline on the display using the
 * assigned painter.
 */
class CanvasWidget : public Widget
{
public:
    CanvasWidget();

    /**
     * Sets a painter for the CanvasWidget.
     *
     * @param [in] painter The painter for the CanvasWidget.
     *
     * @see getPainter
     *
     * @note If setPainter() is used to change the painter to a different painter, the area
     *       containing the CanvasWidget is not automatically invalidated.
     */
    virtual void setPainter(AbstractPainter& painter);

    /**
     * Gets the current painter for the CanvasWidget.
     *
     * @return The painter.
     *
     * @see setPainter
     */
    virtual AbstractPainter& getPainter() const;

    /**
     * Sets the alpha value for the CanvasWidget. The value can be from 255 (completely solid)
     * to 0 (completely transparent).
     *
     * @param  newAlpha The alpha value.
     *
     * @see getAlpha
     *
     * @note The painter set with setPainter() can also have an alpha value. The alpha value of
     *       the painter and the alpha value of the CanvasWidget are combined to a single alpha
     *       value.
     */
    virtual void setAlpha(uint8_t newAlpha)
    {
        alpha = newAlpha;
    }

    /**
     * Returns the current alpha value.
     *
     * @return Gets the current alpha value of the Box.
     *
     * @see setAlpha
     */
    virtual uint8_t getAlpha() const
    {
        return alpha;
    }

    /**
     * Draws the given invalidated area. If the underlying CanvasWidgetRenderer fail to
     * render the widget (if the widget is too complex), the invalidated area is cut into
     * smaller slices (horizontally) which are then drawn separately. If drawing a single
     * raster line fails, that line is considered too complex and skipped (it is left
     * blank/transparent) and drawing continues on the next raster line.
     *
     * If drawing has failed at least once, which means that the number of horizontal lines
     * draw has been reduced, the number of successfully drawn horizontal lines is
     * remembered for the next invocation of draw(). A future call to draw() would then
     * start off with the reduced number of horizontal lines to prevent potentially drawing
     * the canvas widget in vain, as happened previously in draw().
     *
     * @param  invalidatedArea The invalidated area.
     *
     * @see drawCanvasWidget
     *
     * @note Subclasses of CanvasWidget should implement drawCanvasWidget(), not draw().
     * @note The term "too complex" means that the size of the buffer (assigned to
     *       CanvasWidgetRenderer using CanvasWidgetRenderer::setupBuffer()) is too small.
     */
    virtual void draw(const Rect& invalidatedArea) const;

    /**
     * Invalidates the area covered by this CanvasWidget. Since many widgets are a lot
     * smaller than the actual size of the canvas widget, each widget must be able to tell
     * the smallest rectangle completely containing the shape drawn by the widget. For
     * example a circle arc is typically much smaller than the widget containing the circle.
     *
     * @see getMinimalRect
     */
    virtual void invalidate() const;

    /**
     * Gets minimal rectangle containing the shape drawn by this widget. Default
     * implementation returns the size of the entire widget, but this function should be
     * overwritten in subclasses and return the minimal rectangle containing the shape. See
     * classes such as Circle for example implementations.
     *
     * @return The minimal rectangle containing the shape drawn.
     */
    virtual Rect getMinimalRect() const;

    /**
     * Gets the largest solid (non-transparent) rectangle. Since canvas widgets typically do
     * not have a solid rect, it is recommended to return an empty rectangle.
     *
     * @return The largest solid (non-transparent) rectangle.
     *
     * @see draw
     *
     * @note Function draw() might fail for some horizontal lines due to memory constraints. These
     *       lines will not be drawn and may cause strange display artifacts.
     */
    virtual Rect getSolidRect() const;

    /**
     * Draw canvas widget for the given invalidated area. Similar to draw(), but might be
     * invoked several times with increasingly smaller areas to due to memory constraints
     * from the underlying CanvasWidgetRenderer.
     *
     * @param  invalidatedArea The invalidated area.
     *
     * @return true if the widget was drawn properly, false if not.
     *
     * @see draw
     */
    virtual bool drawCanvasWidget(const Rect& invalidatedArea) const = 0;

private:
    AbstractPainter* canvasPainter;
    mutable int16_t maxRenderLines;
    uint8_t alpha;
};

} // namespace touchgfx

#endif // CANVASWIDGET_HPP
