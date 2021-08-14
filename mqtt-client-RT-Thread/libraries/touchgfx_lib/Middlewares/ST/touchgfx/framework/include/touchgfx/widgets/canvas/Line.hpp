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
 * @file touchgfx/widgets/canvas/Line.hpp
 *
 * Declares the touchgfx::Line class.
 */
#ifndef LINE_HPP
#define LINE_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/Widget.hpp>

#include <touchgfx/widgets/canvas/Canvas.hpp>
#include <touchgfx/widgets/canvas/CanvasWidget.hpp>

namespace touchgfx
{
/**
 * Simple CanvasWidget capable of drawing a line from one point to another point. The end points
 * can be moved to new locations and the line width can be set and changed. A 10 pixel
 * long line along the top of the screen with a width on 1 pixel has endpoints in (0,
 * 0.5) and (10, 0.5) and line width 1. The Line class calculates the corners of the
 * shape, which in this case would be (0, 0), (10, 0), (10, 1) and (0, 1)
 * and tells CanvasWidgetRenderer to moveTo() the first coordinate and then lineTo() the
 * next coordinates in order. Finally it tells CWR to render the inside of the shape
 * using the set Painter object.
 *
 * The Line class caches the four corners of the shape to speed up redrawing. In general,
 * drawing lines involve some extra mathematics for calculating the normal vector of the
 * line and this computation would slow down re-draws if not cached.
 *
 * @note All coordinates are internally handled as CWRUtil::Q5 which means that floating point
 *       values are rounded down to a fixed number of binary digits, for example:
 *       @code
 *            Line line;
 *            line.setStart(1.1f, 1.1f); // Will use (35/32, 35/32) = (1.09375f, 1.09375f)
 *            int x, y;
 *            line.getStart(&amp;x, &amp;y); // Will return (1, 1)
 *       @endcode.
 */
class Line : public CanvasWidget
{
public:
    /** Values that represent line ending styles. */
    enum LINE_ENDING_STYLE
    {
        BUTT_CAP_ENDING,  ///< The line ending is cut 90 degrees at the end of the line
        ROUND_CAP_ENDING, ///< The line ending is rounded as a circle with center at the end of the line
        SQUARE_CAP_ENDING ///< The line ending is cut 90 degrees, but extends half the width of the line
    };

    Line();

    /**
     * Sets the starting point and ending point of the line.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param  x1 The x coordinate of the start point.
     * @param  y1 The y coordinate of the start point.
     * @param  x2 The x coordinate of the end point.
     * @param  y2 The y coordinate of the end point.
     *
     * @see setStart, setEnd
     *
     * @note The area containing the Line is not invalidated.
     */
    template <typename T>
    void setLine(T x1, T y1, T x2, T y2)
    {
        setStart(x1, y1);
        setEnd(x2, y2);
    }

    /**
     * Sets the starting point of the line.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param  x The x coordinate of the start point.
     * @param  y The y coordinate of the start point.
     *
     * @see updateStart, getStart, setLine, setEnd
     *
     * @note The area containing the Line is not invalidated.
     */
    template <typename T>
    void setStart(T x, T y)
    {
        setStart(CWRUtil::toQ5<T>(x), CWRUtil::toQ5<T>(y));
    }

    /**
     * Sets the starting point of the line.
     *
     * @param  xQ5 The x coordinate of the start point in Q5 format.
     * @param  yQ5 The y coordinate of the start point in Q5 format.
     *
     * @see updateStart, getStart, setLine, setEnd
     *
     * @note The area containing the Line is not invalidated.
     */
    void setStart(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5);

    /**
     * Update the start point for this Line. The rectangle that surrounds the line before
     * and after will be invalidated.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param  x The x coordinate of the start point.
     * @param  y The y coordinate of the start point.
     *
     * @see setStart, updateEnd
     *
     * @note The area containing the Line is invalidated before and after the change.
     */
    template <typename T>
    void updateStart(T x, T y)
    {
        updateStart(CWRUtil::toQ5<T>(x), CWRUtil::toQ5<T>(y));
    }

    /**
     * Update the start point for this Line. The rectangle that surrounds the line before
     * and after will be invalidated.
     *
     * @param  xQ5 The x coordinate of the start point in CWRUtil::Q5 format.
     * @param  yQ5 The y coordinate of the start point in CWRUtil::Q5 format.
     *
     * @see setStart, updateEnd
     *
     * @note The area containing the Line is invalidated before and after the change.
     */
    void updateStart(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5);

    /**
     * Gets the starting point of the line as either integers or floats.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param [out] x The x coordinate rounded down to the precision of T.
     * @param [out] y The y coordinate rounded down to the precision of T.
     *
     * @see setStart, setLine
     */
    template <typename T>
    void getStart(T& x, T& y) const
    {
        x = x1.to<T>();
        y = y1.to<T>();
    }

    /**
     * Sets the endpoint coordinates of the line.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param  x The x coordinate of the end point.
     * @param  y The y coordinate of the end point.
     *
     * @see updateEnd, getEnd
     *
     * @note The area containing the Line is not invalidated.
     */
    template <typename T>
    void setEnd(T x, T y)
    {
        setEnd(CWRUtil::toQ5<T>(x), CWRUtil::toQ5<T>(y));
    }

    /**
     * Sets the endpoint coordinates of the line.
     *
     * @param  xQ5 The x coordinate of the end point in Q5 format.
     * @param  yQ5 The y coordinate of the end point in Q5 format.
     *
     * @see updateEnd, getEnd
     *
     * @note The area containing the Line is not invalidated.
     */
    void setEnd(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5);

