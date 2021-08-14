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
 * @file platform/driver/button/ButtonController.hpp
 *
 * Declares the touchgfx::ButtonController interface class.
 */
#ifndef BUTTONCONTROLLER_HPP
#define BUTTONCONTROLLER_HPP

#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/** Interface for sampling external key events. */
class ButtonController
{
public:
    /** Finalizes an instance of the ButtonController class. */
    virtual ~ButtonController()
    {
    }

    /** Initializes button controller. */
    virtual void init() = 0;

    /**
     * Sample external key events.
     *
     * @param [out] key Output parameter that will be set to the key value if a keypress was
     *                     detected.
     *
     * @return True if a keypress was detected and the "key" parameter is set to a value.
     */
    virtual bool sample(uint8_t& key) = 0;

    /** Resets button controller. Does nothing in the default implementation. */
    virtual void reset()
    {
    }
};

} // namespace touchgfx

#endif // BUTTONCONTROLLER_HPP
