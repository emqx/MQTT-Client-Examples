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
 * @file touchgfx/mixins/ClickListener.hpp
 *
 * Declares the touchgfx::ClickListener class.
 */
#ifndef CLICKLISTENER_HPP
#define CLICKLISTENER_HPP

#include <touchgfx/Callback.hpp>

namespace touchgfx
{
/**
 * Mix-in class that extends a class with a click action event that is called when the class
 * receives a click event.
 *
 * @tparam T specifies the type to extend with the ClickListener behavior.
 */
template <class T>
class ClickListener : public T
{
public:
    /** Initializes a new instance of the ClickListener class. Make the object touchable. */
    ClickListener()
        : T(), clickAction(0)
    {
        T::setTouchable(true);
    }

    /**
     * Ensures that the clickEvent is propagated to the super class T and to the clickAction
     * listener.
     *
     * @param  event Information about the click.
     */
    virtual void handleClickEvent(const ClickEvent& event)
    {
        T::handleClickEvent(event);
        if (clickAction && clickAction->isValid())
        {
            clickAction->execute(*this, event);
        }
    }

    /**
     * Associates an action to be performed when the class T is clicked.
     *
     * @param  callback The callback to be executed. The callback will be given a reference
     *                  to T.
     */
    void setClickAction(GenericCallback<const T&, const ClickEvent&>& callback)
    {
        clickAction = &callback;
    }

protected:
    GenericCallback<const T&, const ClickEvent&>* clickAction; ///< The callback to be executed when T is clicked
};
} //namespace touchgfx

#endif // CLICKLISTENER_HPP
