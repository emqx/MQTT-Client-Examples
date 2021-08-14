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
 * @file platform/driver/touch/TouchController.hpp
 *
 * Declares the touchgfx::TouchController interface class.
 */
#ifndef TOUCHCONTROLLER_HPP
#define TOUCHCONTROLLER_HPP

#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/** Basic Touch Controller interface. */
class TouchController
{
public:
    /** Finalizes an instance of the TouchController class. */
    virtual ~TouchController()
    {
    }

    /** Initializes touch controller. */
    virtual void init() = 0;

    /**
     * Checks whether the touch screen is being touched, and if so, what coordinates.
     *
     * @param [out] x The x position of the touch.
     * @param [out] y The y position of the touch.
     *
     * @return True if a touch has been detected, otherwise false.
     */
    virtual bool sampleTouch(int32_t& x, int32_t& y) = 0;
};

} // namespace touchgfx

#endif // TOUCHCONTROLLER_HPP
