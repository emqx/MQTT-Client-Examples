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

#include <touchgfx/containers/progress_indicators/AbstractProgressIndicator.hpp>

namespace touchgfx
{
AbstractProgressIndicator::AbstractProgressIndicator()
    : Container(), rangeMin(0), rangeMax(100), currentValue(0), rangeSteps(100), rangeStepsMin(0)
{
    background.setXY(0, 0);
    Container::add(background);

    Container::add(progressIndicatorContainer);
}

void AbstractProgressIndicator::setBackground(const Bitmap& bmpBackground)
{
    background.setBitmap(bmpBackground);
    Drawable::setWidth(background.getWidth());
    Drawable::setHeight(background.getHeight());
}

void AbstractProgressIndicator::setProgressIndicatorPosition(int16_t x, int16_t y, int16_t width, int16_t height)
{
    progressIndicatorContainer.setPosition(x, y, width, height);

    if (getWidth() < x + width)
    {
        AbstractProgressIndicator::setWidth(x + width);
    }

    if (getHeight() < y + height)
    {
        AbstractProgressIndicator::setHeight(y + height);
    }
}

int16_t AbstractProgressIndicator::getProgressIndicatorX() const
{
    return progressIndicatorContainer.getX();
}

int16_t AbstractProgressIndicator::getProgressIndicatorY() const
{
    return progressIndicatorContainer.getY();
}

int16_t AbstractProgressIndicator::getProgressIndicatorWidth() const
{
    return progressIndicatorContainer.getWidth();
}

int16_t AbstractProgressIndicator::getProgressIndicatorHeight() const
{
    return progressIndicatorContainer.getHeight();
}

void AbstractProgressIndicator::setRange(int16_t min, int16_t max, uint16_t steps /*= 0*/, uint16_t minStep /*= 0*/)
{
    assert(min < max);
    rangeMin = min;
    rangeMax = max;
    setValue(currentValue);
    if (steps == 0)
    {
        rangeSteps = max - min;
    }
    else
    {
        rangeSteps = steps;
    }
    rangeStepsMin = minStep;
    assert(rangeStepsMin < rangeSteps);
}

void AbstractProgressIndicator::getRange(int16_t& min, int16_t& max, uint16_t& steps, uint16_t& minStep) const
{
    min = rangeMin;
    max = rangeMax;
    steps = rangeSteps;
    minStep = rangeStepsMin;
}

void AbstractProgressIndicator::getRange(int16_t& min, int16_t& max, uint16_t& steps) const
{
    min = rangeMin;
    max = rangeMax;
    steps = rangeSteps;
}

void AbstractProgressIndicator::getRange(int16_t& min, int16_t& max) const
{
    min = rangeMin;
    max = rangeMax;
}

void AbstractProgressIndicator::setValue(int value)
{
    int newValue = MAX(value, rangeMin);
    newValue = MIN(newValue, rangeMax);
    currentValue = newValue;
}

int AbstractProgressIndicator::getValue() const
{
    return currentValue;
}

uint16_t AbstractProgressIndicator::getProgress(uint16_t range /*= 100*/) const
{
    if (range == 0)
    {
        return 0;
    }
    int32_t remainder; // Not used here
    // Find out at what step the current value is.
    int32_t step = rangeStepsMin + muldiv(currentValue - rangeMin, rangeSteps - rangeStepsMin, rangeMax - rangeMin, remainder);
    // Scale the step up to [0..range]
    int32_t prog = muldiv(step, range, rangeSteps, remainder);
    return (uint16_t)prog;
}
} // namespace touchgfx
