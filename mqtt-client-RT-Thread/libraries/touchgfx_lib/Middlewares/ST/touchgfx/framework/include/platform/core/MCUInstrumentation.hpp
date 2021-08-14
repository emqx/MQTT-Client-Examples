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
 * @file platform/core/MCUInstrumentation.hpp
 *
 * Declares the touchgfx::MCUInstrumentation interface class.
 */
#ifndef MCUINSTRUMENTATION_HPP
#define MCUINSTRUMENTATION_HPP

#include <stdint.h>

namespace touchgfx
{
/** Interface for instrumenting processors to measure MCU load via measured CPU cycles. */
class MCUInstrumentation
{
public:
    /** Initializes a new instance of the MCUInstrumentation class. */
    MCUInstrumentation()
        : cc_consumed(0),
          cc_in(0)
    {
    }

    /** Initialize. */
    virtual void init() = 0;

    /** Finalizes an instance of the MCUInstrumentation class. */
    virtual ~MCUInstrumentation()
    {
    }

    /**
     * Gets elapsed microseconds based on clock frequency.
     *
     * @param  start          Start time.
     * @param  now            Current time.
     * @param  clockfrequency Clock frequency of the system expressed in MHz.
     *
     * @return Elapsed microseconds start and now.
     */
    virtual unsigned int getElapsedUS(unsigned int start, unsigned int now, unsigned int clockfrequency) = 0;

    /**
     * Gets CPU cycles from register.
     *
     * @return CPU cycles.
     */
    virtual unsigned int getCPUCycles(void) = 0;

    /**
     * Sets MCU activity high.
     *
     * @param  active if True, inactive otherwise.
     */
    virtual void setMCUActive(bool active)
    {
        if (active) //idle task sched out
        {
            uint32_t cc_temp = getCPUCycles() - cc_in;
            cc_consumed += cc_temp;
        }
        else //idle task sched in
        {
            cc_in = getCPUCycles();
        }
    }

    /**
     * Gets number of consumed clock cycles.
     *
     * @return clock cycles.
     */
    virtual uint32_t getCCConsumed()
    {
        return cc_consumed;
    }

    /**
     * Sets number of consumed clock cycles.
     *
     * @param  val number of clock cycles.
     */
    virtual void setCCConsumed(uint32_t val)
    {
        cc_consumed = val;
    }

protected:
    volatile uint32_t cc_consumed; ///< Amount of consumed CPU cycles.
    volatile uint32_t cc_in;       ///< Current CPU cycles.
};

} // namespace touchgfx

#endif // MCUINSTRUMENTATION_HPP
