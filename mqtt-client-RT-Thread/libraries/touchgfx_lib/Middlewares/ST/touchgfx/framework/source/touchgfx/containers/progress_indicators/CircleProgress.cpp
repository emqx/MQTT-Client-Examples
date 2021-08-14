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

#include <touchgfx/containers/progress_indicators/CircleProgress.hpp>

namespace touchgfx
{
CircleProgress::CircleProgress()
    : AbstractProgressIndicator(), circle()
{
    progressIndicatorContainer.add(circle);
    circle.setPosition(0, 0, getWidth(), getHeight());
    CircleProgress::setStartEndAngle(0, 360);
}

void CircleProgress::setProgressIndicatorPosition(int16_t x, int16_t y, int16_t width, int16_t height)
{
    circle.setPosition(0, 0, width, height);

    AbstractProgressIndicator::setProgressIndicatorPosition(x, y, width, height);
}

void CircleProgress::setPainter(AbstractPainter& painter)
{
    circle.setPainter(painter);
}

void CircleProgress::setCenter(int x, int y)
{
    circle.setCenter(x, y);
}

void CircleProgress::getCenter(int& x, int& y) const
{
    circle.getCenter(x, y);
}

void CircleProgress::setRadius(int r)
{
    circle.setRadius(r);
}

int CircleProgress::getRadius() const
{
    int radius;
    circle.getRadius(radius);
    return radius;
}

void CircleProgress::setLineWidth(int width)
{
    circle.setLineWidth(width);
}

int CircleProgress::getLineWidth() const
{
    int width;
    circle.getLineWidth(width);
    return width;
}

void CircleProgress::setCapPrecision(int precision)
{
    circle.setCapPrecision(precision);
}

void CircleProgress::setStartEndAngle(int startAngle, int endAngle)
{
    assert(startAngle != endAngle);
    circle.setArc(startAngle, endAngle);
    circleEndAngle = endAngle;
    CircleProgress::setValue(CircleProgress::getValue());
}

int CircleProgress::getStartAngle() const
{
    return circle.getArcStart();
}

int CircleProgress::getEndAngle() const
{
    return circleEndAngle;
}

void CircleProgress::setAlpha(uint8_t alpha)
{
    circle.setAlpha(alpha);
}

uint8_t CircleProgress::getAlpha() const
{
    return circle.getAlpha();
}

void CircleProgress::setValue(int value)
{
    CWRUtil::Q5 startAngle;
    CWRUtil::Q5 endAngle = CWRUtil::toQ5(circleEndAngle);
    circle.getArcStart<CWRUtil::Q5>(startAngle);
    AbstractProgressIndicator::setValue(value);
    uint16_t rangeAngleSteps = endAngle < startAngle ? (int)(startAngle - endAngle) : (int)(endAngle - startAngle);
    CWRUtil::Q5 progress = CWRUtil::Q5(AbstractProgressIndicator::getProgress(rangeAngleSteps));
    if (endAngle < startAngle)
    {
        circle.updateArcEnd<CWRUtil::Q5>(startAngle - progress);
    }
    else
    {
        circle.updateArcEnd<CWRUtil::Q5>(startAngle + progress);
    }
}
} // namespace touchgfx
