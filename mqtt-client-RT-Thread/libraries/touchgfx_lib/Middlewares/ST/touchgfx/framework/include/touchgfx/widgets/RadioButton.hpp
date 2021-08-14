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
 * @file touchgfx/widgets/RadioButton.hpp
 *
 * Declares the touchgfx::RadioButton class.
 */
#ifndef RADIOBUTTON_HPP
#define RADIOBUTTON_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/widgets/AbstractButton.hpp>

namespace touchgfx
{
/**
 * Radio button with two states. A RadioButton is a button that changes appearance (state) when
 * it has been pushed. Pushing the RadioButton again will return the to original state.
 *
 * To make managing radio buttons much easier, they can be added to a RadioButtonGroup
 * which then automates deselecting radio buttons when a new radio button is pressed.
 *
 * @see RadioButtonGroup
 */
class RadioButton : public AbstractButton
{
public:
    RadioButton()
        : AbstractButton(), bitmapUnselected(), bitmapUnselectedPressed(), bitmapSelected(), bitmapSelectedPressed(), alpha(255), selected(false), deselectionEnabled(false), deselectedAction(0)
    {
    }

    virtual void draw(const Rect& invalidatedArea) const;

    virtual void handleClickEvent(const ClickEvent& event);

    /**
     * Sets the four bitmaps used by this button. The first two bitmaps must show the
     * unselected Button when it is released and pressed. The last two bitmaps must show the
     * selected Button when it is released and pressed.
     *
     * @param  bmpUnselected        Bitmap to use when button is unselected and released.
     * @param  bmpUnselectedPressed Bitmap to use when button is unselected and pressed.
     * @param  bmpSelected          Bitmap to use when button is selected and released.
     * @param  bmpSelectedPressed   Bitmap to use when button is selected and pressed.
     *
     * @note It is not uncommon to have the same bitmap for released (normal) and pressed state.
     */
    virtual void setBitmaps(const Bitmap& bmpUnselected, const Bitmap& bmpUnselectedPressed, const Bitmap& bmpSelected, const Bitmap& bmpSelectedPressed);

    /**
     * Associates an action to be performed when the RadioButton is deselected.
     *
     * @param  callback The callback to be executed. The callback will be given a reference
     *                  to the AbstractButton.
     *
     * @note The action performed when the RadioButton is selected, is set using
     *       setAction().
     */
    void setDeselectedAction(GenericCallback<const AbstractButton&>& callback)
    {
        deselectedAction = &callback;
    }

    virtual Rect getSolidRect() const;

    /**
     * Sets the alpha channel for the RadioButton, i.e. all the images used. The default
     * alpha value on a RadioButton is 255.
     *
     * @param  alpha The alpha value ranging from 255=solid to 0=invisible.
     *
     * @see getAlpha
     */
    void setAlpha(uint8_t alpha)
    {
        this->alpha = alpha;
    }

    /**
     * Gets the current alpha value, as previously set using setAlpha. The default alpha
     * value (if the alpha value has not been changed using setAlpha) is 255=solid.
     *
     * @return The current alpha value ranging from 255=solid to 0=invisible.
     *
     * @see setAlpha
     */
    uint8_t getAlpha() const
    {
        return alpha;
    }

    /**
     * Sets whether or not it is possible to deselect the RadioButton by clicking it. By
     * default it is not possible to deselect a RadioButton. The meaning of this is most
     * clear when the RadioButton is used in a RadioButtonGroup where exactly one
     * RadioButton should always be selected. Pressing the currently selected RadioButton
     * should not deselect it, but rather select it again. This makes the button "sticky",
     * i.e. a button can only be deselected by selecting another RadioButton in the same
     * RadioButtonGroup.
     *
     * @param  state true if it should be possible to deselect by click. Default is false.
     *
     * @see getDeselectionEnabled
     */
    void setDeselectionEnabled(bool state)
    {
        deselectionEnabled = state;
    }

    /**
     * Gets the current deselectionEnabled state.
     *
     * @return The current deselectionEnabled state.
     *
     * @see setDeselectionEnabled
     */
    bool getDeselectionEnabled() const
    {
        return deselectionEnabled;
    }

    /**
     * Sets the radio buttons selected state. Note that the associated action is also
     * performed.
     *
     * @param  newSelected The new selected state.
     *
     * @see setAction, setDeselectedAction, RadioButtonGroup
     *
     * @note If the RadioButton is part of a RadioButtonGroup, setting the selected state of
     *       individual RadioButtons is not recommended.
     */
    void setSelected(bool newSelected);

    /**
     * Gets the current selected state.
     *
     * @return The current selected state.
     */
    bool getSelected() const
    {
        return selected;
    }

    /**
     * Gets currently displayed bitmap. This depends on whether the RadioButton is currently
     * selected or not and whether it is being pressed or not, i.e. it depends on the radio
     * button's pressed and selected state.
     *
     * @return The bitmap currently displayed.
     */
    Bitmap getCurrentlyDisplayedBitmap() const
    {
        return (selected ? (AbstractButton::pressed ? bitmapSelectedPressed : bitmapSelected) : (AbstractButton::pressed ? bitmapUnselectedPressed : bitmapUnselected));
    }

protected:
    Bitmap bitmapUnselected;        ///< The image to display when radio button unselected and released.
    Bitmap bitmapUnselectedPressed; ///< The image to display when radio button unselected and pressed.
    Bitmap bitmapSelected;          ///< The image to display when radio button selected and released.
    Bitmap bitmapSelectedPressed;   ///< The image to display when radio button selected and pressed.
    uint8_t alpha;                  ///< The current alpha value. 255=solid, 0=invisible.
    bool selected;                  ///< The current selected state.
    bool deselectionEnabled;        ///< Is it possible to deselect by pressing a selected RadioButton.

    GenericCallback<const AbstractButton&>* deselectedAction; ///< The callback to be executed when this AbstractButton is deselected.
};

} // namespace touchgfx

#endif // RADIOBUTTON_HPP
