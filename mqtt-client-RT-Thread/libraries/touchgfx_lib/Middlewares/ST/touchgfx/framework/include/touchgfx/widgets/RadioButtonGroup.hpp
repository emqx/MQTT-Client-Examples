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
 * @file touchgfx/widgets/RadioButtonGroup.hpp
 *
 * Declares the touchgfx::RadioButtonGroup class.
 */
#ifndef RADIOBUTTONGROUP_HPP
#define RADIOBUTTONGROUP_HPP

#include <cassert>
#include <touchgfx/Callback.hpp>
#include <touchgfx/widgets/RadioButton.hpp>

namespace touchgfx
{
/**
 * Class for handling a collection of RadioButton objects. The RadioButtonGroup handles the
 * automatic deselection of other radio buttons when a new RadioButton is selected. A
 * callback is executed when a new selection occurs reporting the newly selected
 * RadioButton.
 *
 * @tparam CAPACITY The number of RadioButtons to store in the RadioButtonGroup.
 *
 * @see RadioButton
 */
template <uint16_t CAPACITY>
class RadioButtonGroup
{
public:
    /** Initializes a new instance of the RadioButtonGroup class. */
    RadioButtonGroup()
        : size(0),
          radioButtonClicked(this, &RadioButtonGroup::radioButtonClickedHandler),
          radioButtonUnselected(this, &RadioButtonGroup::radioButtonDeselectedHandler),
          radioButtonSelectedCallback(0),
          radioButtonDeselectedCallback(0)
    {
    }

    /** Finalizes an instance of the RadioButtonGroup class. */
    virtual ~RadioButtonGroup()
    {
    }

    /**
     * Add the RadioButton to the RadioButtonGroup. Adding more radio buttons than the
     * \a CAPACITY of the RadioButtonGroup raises an assert.
     *
     * @param [in] radioButton The RadioButton to add.
     */
    virtual void add(RadioButton& radioButton)
    {
        assert(size < CAPACITY && "RadioButtonGroup capacity exceeded!");
        radioButton.setAction(radioButtonClicked);
        radioButton.setDeselectedAction(radioButtonUnselected);
        radioButtons[size++] = &radioButton;
    }

    /**
     * Gets the RadioButton at the specified index.
     *
     * @param  index the index of the RadioButton to return.
     *
     * @return the RadioButton at the specified index. Returns 0 if the index is illegal.
     */
    virtual RadioButton* getRadioButton(uint16_t index) const
    {
        return (size > index) ? radioButtons[index] : 0;
    }

    /**
     * Gets the index of the currently selected RadioButton.
     *
     * @return the index of the selected RadioButton. Returns -1 if no RadioButton is
     *         selected.
     */
    virtual int32_t getSelectedRadioButtonIndex() const
    {
        for (uint16_t i = 0; i < size; i++)
        {
            if (radioButtons[i]->getSelected())
            {
                return i;
            }
        }
        return -1;
    }

    /**
     * Gets the currently selected RadioButton
     *
     * @return a pointer to the selected RadioButton. Returns 0 if no RadioButton is
     *         selected.
     */
    virtual RadioButton* getSelectedRadioButton() const
    {
        int32_t index = getSelectedRadioButtonIndex();
        return (index < 0) ? 0 : getRadioButton(index);
    }

    /**
     * Sets the specified RadioButton to be selected.
     *
     * Sets the specified RadioButton to be selected and all other radio buttons to be
     * deselected. Do not call this function before all RadioButton objects have been added
     * to the RadioButtonGroup. Will call the radioButtonSelected callback.
     *
     * @param [in] radioButton the RadioButton to be selected.
     */
    virtual void setSelected(RadioButton& radioButton)
    {
        radioButton.setSelected(true);
        radioButtonClickedHandler(radioButton);
    }

    /**
     * Sets whether or not it is possible to deselect RadioButtons by clicking them when
     * they are selected. If deselection is enabled, it will be possible to select a
     * RadioButton (and as a result deselect all other radio buttons) and the push the same
     * RadioButton again to deselect it. The result is that no RadioButton is selected.
     *
     * @param  deselectionEnabled true if it should be possible to deselect by click.
     *
     * @see getDeselectionEnabled
     */
    virtual void setDeselectionEnabled(bool deselectionEnabled)
    {
        for (uint16_t i = 0; i < size; i++)
        {
            radioButtons[i]->setDeselectionEnabled(deselectionEnabled);
        }
    }

    /**
     * Gets the current deselectionEnabled state.
     *
     * @return The current deselectionEnabled state.
     *
     * @see setDeselectionEnabled
     */
    virtual bool getDeselectionEnabled() const
    {
        return (size > 0) ? radioButtons[0]->getDeselectionEnabled() : false;
    }

    /**
     * Associates an action to be performed when a radio button belonging to this group is
     * selected.
     *
     * @param  callback The callback to be executed. The callback will be given a reference
     *                  to the RadioButton that was selected.
     *
     * @see GenericCallback
     */
    void setRadioButtonSelectedHandler(GenericCallback<const AbstractButton&>& callback)
    {
        radioButtonSelectedCallback = &callback;
    }

    /**
     * Associates an action to be performed when a radio button belonging to this group
     * transition from selected to unselected.
     *
     * @param  callback The callback to be executed. The callback will be given a reference
     *                  to the RadioButton that was selected.
     *
     * @see GenericCallback
     */
    void setRadioButtonDeselectedHandler(GenericCallback<const AbstractButton&>& callback)
    {
        radioButtonDeselectedCallback = &callback;
    }

protected:
    RadioButton* radioButtons[CAPACITY]; ///< The list of added RadioButtons.
    uint16_t size;                       ///< The current number of added RadioButtons.

    Callback<RadioButtonGroup, const AbstractButton&> radioButtonClicked;    ///< Callback that is attached to the RadioButtons.
    Callback<RadioButtonGroup, const AbstractButton&> radioButtonUnselected; ///< Callback that is attached to the RadioButtons.

    GenericCallback<const AbstractButton&>* radioButtonSelectedCallback;   ///< The callback to be executed when a radio button belonging to this group is selected.
    GenericCallback<const AbstractButton&>* radioButtonDeselectedCallback; ///< The callback to be executed when a radio button belonging to this group is deselected.

    /**
     * Handles the event that a RadioButton has been selected. deselects all other
     * RadioButtons.
     *
     * @param  radioButton the RadioButton that has been selected.
     */
    virtual void radioButtonClickedHandler(const AbstractButton& radioButton)
    {
        // Deselect other radio buttons
        for (uint16_t i = 0; i < size; i++)
        {
            if (radioButtons[i] != &radioButton)
            {
                if (radioButtons[i]->getSelected())
                {
                    radioButtons[i]->setSelected(false);
                }
            }
        }

        if (radioButtonSelectedCallback && radioButtonSelectedCallback->isValid())
        {
            radioButtonSelectedCallback->execute(radioButton);
        }
    }

    /**
     * Handles the event that a RadioButton has been deselected.
     *
     * @param  radioButton the RadioButton that has been deselected.
     */
    virtual void radioButtonDeselectedHandler(const AbstractButton& radioButton)
    {
        if (radioButtonDeselectedCallback && radioButtonDeselectedCallback->isValid())
        {
            radioButtonDeselectedCallback->execute(radioButton);
        }
    }
};

} // namespace touchgfx

#endif // RADIOBUTTONGROUP_HPP
