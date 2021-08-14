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
 * @file touchgfx/hal/GPIO.hpp
 *
 * Declares the touchgfx::GPIO class.
 */
#ifndef GPIO_HPP
#define GPIO_HPP

namespace touchgfx
{
/**
 * Interface class for manipulating GPIOs in order to do performance measurements on target. Not
 * used on the PC simulator.
 */
class GPIO
{
public:
    /** Enum for the GPIOs used. */
    typedef enum
    {
        VSYNC_FREQ,  /// Pin is toggled at each VSYNC
        RENDER_TIME, /// Pin is high when frame rendering begins, low when finished
        FRAME_RATE,  /// Pin is toggled when the framebuffers are swapped.
        MCU_ACTIVE   /// Pin is high when the MCU is doing work (i.e. not in idle task).
    } GPIO_ID;

    /** Perform configuration of IO pins. */
    static void init();

    /**
     * Sets a pin high.
     *
     * @param  id the pin to set.
     */
    static void set(GPIO_ID id);

    /**
     * Sets a pin low.
     *
     * @param  id the pin to set.
     */
    static void clear(GPIO_ID id);

    /**
     * Toggles a pin.
     *
     * @param  id the pin to toggle.
     */
    static void toggle(GPIO_ID id);

    /**
     * Gets the state of a pin.
     *
     * @param  id the pin to get.
     *
     * @return true if the pin is high, false otherwise.
     */
    static bool get(GPIO_ID id);
};

} // namespace touchgfx

#endif // GPIO_HPP
