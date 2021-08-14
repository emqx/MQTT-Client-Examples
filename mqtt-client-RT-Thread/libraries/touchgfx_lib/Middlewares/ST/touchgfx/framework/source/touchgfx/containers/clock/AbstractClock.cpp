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

#include <touchgfx/containers/clock/AbstractClock.hpp>

namespace touchgfx
{
AbstractClock::AbstractClock() :
    Container(),
    currentHour(0),
    currentMinute(0),
    currentSecond(0)
{
}

void AbstractClock::setTime24Hour(uint8_t hour, uint8_t minute, uint8_t second)
{
    currentHour = hour % 24;
    currentMinute = minute % 60;
    currentSecond = second % 60;

    updateClock();
}

void AbstractClock::setTime12Hour(uint8_t hour, uint8_t minute, uint8_t second, bool am)
{
    setTime24Hour((hour % 12) + (am ? 0 : 12), minute, second);
}

uint8_t AbstractClock::getCurrentHour() const
{
    return currentHour;
}

uint8_t AbstractClock::getCurrentMinute() const
{
    return currentMinute;
}

uint8_t AbstractClock::getCurrentSecond() const
{
    return currentSecond;
}
}
