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

#include <touchgfx/containers/clock/DigitalClock.hpp>

namespace touchgfx
{
DigitalClock::DigitalClock() :
    AbstractClock(),
    displayMode(DISPLAY_24_HOUR),
    useLeadingZeroForHourIndicator(false)
{
    buffer[0] = '\0';
    text.setXY(0, 0);
    text.setWildcard(buffer);
    Container::add(text);
}

void DigitalClock::setWidth(int16_t width)
{
    Container::setWidth(width);
    text.setWidth(width);
}

void DigitalClock::setHeight(int16_t height)
{
    Container::setHeight(height);
    text.setHeight(height);
}

void DigitalClock::setBaselineY(int16_t baselineY)
{
    if (text.getTypedText().hasValidId())
    {
        moveTo(getX(), baselineY - text.getTypedText().getFont()->getFontHeight());
    }
}

void DigitalClock::displayLeadingZeroForHourIndicator(bool displayLeadingZero)
{
    useLeadingZeroForHourIndicator = displayLeadingZero;
}

void DigitalClock::setAlpha(uint8_t newAlpha)
{
    text.setAlpha(newAlpha);
}

uint8_t DigitalClock::getAlpha() const
{
    return text.getAlpha();
}

void DigitalClock::setTypedText(TypedText typedText)
{
    text.setTypedText(typedText);
    text.invalidate();
}

void DigitalClock::setColor(colortype color)
{
    text.setColor(color);
    text.invalidate();
}

void DigitalClock::updateClock()
{
    if (displayMode == DISPLAY_12_HOUR_NO_SECONDS)
    {
        const char* format = useLeadingZeroForHourIndicator ? "%02d:%02d %cM" : "%d:%02d %cM";
        Unicode::snprintf(buffer, BUFFER_SIZE, format, ((currentHour + 11) % 12) + 1, currentMinute, currentHour < 12 ? 'A' : 'P');
    }
    else if (displayMode == DISPLAY_24_HOUR_NO_SECONDS)
    {
        const char* format = useLeadingZeroForHourIndicator ? "%02d:%02d" : "%d:%02d";
        Unicode::snprintf(buffer, BUFFER_SIZE, format, currentHour, currentMinute);
    }
    else if (displayMode == DISPLAY_12_HOUR)
    {
        const char* format = useLeadingZeroForHourIndicator ? "%02d:%02d:%02d %cM" : "%d:%02d:%02d %cM";
        Unicode::snprintf(buffer, BUFFER_SIZE, format, ((currentHour + 11) % 12) + 1, currentMinute, currentSecond, currentHour < 12 ? 'A' : 'P');
    }
    else if (displayMode == DISPLAY_24_HOUR)
    {
        const char* format = useLeadingZeroForHourIndicator ? "%02d:%02d:%02d" : "%d:%02d:%02d";
        Unicode::snprintf(buffer, BUFFER_SIZE, format, currentHour, currentMinute, currentSecond);
    }
    text.invalidate();
}
} // namespace touchgfx
