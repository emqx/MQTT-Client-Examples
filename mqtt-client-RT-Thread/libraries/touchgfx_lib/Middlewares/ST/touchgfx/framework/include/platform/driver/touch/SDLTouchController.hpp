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
 * @file platform/driver/touch/SDLTouchController.hpp
 *
 * Declares the touchgfx::SDLTouchController class.
 */
#ifndef SDLTOUCHCONTROLLER_HPP
#define SDLTOUCHCONTROLLER_HPP

#include <platform/driver/touch/TouchController.hpp>

namespace touchgfx
{
/**
 * TouchController for the simulator.
 *
 * @see TouchController
 *
 * @deprecated Use SDL2TouchController
 */
class SDLTouchController : public TouchController
{
public:
    virtual void init();

    virtual bool sampleTouch(int32_t& x, int32_t& y);
};

} // namespace touchgfx

#endif // SDLTOUCHCONTROLLER_HPP
