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

#include <touchgfx/containers/clock/AnalogClock.hpp>

namespace touchgfx
{
AnalogClock::AnalogClock() :
    AbstractClock(),
    animationEquation(EasingEquations::linearEaseNone),
    animationDuration(0),
    clockRotationCenterX(0),
    clockRotationCenterY(0),
    lastHour(0),
    lastMinute(0),
    lastSecond(0),
    hourHandMinuteCorrectionActive(false),
    minuteHandSecondCorrectionActive(false)
{
    Container::add(background);

    hourHand.updateZAngle(0.f);
    minuteHand.updateZAngle(0.f);
    secondHand.updateZAngle(0.f);

    hourHand.setVisible(false);
    minuteHand.setVisible(false);
    secondHand.setVisible(false);
}

void AnalogClock::setBackground(const BitmapId backgroundBitmapId)
{
    setBackground(backgroundBitmapId, Bitmap(backgroundBitmapId).getWidth() / 2, Bitmap(backgroundBitmapId).getHeight() / 2);
}

void AnalogClock::setBackground(const BitmapId backgroundBitmapId, const int16_t rotationCenterX, const int16_t rotationCenterY)
{
    background.setBitmap(Bitmap(backgroundBitmapId));

    clockRotationCenterX = rotationCenterX;
    clockRotationCenterY = rotationCenterY;

    setWidth(background.getWidth());
    setHeight(background.getHeight());
}

void AnalogClock::setRotationCenter(int16_t rotationCenterX, int16_t rotationCenterY)
{
    clockRotationCenterX = rotationCenterX;
    clockRotationCenterY = rotationCenterY;
}

void AnalogClock::setupHourHand(const BitmapId hourHandBitmapId, int16_t rotationCenterX, int16_t rotationCenterY)
{
    setupHand(hourHand, hourHandBitmapId, rotationCenterX, rotationCenterY);
}

void AnalogClock::setupMinuteHand(const BitmapId minuteHandBitmapId, int16_t rotationCenterX, int16_t rotationCenterY)
{
    setupHand(minuteHand, minuteHandBitmapId, rotationCenterX, rotationCenterY);
}

void AnalogClock::setupSecondHand(const BitmapId secondHandBitmapId, int16_t rotationCenterX, int16_t rotationCenterY)
{
    setupHand(secondHand, secondHandBitmapId, rotationCenterX, rotationCenterY);
}

void AnalogClock::setupHand(TextureMapper& hand, const BitmapId bitmapId, int16_t rotationCenterX, int16_t rotationCenterY)
{
    remove(hand);

    hand.setBitmap(Bitmap(bitmapId));
    hand.setWidth(getWidth());
    hand.setHeight(getHeight());
    hand.setXY(0, 0);
    hand.setBitmapPosition(clockRotationCenterX - rotationCenterX, clockRotationCenterY - rotationCenterY);
    hand.setCameraDistance(300.0f);
    hand.setOrigo((float)clockRotationCenterX, (float)clockRotationCenterY, hand.getCameraDistance());
    hand.setCamera(hand.getOrigoX(), hand.getOrigoY());
    hand.setRenderingAlgorithm(TextureMapper::BILINEAR_INTERPOLATION);

    add(hand);
    hand.setVisible(true);
}

void AnalogClock::initializeTime24Hour(uint8_t hour, uint8_t minute, uint8_t second)
{
    lastHour = 255;
    lastMinute = 255;
    lastSecond = 255;

    // Disable animation and set time
    uint16_t tempAnimationDuration = animationDuration;
    animationDuration = 1;
    setTime24Hour(hour, minute, second);

    animationDuration = tempAnimationDuration;
}

void AnalogClock::initializeTime12Hour(uint8_t hour, uint8_t minute, uint8_t second, bool am)
{
    initializeTime24Hour((hour % 12) + (am ? 0 : 12), minute, second);
}

void AnalogClock::updateClock()
{
    // Make sure that animating to 0 will move from left to right
    if (lastHour != 0 && currentHour == 0)
    {
        hourHand.updateZAngle(hourHand.getZAngle() - (2 * PI));
    }
    if (lastMinute != 0 && currentMinute == 0)
    {
        minuteHand.updateZAngle(minuteHand.getZAngle() - (2 * PI));
    }
    if (lastSecond != 0 && currentSecond == 0)
    {
        secondHand.updateZAngle(secondHand.getZAngle() - (2 * PI));
    }

    float newHandAngle;

    // Move hour hand
    if (hourHand.isVisible() && ((currentHour != lastHour) || (hourHandMinuteCorrectionActive && (currentMinute != lastMinute))))
    {
        newHandAngle = convertHandValueToAngle(12, currentHour, hourHandMinuteCorrectionActive ? currentMinute : 0);
        if (animationEnabled() && !hourHand.isTextureMapperAnimationRunning())
        {
            hourHand.setupAnimation(AnimationTextureMapper::Z_ROTATION, newHandAngle, animationDuration, 0, animationEquation);
            hourHand.startAnimation();
        }
        else
        {
            if (animationEnabled())
            {
                hourHand.cancelAnimationTextureMapperAnimation();
            }
            hourHand.updateZAngle(newHandAngle);
        }
    }

    // Move minute hand
    if (minuteHand.isVisible() && ((currentMinute != lastMinute) || (minuteHandSecondCorrectionActive && (currentSecond != lastSecond))))
    {
        newHandAngle = convertHandValueToAngle(60, currentMinute, minuteHandSecondCorrectionActive ? currentSecond : 0);
        if (animationEnabled() && !minuteHand.isTextureMapperAnimationRunning())
        {
            minuteHand.setupAnimation(AnimationTextureMapper::Z_ROTATION, newHandAngle, animationDuration, 0, animationEquation);
            minuteHand.startAnimation();
        }
        else
        {
            if (animationEnabled())
            {
                minuteHand.cancelAnimationTextureMapperAnimation();
            }
            minuteHand.updateZAngle(newHandAngle);
        }
    }

    // Move second hand
    if (secondHand.isVisible() && (currentSecond != lastSecond))
    {
        newHandAngle = convertHandValueToAngle(60, currentSecond);
        if (animationEnabled() && !secondHand.isTextureMapperAnimationRunning())
        {
            secondHand.setupAnimation(AnimationTextureMapper::Z_ROTATION, newHandAngle, animationDuration, 0, animationEquation);
            secondHand.startAnimation();
        }
        else
        {
            if (animationEnabled())
            {
                secondHand.cancelAnimationTextureMapperAnimation();
            }
            secondHand.updateZAngle(newHandAngle);
        }
    }

    lastHour = currentHour;
    lastMinute = currentMinute;
    lastSecond = currentSecond;
}

float AnalogClock::convertHandValueToAngle(uint8_t steps, uint8_t handValue, uint8_t secondHandValue /*= 0*/) const
{
    return ((handValue / (float)steps) + (secondHandValue / (steps * 60.f))) * 2 * PI;
}

void AnalogClock::setHourHandMinuteCorrection(bool active)
{
    hourHandMinuteCorrectionActive = active;
    setTime24Hour(getCurrentHour(), getCurrentMinute(), getCurrentSecond());
}

bool AnalogClock::getHourHandMinuteCorrection() const
{
    return hourHandMinuteCorrectionActive;
}

void AnalogClock::setMinuteHandSecondCorrection(bool active)
{
    minuteHandSecondCorrectionActive = active;
    setTime24Hour(getCurrentHour(), getCurrentMinute(), getCurrentSecond());
}

bool AnalogClock::getMinuteHandSecondCorrection() const
{
    return minuteHandSecondCorrectionActive;
}

bool AnalogClock::animationEnabled() const
{
    return animationDuration > 1;
}

void AnalogClock::setAnimation(uint16_t duration, EasingEquation animationProgressionEquation)
{
    animationDuration = duration;
    animationEquation = animationProgressionEquation;
}
}