    /**
     * Update the endpoint for this Line. The rectangle that surrounds the line before and
     * after will be invalidated.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param  x The x coordinate of the end point.
     * @param  y The y coordinate of the end point.
     *
     * @see setEnd, updateStart
     *
     * @note The area containing the Line is invalidated before and after the change.
     */
    template <typename T>
    void updateEnd(T x, T y)
    {
        CWRUtil::Q5 xQ5 = CWRUtil::toQ5<T>(x);
        CWRUtil::Q5 yQ5 = CWRUtil::toQ5<T>(y);
        updateEnd(xQ5, yQ5);
    }

    /**
     * Update the endpoint for this Line. The rectangle that surrounds the line before and
     * after will be invalidated.
     *
     * @param  xQ5 The x coordinate of the end point in Q5 format.
     * @param  yQ5 The y coordinate of the end point in Q5 format.
     *
     * @see setEnd, updateStart
     *
     * @note The area containing the Line is invalidated before and after the change.
     */
    void updateEnd(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5);

    /**
     * Gets the endpoint coordinates for the line.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param [out] x The x coordinate rounded down to the precision of T.
     * @param [out] y The y coordinate rounded down to the precision of T.
     *
     * @see setEnd, updateEnd
     */
    template <typename T>
    void getEnd(T& x, T& y) const
    {
        x = x2.to<T>();
        y = y2.to<T>();
    }

    /**
     * Sets the width for this Line.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param  width The width of the line measured in pixels.
     *
     * @see updateLineWidth
     *
     * @note The area containing the Line is not invalidated.
     */
    template <typename T>
    void setLineWidth(T width)
    {
        setLineWidth(CWRUtil::toQ5<T>(width));
    }

    /**
     * Sets the width for this Line.
     *
     * @param  widthQ5 The width of the line measured in pixels in Q5 format.
     *
     * @see updateLineWidth
     *
     * @note The area containing the Line is not invalidated.
     */
    void setLineWidth(CWRUtil::Q5 widthQ5)
    {
        if (lineWidth == widthQ5)
        {
            return;
        }

        lineWidth = widthQ5;

        updateCachedShape();
    }

    /**
     * Update the width for this Line and invalidates the minimal rectangle surrounding the
     * line on screen.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param  width The width of the line measured in pixels.
     *
     * @see setLineWidth
     *
     * @note The area containing the Line is invalidated before and after the change.
     */
    template <typename T>
    void updateLineWidth(T width)
    {
        updateLineWidth(CWRUtil::toQ5<T>(width));
    }

    /**
     * Update the width for this Line.
     *
     * Update the width for this Line and invalidates the minimal rectangle surrounding the
     * line on screen.
     *
     * @param  widthQ5 The width of the line measured in pixels in Q5 format.
     *
     * @see setLineWidth
     *
     * @note The area containing the Line is invalidated before and after the change.
     */
    void updateLineWidth(CWRUtil::Q5 widthQ5)
    {
        if (lineWidth == widthQ5)
        {
            return;
        }

        Rect rectBefore = getMinimalRect();

        lineWidth = widthQ5;

        updateCachedShape();

        Rect rectAfter = getMinimalRect();
        rectBefore.expandToFit(rectAfter);
        invalidateRect(rectBefore);
    }

    /**
     * Gets line width.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param [out] width The line width rounded down to the precision of T.
     *
     * @see setLineWidth
     */
    template <typename T>
    void getLineWidth(T& width) const
    {
        width = lineWidth.to<T>();
    }

    /**
     * Gets line width.
     *
     * @tparam T Generic type parameter, either int or float.
     *
     * @return The line width rounded down to the precision of T.
     *
     * @see setLineWidth
     */
    template <typename T>
    T getLineWidth() const
    {
        return lineWidth.to<T>();
    }

    /**
     * Sets line ending style. The same style is applied to both ends of the line.
     *
     * @param  lineEnding The line ending style.
     *
     * @see LINE_ENDING_STYLE, getLineEndingStyle
     *
     * @note The area containing the Line is not invalidated.
     */
    void setLineEndingStyle(LINE_ENDING_STYLE lineEnding);

    /**
     * Gets line ending style.
     *
     * @return The line ending style.
     *
     * @see LINE_ENDING_STYLE, setLineEndingStyle
     */
    LINE_ENDING_STYLE getLineEndingStyle() const;

    /**
     * Sets a precision of the arc at the ends of the Line. This only works for
     * ROUND_CAP_ENDING. The precision is given in degrees where 18 is the default which
     * results in a nice half circle with 10 line segments. 90 will draw "an arrow head",
     * 180 will look exactly like a BUTT_CAP_ENDING.
     *
     * @param  precision The new ROUND_CAP_ENDING precision.
     *
     * @note The line is not invalidated.
     * @note This is only used if line ending is set to ROUND_CAP_ENDING.
     */
    void setCapPrecision(int precision);

    virtual bool drawCanvasWidget(const Rect& invalidatedArea) const;

    virtual Rect getMinimalRect() const;

    /**
     * Update the end point for this Line given the new length and angle. The rectangle that
     * surrounds the line before and after will be invalidated. The starting coordinates
     * will be fixed but the ending point will be updated. This is simply a different way to
     * update the ending point.
     *
     * @param  length The new length of the line in Q5 format.
     * @param  angle  The new angle of the line in Q5 format.
     *
     * @see updateEnd
     *
     * @note The area containing the Line is invalidated before and after the change.
     */
    void updateLengthAndAngle(CWRUtil::Q5 length, CWRUtil::Q5 angle);

private:
    CWRUtil::Q5 x1, y1, x2, y2;
    CWRUtil::Q5 lineWidth;
    LINE_ENDING_STYLE lineEnding;
    CWRUtil::Q5 xCorner[4], yCorner[4];
    Rect minimalRect;
    int lineCapArcIncrement;

    void updateCachedShape();
};

} // namespace touchgfx

#endif // LINE_HPP
