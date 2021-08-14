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
 * @file touchgfx/transitions/NoTransition.hpp
 *
 * Declares the touchgfx::NoTransition class.
 */
#ifndef NOTRANSITION_HPP
#define NOTRANSITION_HPP

#include <touchgfx/transitions/Transition.hpp>

namespace touchgfx
{
/**
 * The most simple Transition without any visual effects. THe screen transition is done by
 * immediately replace the current Screen with a new Screen.
 *
 * @see Transition
 */
class NoTransition : public Transition
{
public:
    /** Indicates that the transition is done after the first tick. */
    virtual void handleTickEvent()
    {
        done = true;
    }
};

} // namespace touchgfx

#endif // NOTRANSITION_HPP
