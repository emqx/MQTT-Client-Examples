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
 * @file touchgfx/containers/buttons/AbstractButtonContainer.hpp
 *
 * Declares the touchgfx::AbstractButtonContainer class.
 */
#ifndef ABSTRACTBUTTONCONTAINER_HPP
#define ABSTRACTBUTTONCONTAINER_HPP

#include <touchgfx/containers/Container.hpp>

namespace touchgfx
{
/**
 * An abstract button container. The AbstractButtonContainer defines pressed/not pressed state,
 * the alpha value, and the action Callback of a button. AbstractButtonContainer is used
 * as superclass for classes defining a specific button behavior.
 *
 * @see ClickButtonTrigger, RepeatButtonTrigger, ToggleButtonTrigger, TouchButtonTrigger
 */
class AbstractButtonContainer : public Container
{
public:
    AbstractButtonContainer()
        : pressed(false), alpha(255), action(0)
    {
        setTouchable(true);
    }

    /**
     * Sets the pressed state to the given state. A subclass of AbstractButtonContainer
     * should implement handlePressedUpdate() to handle the new pressed state.
     *
     * @param  isPressed True if is pressed, false if not.
     *
     * @see getPressed, handlePressedUpdated
     */
    void setPressed(bool isPressed)
    {
        pressed = isPressed;
        handlePressedUpdated();
    }

    /**
     * Gets the pressed state.
     *
     * @return True if it succeeds, false if it fails.
     *
     * @see setPressed
     */
    bool getPressed()
    {
        return pressed;
    }

    /** @copydoc Image::setAlpha() */
    void setAlpha(uint8_t newAlpha)
    {
        alpha = newAlpha;
        handleAlphaUpdated();
    }

    /** @copydoc Image::getAlpha() */
    uint8_t getAlpha() const
    {
        return alpha;
    }

    /**
     * Sets an action callback to be executed by the subclass of AbstractContainerButton.
     *
     * @param  callback The callback.
     */
    void setAction(GenericCallback<const AbstractButtonContainer&>& callback)
    {
        action = &callback;
    }

protected:
    bool pressed;  ///< True if pressed
    uint8_t alpha; ///< The current alpha value. 255 denotes solid, 0 denotes completely invisible.

    GenericCallback<const AbstractButtonContainer&>* action; ///< The action to be executed

    /** Handles what should happen when the pressed state is updated. */
    virtual void handlePressedUpdated()
    {
    }

    /** Handles what should happen when the alpha is updated. */
    virtual void handleAlphaUpdated()
    {
    }
};

} // namespace touchgfx

#endif // ABSTRACTBUTTONCONTAINER_HPP
