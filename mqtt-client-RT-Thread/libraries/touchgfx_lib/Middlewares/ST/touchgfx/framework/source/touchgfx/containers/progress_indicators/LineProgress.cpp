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

#include <touchgfx/containers/progress_indicators/LineProgress.hpp>

namespace touchgfx
{
LineProgress::LineProgress()
    : AbstractProgressIndicator(), line(), endX(0), endY(0)
{
    progressIndicatorContainer.add(line);
    line.setPosition(0, 0, getWidth(), getHeight());
}

void LineProgress::setProgressIndicatorPosition(int16_t x, int16_t y, int16_t width, int16_t height)
{
    line.setPosition(0, 0, width, height);

    AbstractProgressIndicator::setProgressIndicatorPosition(x, y, width, height);
}

void LineProgress::setPainter(AbstractPainter& painter)
{
    line.setPainter(painter);
}

void LineProgress::setStart(int x, int y)
{
    startX = CWRUtil::toQ5<int>(x);
    startY = CWRUtil::toQ5<int>(y);
    line.setStart(x, y);
}

void LineProgress::getStart(int& x, int& y) const
{
    x = startX.to<int>();
    y = startY.to<int>();
}

void LineProgress::setEnd(int x, int y)
{
    endX = CWRUtil::toQ5<int>(x);
    endY = CWRUtil::toQ5<int>(y);
}

void LineProgress::getEnd(int& x, int& y) const
{
    x = endX.to<int>();
    y = endY.to<int>();
}

void LineProgress::setLineWidth(int width)
{
    line.setLineWidth(width);
}

int LineProgress::getLineWidth() const
{
    int width;
    line.getLineWidth(width);
    return width;
}

void LineProgress::setLineEndingStyle(Line::LINE_ENDING_STYLE lineEndingStyle)
{
    line.setLineEndingStyle(lineEndingStyle);
}

touchgfx::Line::LINE_ENDING_STYLE LineProgress::getLineEndingStyle() const
{
    return line.getLineEndingStyle();
}

void LineProgress::setAlpha(uint8_t newAlpha)
{
    line.setAlpha(newAlpha);
}

uint8_t LineProgress::getAlpha() const
{
    return line.getAlpha();
}

void LineProgress::setValue(int value)
{
    if (rangeSteps > 0)
    {
        AbstractProgressIndicator::setValue(value);
        int progress = (int)AbstractProgressIndicator::getProgress(rangeSteps);
        CWRUtil::Q5 r(rangeSteps);
        CWRUtil::Q5 p(progress);
        CWRUtil::Q5 x = startX + (endX - startX) / r * p;
        CWRUtil::Q5 y = startY + (endY - startY) / r * p;
        line.updateEnd(x, y);
    }
}
} // namespace touchgfx
