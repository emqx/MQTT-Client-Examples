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
 * @file touchgfx/widgets/TouchArea.hpp
 *
 * Declares the touchgfx::TouchArea class.
 */
#ifndef TOUCHAREA_HPP
#define TOUCHAREA_HPP

#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/AbstractButton.hpp>

namespace touchgfx
{
/**
 * Invisible widget used to capture touch events. The TouchArea consumes drag events without the
 * widget it self moving.
 */
class TouchArea : public AbstractButton
{
public:
    TouchArea()
        : AbstractButton(), pressedAction(0)
    {
    }

    virtual void draw(const Rect& invalidatedArea) const
    {
    }

    virtual void handleDragEvent(const DragEvent& evt)
    {
    }

    virtual void handleClickEvent(const ClickEvent& event);

    virtual Rect getSolidRect() const
    {
        return Rect(0, 0, 0, 0);
    }

    /**
     * Associates an action to be performed when the TouchArea is pressed.
     *
     * @param  callback The callback is given a reference to this touch area.
     */
    void setPressedAction(GenericCallback<const AbstractButton&>& callback)
    {
        pressedAction = &callback;
    }

protected:
    GenericCallback<const AbstractButton&>* pressedAction; ///< The action to perform when the TouchArea is clicked
};

} // namespace touchgfx

#endif // TOUCHAREA_HPP
