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
 * @file touchgfx/widgets/AbstractButton.hpp
 *
 * Declares the touchgfx::AbstractButton class
 */
#ifndef ABSTRACTBUTTON_HPP
#define ABSTRACTBUTTON_HPP

#include <touchgfx/Callback.hpp>
#include <touchgfx/widgets/Widget.hpp>

namespace touchgfx
{
/**
 * This class defines an abstract interface for button-like elements. A button is a clickable
 * element that has two states: pressed and released. A button also has an action that
 * is executed when the button goes from state pressed to state released.
 */
class AbstractButton : public Widget
{
public:
    /** Sets this Widget touchable so the user can interact with buttons. */
    AbstractButton()
        : Widget(), action(), pressed(false)
    {
        setTouchable(true);
    }

    /**
     * Updates the current state of the button. The state can be either pressed or released,
     * and if the new state is different from the current state, the button is also
     * invalidated to force a redraw.
     *
     * If the button state is changed from ClickEvent::PRESSED to ClickEvent::RELEASED, the
     * associated action (if any) is also executed.
     *
     * @param  event Information about the click.
     */
    virtual void handleClickEvent(const ClickEvent& event);

    /**
     * Associates an action with the button. The action is performed when the AbstractButton
     * is in the pressed state, goes to the released.
     *
     * @param  callback The callback to be executed. The callback will be executed with a
     *                  reference to the AbstractButton.
     *
     * @see GenericCallback, handleClickEvent, ClickEvent
     */
    void setAction(GenericCallback<const AbstractButton&>& callback)
    {
        action = &callback;
    }

    /**
     * Function to determine if the AbstractButton is currently pressed.
     *
     * @return true if button is pressed, false otherwise.
     */
    virtual bool getPressedState() const
    {
        return pressed;
    }

protected:
    GenericCallback<const AbstractButton&>* action; ///< The callback to be executed when this AbstractButton is clicked

    bool pressed; ///< Is the button pressed or released? True if pressed.
};

} // namespace touchgfx

#endif // ABSTRACTBUTTON_HPP
