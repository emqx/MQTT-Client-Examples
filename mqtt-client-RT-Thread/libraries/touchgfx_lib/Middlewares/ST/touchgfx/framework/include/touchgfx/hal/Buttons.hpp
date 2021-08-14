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
 * @file touchgfx/hal/Buttons.hpp
 *
 * Declares the touchgfx::Buttons class.
 */
#ifndef BUTTONS_HPP
#define BUTTONS_HPP

namespace touchgfx
{
/** A class for accessing a physical button. */
class Buttons
{
public:
    /** Perform configuration of IO pins. */
    static void init();

    /**
     * Sample button states.
     *
     * @return the sampled state of the buttons.
     */
    static unsigned int sample();
};

} // namespace touchgfx

#endif // BUTTONS_HPP
