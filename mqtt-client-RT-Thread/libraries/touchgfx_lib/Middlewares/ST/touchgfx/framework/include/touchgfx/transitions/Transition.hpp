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
 * @file touchgfx/transitions/Transition.hpp
 *
 * Declares the touchgfx::Transition class.
 */
#ifndef TRANSITION_HPP
#define TRANSITION_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/Application.hpp>

namespace touchgfx
{
class Container;

/**
 * The Transition class is the base class for Transitions. Implementations of Transition defines
 * what happens when transitioning between Screens, which typically involves visual
 * effects. An example of a transition implementation can be seen in example
 * custom_transition_example. The most basic transition is the NoTransition class that
 * does a transition without any visual effects.
 *
 * @see NoTransition, SlideTransition
 */
class Transition
{
public:
    /** Initializes a new instance of the Transition class. */
    Transition()
        : screenContainer(0), done(false)
    {
    }

    /** Finalizes an instance of the Transition class. */
    virtual ~Transition()
    {
    }

    /** Called for every tick when transitioning. */
    virtual void handleTickEvent()
    {
    }

    /**
     * Query if the transition is done transitioning. It is the responsibility of the
     * inheriting class to set the underlying done flag once the transition has been
     * completed.
     *
     * @return True if the transition is done, false otherwise.
     */
    bool isDone() const
    {
        return done;
    }

    /**
     * Tears down the Animation. Called before the destructor is called, when the
     * application changes the transition.
     */
    virtual void tearDown()
    {
    }

    /**
     * Initializes the transition. Called after the constructor is called, when the
     * application changes the transition.
     */
    virtual void init()
    {
    }

    /**
     * Invalidates the screen when starting the Transition. Default is
     * to invalidate the whole screen. Subclasses can do partial
     * invalidation.
     */
    virtual void invalidate()
    {
        Application::getInstance()->draw();
    }

    /**
     * Sets the Screen Container. Is used by Screen to enable the transition to access the
     * Container.
     *
     * @param [in] cont The Container the transition should have access to.
     */
    virtual void setScreenContainer(Container& cont)
    {
        screenContainer = &cont;
    }

protected:
    Container* screenContainer; ///< The screen Container of the Screen transitioning to.
    bool done;                  ///< Flag that indicates when the transition is done. This should be set by implementing classes.
};

} // namespace touchgfx

#endif // TRANSITION_HPP
