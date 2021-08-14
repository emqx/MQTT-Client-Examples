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
 * @file mvp/Presenter.hpp
 *
 * Declares the touchgfx::Presenter class.
 */
#ifndef PRESENTER_HPP
#define PRESENTER_HPP

namespace touchgfx
{
/**
 * The Presenter base class that all application-specific presenters should derive from. Only
 * contains activate and deactivate virtual functions which are called automatically
 * during screen transition.
 */
class Presenter
{
public:
    /**
     * Place initialization code for the Presenter here.
     *
     * The activate function is called automatically when a screen transition causes this
     * Presenter to become active. Place initialization code for the Presenter here.
     */
    virtual void activate()
    {
    }

    /**
     * Place cleanup code for the Presenter here.
     *
     * The deactivate function is called automatically when a screen transition causes this
     * Presenter to become inactive. Place cleanup code for the Presenter here.
     */
    virtual void deactivate()
    {
    }

    /** Finalizes an instance of the Presenter class. */
    virtual ~Presenter()
    {
    }

protected:
    /** Initializes a new instance of the Presenter class. */
    Presenter()
    {
    }
};

} // namespace touchgfx

#endif // PRESENTER_HPP
