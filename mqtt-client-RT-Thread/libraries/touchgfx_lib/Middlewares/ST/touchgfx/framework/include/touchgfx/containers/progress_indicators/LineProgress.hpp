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
 * @file touchgfx/containers/progress_indicators/LineProgress.hpp
 *
 * Declares the touchgfx::LineProgress class.
 */
#ifndef LINEPROGRESS_HPP
#define LINEPROGRESS_HPP

#include <touchgfx/containers/progress_indicators/AbstractProgressIndicator.hpp>
#include <touchgfx/widgets/canvas/AbstractPainter.hpp>
#include <touchgfx/widgets/canvas/Line.hpp>

namespace touchgfx
{
/**
 * Using Line from CanvasWidgetRenderer, progress will be rendered as a line. This means that
 * the user must create a painter for painting the circle. The line does not need to
 * horizontal or vertical, but can start at any coordinate and finish at any coordinate.
 *
 * @note As LineProgress uses CanvasWidgetRenderer, it is important that a buffer is set up by
 *       calling CanvasWidgetRendere::setBuffer().
 */
class LineProgress : public AbstractProgressIndicator
{
public:
    LineProgress();

    virtual void setProgressIndicatorPosition(int16_t x, int16_t y, int16_t width, int16_t height);

    /**
     * Sets a painter to be used for drawing the line. This can be any Painter, a simple
     * single color painter, a bitmap painter or a custom painter.
     *
     * @param [in] painter The painter.
     */
    virtual void setPainter(AbstractPainter& painter);

    /**
     * Sets a starting point for the line.
     *
     * @param  x The x coordinate of the start point.
     * @param  y The y coordinate of the start point.
     *
     * @see setEnd
     */
    virtual void setStart(int x, int y);

    /**
     * Gets the coordinates of the starting point of the line.
     *
     * @param [out] x The x coordinate.
     * @param [out] y The y coordinate.
     */
    virtual void getStart(int& x, int& y) const;

    /**
     * Sets the end point for the line. When progress is at 100%, the line will go from the
     * coordinates set by setStart() to the coordinates set by setEnd()
     *
     * @param  x The x coordinate of the end point.
     * @param  y The y coordinate of the end point.
     *
     * @see setStart
     */
    virtual void setEnd(int x, int y);

    /**
     * Gets the coordinates of the end point of the line. Beware that this is not the
     * coordinates of the current progress of the line, but the coordinates when the line is
     * at 100%.
     *
     * @param [out] x The x coordinate.
     * @param [out] y The y coordinate.
     */
    virtual void getEnd(int& x, int& y) const;

    /**
     * Sets the line width.
     *
     * @param  width The width.
     *
     * @see Line::setLineWidth
     */
    virtual void setLineWidth(int width);

    /**
     * Gets the line width.
     *
     * @return The line width.
     */
    virtual int getLineWidth() const;

    /**
     * Sets line ending style.
     *
     * @param  lineEndingStyle The line ending style.
     *
     * @see Line::setLineEndingStyle
     */
    virtual void setLineEndingStyle(Line::LINE_ENDING_STYLE lineEndingStyle);

    /**
     * Gets line ending style.
     *
     * @return The line ending style.
     */
    virtual Line::LINE_ENDING_STYLE getLineEndingStyle() const;

    /**
     * @copydoc Image::setAlpha
     */
    virtual void setAlpha(uint8_t newAlpha);

    /**
     * @copydoc Image::getAlpha
     */
    virtual uint8_t getAlpha() const;

    virtual void setValue(int value);

protected:
    Line line;          ///< The line
    CWRUtil::Q5 startX; ///< The start x coordinate
    CWRUtil::Q5 startY; ///< The start y coordinate
    CWRUtil::Q5 endX;   ///< The end x coordinate
    CWRUtil::Q5 endY;   ///< The end y coordinate
};

} // namespace touchgfx

#endif // LINEPROGRESS_HPP
