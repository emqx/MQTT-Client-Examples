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
 * @file touchgfx/widgets/ToggleButton.hpp
 *
 * Declares the touchgfx::ToggleButton class.
 */
#ifndef TOGGLEBUTTON_HPP
#define TOGGLEBUTTON_HPP

#include <touchgfx/widgets/Button.hpp>

namespace touchgfx
{
/**
 * A ToggleButton is a Button specialization that swaps the two bitmaps when clicked, such that
 * the previous "pressed" bitmap, now becomes the one displayed when button is not
 * pressed. This can by used to give the effect of a button that can be pressed in and
 * when it is subsequently pressed, it will pop back out.
 */
class ToggleButton : public Button
{
public:

    virtual void setBitmaps(const Bitmap& bmpReleased, const Bitmap& bmpPressed)
    {
        originalPressed = bmpPressed;
        Button::setBitmaps(bmpReleased, bmpPressed);
    }

    /**
     * Allows the ToggleButton to be forced into either the pressed state, or the normal
     * state. In the pressed state, the Button will always be shown as pressed down (and
     * shown as released when the user presses it). In the normal state, the Button will be
     * show as released or pressed depending on its actual state.
     *
     * @param  activeState If true, swap the images for released and pressed. If false display
     *                     the Button normally.
     */
    void forceState(bool activeState);

    /**
     * Gets the state of the ToggleButton as set with forceState.
     *
     * @return True if the button has been toggled, i.e. the pressed state is shown when the
     *         button is not pressed.
     */
    bool getState() const
    {
        return up.getId() == originalPressed.getId();
    }

    virtual void handleClickEvent(const ClickEvent& event);

protected:
    Bitmap originalPressed; ///< Contains the bitmap that was originally being displayed when button is pressed.
};

} // namespace touchgfx

#endif // TOGGLEBUTTON_HPP
