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
 * @file touchgfx/hal/LED.hpp
 *
 * Declares the touchgfx::LED class.
 */
#ifndef LED_HPP
#define LED_HPP

namespace touchgfx
{
/** A class for controlling a LED. */
class LED
{
public:
    ///@cond
    /** Perform configuration of IO pins. */
    TOUCHGFX_DEPRECATED(
        "Class LED is scheduled for removal",
        static void init());

    /**
     * Turn on a LED.
     *
     * @param  nr of the LED to switch on.
     */
    TOUCHGFX_DEPRECATED(
        "Class LED is scheduled for removal",
        static void on(uint8_t nr));

    /**
     * Turn off a LED.
     *
     * @param  nr of the LED to switch off.
     */
    TOUCHGFX_DEPRECATED(
        "Class LED is scheduled for removal",
        static void off(uint8_t nr));

    /**
     * Toggles a LED.
     *
     * @param  nr of the LED to toggle.
     */
    TOUCHGFX_DEPRECATED(
        "Class LED is scheduled for removal",
        static void toggle(uint8_t nr));

    /**
     * Get state of a LED.
     *
     * @param  nr of the LED to toggle.
     *
     * @return the state of the LED.
     */
    TOUCHGFX_DEPRECATED(
        "Class LED is scheduled for removal",
        static bool get(uint8_t nr));
    ///@endcond
};

} // namespace touchgfx

#endif // LED_HPP
