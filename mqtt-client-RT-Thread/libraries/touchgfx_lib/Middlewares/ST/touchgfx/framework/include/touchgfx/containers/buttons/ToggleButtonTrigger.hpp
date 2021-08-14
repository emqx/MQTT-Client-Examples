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
 * @file touchgfx/containers/buttons/ToggleButtonTrigger.hpp
 *
 * Declares the touchgfx::ToggleButtonTrigger class.
 */
#ifndef TOGGLEBUTTONTRIGGER_HPP
#define TOGGLEBUTTONTRIGGER_HPP

#include <touchgfx/containers/buttons/AbstractButtonContainer.hpp>

namespace touchgfx
{
/**
 * A toggle button trigger. This trigger will create a button that reacts on clicks. This means
 * it will call the set action when it gets a touch released event, just like a
 * ClickButtonTrigger. The difference being that a ToggleButtonTrigger will stay in
 * pressed state until it is clicked again.
 *
 * The ToggleButtonTrigger can be combined with one or more of the ButtonStyle classes
 * to create a fully functional button.
 */
class ToggleButtonTrigger : public AbstractButtonContainer
{
public:
    ToggleButtonTrigger()
        : AbstractButtonContainer(), toggleCanceled(false)
    {
    }

    /**
     * Allows the button to be forced into either the pressed state, or the normal state. In
     * the pressed state, the button will always be shown as pressed down (and shown as
     * released when the user presses it). In the normal state, the button will be show as
     * released or pressed depending on its actual state.
     *
     * @param  activeState If true, swap the images for released and pressed. If false display
     *                     the button normally.
     */
    void forceState(bool activeState)
    {
        AbstractButtonContainer::setPressed(activeState);
    }

    /**
     * Sets toggle canceled.
     *
     * @param  isToggleCanceled True if is toggle canceled, false if not.
     */
    void setToggleCanceled(bool isToggleCanceled)
    {
        toggleCanceled = isToggleCanceled;
    }

    /**
     * Gets toggle canceled.
     *
     * @return True if it succeeds, false if it fails.
     */
    bool getToggleCanceled()
    {
        return toggleCanceled;
    }

    virtual void handleClickEvent(const ClickEvent& event)
    {
        bool wasPressed = getPressed();
        bool newPressedValue = !getPressed();
        bool toggleCanceled = getToggleCanceled();
        setToggleCanceled(event.getType() == ClickEvent::CANCEL);

        if (((newPressedValue && !wasPressed) || (!newPressedValue && wasPressed)) && (event.getType() != ClickEvent::RELEASED) && !toggleCanceled)
        {
            setPressed(newPressedValue);
            invalidate();
        }

        if (!toggleCanceled && (event.getType() == ClickEvent::RELEASED) && action)
        {
            if (action->isValid())
            {
                action->execute(*this);
            }
        }
    }

protected:
    bool toggleCanceled; ///< True if toggle canceled
};

} // namespace touchgfx

#endif // TOGGLEBUTTONTRIGGER_HPP
