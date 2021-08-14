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
 * @file touchgfx/widgets/canvas/Canvas.hpp
 *
 * Declares the touchgfx::Canvas class.
 */
#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/canvas_widget_renderer/Rasterizer.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>
#include <touchgfx/widgets/canvas/CWRUtil.hpp>
#include <touchgfx/widgets/canvas/CanvasWidget.hpp>

namespace touchgfx
{
/**
 * Class for easy rendering using CanvasWidgetRenderer.
 *
 * The Canvas class will make implementation of a new CanvasWidget very easy. The few
 * simple primitives allows moving a "pen" and drawing the outline of a shape which can
 * then be rendered.
 *
 * The Canvas class has been optimized to eliminate drawing unnecessary lines outside
 * the currently invalidated rectangle.
 */
class Canvas
{
public:
    /**
     * Canvas Constructor. Locks the framebuffer and prepares for drawing only in the
     * allowed area which has been invalidated. The color depth of the LCD is taken into
     * account.
     *
     * @param  _widget         a pointer to the CanvasWidget using this Canvas. Used for
     *                         getting the canvas dimensions.
     * @param  invalidatedArea the are which should be updated.
     * @note Locks the framebuffer.
     */
    Canvas(const CanvasWidget* _widget, const Rect& invalidatedArea);

    /**
     * Finalizes an instance of the Canvas class.
     *
     * @note Unlocks the framebuffer.
     */
    virtual ~Canvas();

    /**
     * Move the current pen position to (x, y). If the pen is outside the drawing area,
     * nothing is done, but the coordinates are saved in case the next operation is lineTo a
     * coordinate which is inside (or on the opposite side of) the drawing area.
     *
     * @param  x The x coordinate for the pen position in CWRUtil::Q5 format.
     * @param  y The y coordinate for the pen position in CWRUtil::Q5 format.
     *
     * @see CWRUtil::Q5, lineTo
     */
    void moveTo(CWRUtil::Q5 x, CWRUtil::Q5 y);

    /**
     * Draw line from the current (x, y) to the new (x, y) as part of the shape being drawn.
     * As for moveTo, lineTo commands completely outside the drawing are are discarded.
     *
     * @param  x The x coordinate for the pen position in CWRUtil::Q5 format.
     * @param  y The y coordinate for the pen position in CWRUtil::Q5 format.
     *
     * @see CWRUtil::Q5, moveTo
     */
    void lineTo(CWRUtil::Q5 x, CWRUtil::Q5 y);

    /**
     * Move the current pen position to (x, y). If the pen is outside (above or below)
     * the drawing area, nothing is done, but the coordinates are saved in case the next
     * operation is lineTo a coordinate which is inside (or on the opposite side of) the
     * drawing area.
     *
     * @tparam T Either int or float.
     * @param  x The x coordinate for the pen position.
     * @param  y The y coordinate for the pen position.
     */
    template <typename T>
    void moveTo(T x, T y)
    {
        moveTo(CWRUtil::toQ5<T>(x), CWRUtil::toQ5<T>(y));
    }

    /**
     * Draw line from the current (x, y) to the new (x, y) as part of the shape being drawn.
     * As for moveTo, lineTo commands completely outside the drawing are are discarded.
     *
     * @tparam T either int or float.
     * @param  x The x coordinate for the pen position.
     * @param  y The y coordinate for the pen position.
     */
    template <typename T>
    void lineTo(T x, T y)
    {
        lineTo(CWRUtil::toQ5<T>(x), CWRUtil::toQ5<T>(y));
    }

    /**
     * Render the graphical shape drawn using moveTo() and lineTo() with the given Painter. The
     * shape is automatically closed, i.e. a lineTo() is automatically inserted connecting the
     * current pen position with the initial pen position given in the first moveTo() command.
     *
     * @param  customAlpha (Optional) Alpha to apply to the entire canvas. Useful if the canvas
     *                     is part of a more complex container setup that needs to be faded.
     *                     Default is solid.
     *
     * @return true if the widget was rendered, false if insufficient memory was available to
     *         render the widget.
     */
    bool render(uint8_t customAlpha = 255);

private:
    // Pointer to the widget using the Canvas
    const CanvasWidget* widget;

    // Invalidate area in Q5 coordinates
    CWRUtil::Q5 invalidatedAreaX;
    CWRUtil::Q5 invalidatedAreaY;
    CWRUtil::Q5 invalidatedAreaWidth;
    CWRUtil::Q5 invalidatedAreaHeight;

    // For drawing
    RenderingBuffer rbuf;
    Rasterizer ras;

    int16_t offsetX, offsetY;

    // Used for optimization of drawing algorithm
    bool enoughMemory;
    bool penUp, penHasBeenDown;
    CWRUtil::Q5 previousX, previousY;
    uint8_t previousOutside;
    uint8_t penDownOutside;
    CWRUtil::Q5 initialX, initialY;

    enum
    {
        POINT_IS_ABOVE = 1 << 0,
        POINT_IS_BELOW = 1 << 1,
        POINT_IS_LEFT = 1 << 2,
        POINT_IS_RIGHT = 1 << 3
    };

    uint8_t isOutside(const CWRUtil::Q5& x, const CWRUtil::Q5& y, const CWRUtil::Q5& width, const CWRUtil::Q5& height) const;

    void transformFrameBufferToDisplay(CWRUtil::Q5& x, CWRUtil::Q5& y) const;

    void close();
};

} // namespace touchgfx

#endif // CANVAS_HPP
