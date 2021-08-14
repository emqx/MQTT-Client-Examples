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
 * @file touchgfx/containers/progress_indicators/CircleProgress.hpp
 *
 * Declares the touchgfx::CircleProgress class.
 */
#ifndef CIRCLEPROGRESS_HPP
#define CIRCLEPROGRESS_HPP

#include <touchgfx/containers/progress_indicators/AbstractProgressIndicator.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterRGB565.hpp>
#include <touchgfx/widgets/canvas/Circle.hpp>

namespace touchgfx
{
/**
 * A circle progress indicator uses CanvasWidgetRenderer for drawing the arc of a Circle to show
 * progress. This means that the user must create a painter for painting the circle. The
 * circle progress is defined by setting the minimum and maximum angle of the arc.
 *
 * @note As CircleProgress uses CanvasWidgetRenderer, it is important that a buffer is set up
 *       by calling CanvasWidgetRendere::setBuffer().
 */
class CircleProgress : public AbstractProgressIndicator
{
public:
    CircleProgress();

    virtual void setProgressIndicatorPosition(int16_t x, int16_t y, int16_t width, int16_t height);

    /**
     * Sets the painter to use for drawing the circle progress.
     *
     * @param [in] painter The painter.
     *
     * @see Circle::setPainter, AbstractPainter
     */
    virtual void setPainter(AbstractPainter& painter);

    /**
     * Sets the center of the circle / arc.
     *
     * @param  x The x coordinate of the center of the circle.
     * @param  y The y coordinate of the center of the circle.
     */
    virtual void setCenter(int x, int y);

    /**
     * Gets the circle center coordinates.
     *
     * @param [out] x The x coordinate of the center of the circle.
     * @param [out] y The y coordinate of the center of the circle.
     */
    virtual void getCenter(int& x, int& y) const;

    /**
     * Sets the radius of the circle.
     *
     * @param  r The radius.
     *
     * @see Circle::setRadius
     */
    virtual void setRadius(int r);

    /**
     * Gets the radius of the circle.
     *
     * @return The radius.
     */
    virtual int getRadius() const;

    /**
     * Sets line width of the circle. If a line width of zero is specified, it has a special
     * meaning of drawing a filled circle (with the set radius) instead of just the circle arc.
     *
     * @param  width The width of the line (0 produces a filled circle with the given radius).
     *
     * @see Circle::setLineWidth, setRadius
     */
    virtual void setLineWidth(int width);

    /**
     * Gets line width.
     *
     * @return The line width.
     *
     * @see setLineWidth
     */
    virtual int getLineWidth() const;

    /**
     * Sets the cap precision of end of the circle arc. This is not used if line width is
     * zero.
     *
     * @param  precision The cap precision.
     *
     * @see Circle::setCapPrecision, getCapPrecision
     */
    virtual void setCapPrecision(int precision);

    /**
     * Gets the cap precision.
     *
     * @return The cap precision.
     *
     * @see setCapPrecision
     */
    virtual int getCapPrecision() const
    {
        return circle.getCapPrecision();
    }

    /**
     * Sets start and end angle. By swapping end and start angles, circles can progress
     * backwards.
     *
     * @param  startAngle The start angle.
     * @param  endAngle   The end angle.
     */
    virtual void setStartEndAngle(int startAngle, int endAngle);

    /**
     * Gets start angle.
     *
     * @return The start angle.
     *
     * @see setStartEndAngle, getEndAngle
     */
    virtual int getStartAngle() const;

    /**
     * Gets end angle. Beware that the value returned is not related to the current progress
     * of the circle but rather the end point of the circle when it is at 100%.
     *
     * @return The end angle.
     *
     * @see setStartEndAngle
     */
    virtual int getEndAngle() const;

    /**
     * Sets the alpha.
     *
     * Sets the alpha of the Circle. Please note, that the alpha can also be set on the
     * Painter, but this can be controlled directly from the user app, setting alpha for the
     * CircleProgress will set the alpha of the actual circle.
     *
     * @param  alpha The alpha.
     *
     * @see getAlpha
     */
    virtual void setAlpha(uint8_t alpha);

    /**
     * Gets the alpha of the circle.
     *
     * @return The alpha.
     *
     * @see setAlpha
     */
    virtual uint8_t getAlpha() const;

    virtual void setValue(int value);

protected:
    Circle circle;      ///< The circle
    int circleEndAngle; ///< The end angle
};

} // namespace touchgfx

#endif // CIRCLEPROGRESS_HPP
