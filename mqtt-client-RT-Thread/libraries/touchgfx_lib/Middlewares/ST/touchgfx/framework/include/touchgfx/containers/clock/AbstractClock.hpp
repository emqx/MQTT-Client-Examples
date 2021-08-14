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
 * @file touchgfx/containers/clock/AbstractClock.hpp
 *
 * Declares the touchgfx::AbstractClock class.
 */
#ifndef ABSTRACTCLOCK_HPP
#define ABSTRACTCLOCK_HPP

#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/TextureMapper.hpp>

namespace touchgfx
{
/**
 * Superclass of clock widgets. Allows the hour, minute and second of the clock to be set and
 * read.
 *
 * @see AnalogClock, DigitalClock
 */
class AbstractClock : public Container
{
public:
    AbstractClock();

    /**
     * Sets the time with input format as 24H. Note that this does not affect any selected
     * presentation formats.
     *
     * @param  hour   The hours, value should be between 0 and 23.
     * @param  minute The minutes, value should be between 0 and 59.
     * @param  second The seconds, value should be between 0 and 59.
     *
     * @note all values passed are saved modulo the values limit. For example minutes=62 is
     *       treated as minutes=2.
     */
    virtual void setTime24Hour(uint8_t hour, uint8_t minute, uint8_t second);

    /**
     * Sets the time with input format as 12H. Note that this does not affect any selected
     * presentation formats.
     *
     * @param  hour   The hours, value should be between 1 and 12.
     * @param  minute The minutes, value should be between 0 and 59.
     * @param  second The seconds, value should be between 0 and 59.
     * @param  am     AM/PM setting. True = AM, false = PM.
     *
     * @note all values passed are saved modulo the values limit. For example minutes=62 is
     *       treated as minutes=2.
     */
    virtual void setTime12Hour(uint8_t hour, uint8_t minute, uint8_t second, bool am);

    /**
     * Gets the current hour.
     *
     * @return The current hour in range 0-23.
     */
    uint8_t getCurrentHour() const;

    /**
     * Gets the current minute.
     *
     * @return The current minute in range 0-59.
     */
    uint8_t getCurrentMinute() const;

    /**
     * Gets the current second.
     *
     * @return The current second in range 0-59.
     */
    uint8_t getCurrentSecond() const;

protected:
    uint8_t currentHour;   ///< Local copy of the current hour
    uint8_t currentMinute; ///< Local copy of the current minute
    uint8_t currentSecond; ///< Local copy of the current second

    /** Update the visual representation of the clock on the display. */
    virtual void updateClock() = 0;
};

} // namespace touchgfx

#endif // ABSTRACTCLOCK_HPP
