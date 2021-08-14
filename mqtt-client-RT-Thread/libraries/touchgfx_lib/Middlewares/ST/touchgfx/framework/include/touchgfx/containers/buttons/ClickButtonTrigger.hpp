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
 * @file touchgfx/containers/buttons/ClickButtonTrigger.hpp
 *
 * Declares the touchgfx::ClickButtonTrigger class.
 */
#ifndef CLICKBUTTONTRIGGER_HPP
#define CLICKBUTTONTRIGGER_HPP

#include <touchgfx/containers/buttons/AbstractButtonContainer.hpp>

namespace touchgfx
{
/**
 * A click button trigger. This trigger will create a button that reacts on clicks. This means
 * it will call the set action when it gets a touch released event. The
 * ClickButtonTrigger can be combined with one or more of the ButtonStyle classes to
 * create a fully functional button.
 *
 * @see TouchButtonTrigger
 */
class ClickButtonTrigger : public AbstractButtonContainer
{
public:
    /**
     * Handles a ClickAvent. The action callback is called when the ClickButtonTrigger
     * receives a ClickEvent::RELEASED event in PRESSED state. Function setPressed() will
     * be called with the new button state.
     *
     * @param  event The click event.
     *
     * @see setAction, setPressed, getPressed
     */
    virtual void handleClickEvent(const ClickEvent& event)
    {
        bool wasPressed = getPressed();
        bool newPressedValue = (event.getType() == ClickEvent::PRESSED);
        if ((newPressedValue && !wasPressed) || (!newPressedValue && wasPressed))
        {
            setPressed(newPressedValue);
            invalidate();
        }
        if (wasPressed && (event.getType() == ClickEvent::RELEASED) && action)
        {
            if (action->isValid())
            {
                action->execute(*this);
            }
        }
    }
};

} // namespace touchgfx

#endif // CLICKBUTTONTRIGGER_HPP
