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

#include <touchgfx/containers/progress_indicators/BoxProgress.hpp>

namespace touchgfx
{
BoxProgress::BoxProgress()
    : AbstractDirectionProgress(), box()
{
    progressIndicatorContainer.add(box);
}

void BoxProgress::setProgressIndicatorPosition(int16_t x, int16_t y, int16_t width, int16_t height)
{
    box.setPosition(0, 0, width, height);

    AbstractProgressIndicator::setProgressIndicatorPosition(x, y, width, height);
}

void BoxProgress::setColor(colortype color)
{
    box.setColor(color);
}

touchgfx::colortype BoxProgress::getColor() const
{
    return box.getColor();
}

void BoxProgress::setAlpha(uint8_t newAlpha)
{
    box.setAlpha(newAlpha);
}

uint8_t BoxProgress::getAlpha() const
{
    return box.getAlpha();
}

void BoxProgress::setValue(int value)
{
    AbstractProgressIndicator::setValue(value);
    int16_t progress = 0;
    switch (progressDirection)
    {
    case AbstractDirectionProgress::RIGHT:
    case AbstractDirectionProgress::LEFT:
        progress = AbstractProgressIndicator::getProgress(progressIndicatorContainer.getWidth());
        break;
    case AbstractDirectionProgress::DOWN:
    case AbstractDirectionProgress::UP:
        progress = AbstractProgressIndicator::getProgress(progressIndicatorContainer.getHeight());
        break;
    }
    switch (progressDirection)
    {
    case AbstractDirectionProgress::RIGHT:
        {
            int16_t oldWidth = box.getWidth();
            box.setPosition(0, 0, progress, progressIndicatorContainer.getHeight());
            int16_t newWidth = box.getWidth();
            Rect rect(MIN(oldWidth, newWidth), 0, abs(oldWidth - newWidth), box.getHeight());
            progressIndicatorContainer.invalidateRect(rect);
            break;
        }
    case AbstractDirectionProgress::LEFT:
        {
            int16_t oldX = box.getX();
            box.setPosition(getWidth() - progress, 0, progress, progressIndicatorContainer.getHeight());
            int16_t newX = box.getX();
            Rect rect(MIN(oldX, newX), 0, abs(oldX - newX), box.getHeight());
            progressIndicatorContainer.invalidateRect(rect);
            break;
        }
    case AbstractDirectionProgress::DOWN:
        {
            int16_t oldHeight = box.getHeight();
            box.setPosition(0, 0, progressIndicatorContainer.getWidth(), progress);
            int16_t newHeight = box.getHeight();
            Rect rect(0, MIN(oldHeight, newHeight), box.getWidth(), abs(oldHeight - newHeight));
            progressIndicatorContainer.invalidateRect(rect);
            break;
        }
    case AbstractDirectionProgress::UP:
        {
            int16_t oldY = box.getY();
            box.setPosition(0, progressIndicatorContainer.getHeight() - progress, progressIndicatorContainer.getWidth(), progress);
            int16_t newY = box.getY();
            Rect rect(0, MIN(oldY, newY), box.getWidth(), abs(oldY - newY));
            progressIndicatorContainer.invalidateRect(rect);
            break;
        }
    }
}
} // namespace touchgfx
