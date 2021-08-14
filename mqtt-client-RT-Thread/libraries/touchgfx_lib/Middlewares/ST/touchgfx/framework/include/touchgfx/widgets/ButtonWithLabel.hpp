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
 * @file touchgfx/widgets/ButtonWithLabel.hpp
 *
 * Declares the touchgfx::ButtonWithLabel class.
 */
#ifndef BUTTONWITHLABEL_HPP
#define BUTTONWITHLABEL_HPP

#include <touchgfx/TypedText.hpp>
#include <touchgfx/widgets/Button.hpp>

namespace touchgfx
{
/**
 * A Button that has a bitmap with a text on top of it. It is possible to have two different
 * colors for the text depending on the current state of the button (released or
 * pressed).
 *
 * Typical use case could be a red button with a normal and a pressed image. Those
 * images can be reused across several buttons. The ButtonWithLabel will then allow a
 * text to be superimposed on top of the red button.
 *
 * @see Button
 */
class ButtonWithLabel : public Button
{
public:
    ButtonWithLabel();

    /**
     * Sets the text to display on the button. Texts with wildcards are not supported.
     *
     * @param  t The text to display.
     *
     * @note The user code must call invalidate() in order to update the button on the display.
     */
    void setLabelText(TypedText t)
    {
        typedText = t;
        updateTextPosition();
    }

    /**
     * Gets the text used for the label.
     *
     * @return The text used for the label.
     */
    TypedText getLabelText() const
    {
        return typedText;
    }

    /**
     * Sets label color for the text when the button is in the normal, released state.
     *
     * @param  col               The color with which the text label should be drawn.
     *
     * @see setLabelColorPressed
     *
     * @note If the button is currently in the normal, released state, the button should be forced
     *       to redraw itself. This is done by calling invalidate() on the ButtonWithLabel.
     * @note The user code must call invalidate() in order to update the button on the display.
     */
    void setLabelColor(colortype col)
    {
        color = col;
    }

    ///@cond
    /**
     * Sets label color for the text when the button is in the normal, released state.
     *
     * @param  col               The color with which the text label should be drawn.
     * @param  performInvalidate If true, performs an instant invalidation of the button.
     *
     * @see setLabelColorPressed
     *
     * @note If the button is currently in the normal, released state, the button should be forced
     *       to redraw itself. This is done by calling invalidate() on the ButtonWithLabel.
     *
     * @deprecated The use of performInvalidate is scheduled to be removed
     */
    TOUCHGFX_DEPRECATED(
        "The use of performInvalidate is scheduled to be removed",
        void setLabelColor(colortype col, bool performInvalidate))
    {
        color = col;
        if (performInvalidate)
        {
            invalidate();
        }
    }
    ///@endcond

    /**
     * Sets label color for the text when the button is in the pressed state.
     *
     * @param  col               The color with which the text label should be drawn when the
     *                           button is pressed.
     *
     * @see setLabelColor
     *
     * @note If the button is currently in the pressed state, the button should be forced to
     *       redraw itself. This is done by calling invalidate() on the ButtonWithLabel.
     * @note The user code must call invalidate() in order to update the button on the display.
     */
    void setLabelColorPressed(colortype col)
    {
        colorPressed = col;
    }

    ///@cond
    /**
     * Sets label color for the text when the button is in the pressed state.
     *
     * @param  col               The color with which the text label should be drawn when the
     *                           button is pressed.
     * @param  performInvalidate If true, performs an instant invalidation of the button.
     *
     * @see setLabelColor
     *
     * @note If the button is currently in the pressed state, the button should be forced to
     *       redraw itself. This is done by calling invalidate() on the ButtonWithLabel.
     *
     * @deprecated The use of performInvalidate is scheduled to be removed
     */
    TOUCHGFX_DEPRECATED(
        "The use of performInvalidate is scheduled to be removed",
        void setLabelColorPressed(colortype col, bool performInvalidate))
    {
        colorPressed = col;
        if (performInvalidate)
        {
            invalidate();
        }
    }
    ///@endcond

    /**
     * Sets the rotation of the text on the label. The text can be rotated in steps of 90
     * degrees.
     *
     * @param  rotation The rotation of the text. Default is TEXT_ROTATE_0.
     *
     * @see TextArea::setRotation
     *
     * @note that this will not rotate the bitmap of the label, only the text.
     * @note The user code must call invalidate() in order to update the button on the display.
     */
    void setLabelRotation(TextRotation rotation)
    {
        this->rotation = rotation;
    }

    /**
     * Gets the current rotation of the text on the label.
     *
     * @return The current rotation of the text.
     */
    TextRotation getLabelRotation()
    {
        return rotation;
    }

    /**
     * Positions the label text horizontally centered. If the text changes due to a language
     * change you may need to reposition the label text by calling this function to keep the
     * text horizontally centered.
     *
     * @note The user code must call invalidate() in order to update the button on the display.
     */
    void updateTextPosition()
    {
        if (typedText.hasValidId())
        {
            const Font* f = typedText.getFont();
            const Unicode::UnicodeChar* s = typedText.getText();
            textHeightIncludingSpacing = f->getMaxTextHeight(s) * f->getNumberOfLines(s) + f->getSpacingAbove(s);
        }
        else
        {
            textHeightIncludingSpacing = 0;
        }
    }

    virtual Rect getSolidRect() const
    {
        return Button::getSolidRect();
    }

    virtual void draw(const Rect& area) const;

protected:
    TypedText typedText;                ///< The TypedText used for the button label.
    colortype color;                    ///< The color used for the label when the button is in the released, normal state.
    colortype colorPressed;             ///< The color used for the label when the button is in the pressed state.
    TextRotation rotation;              ///< The rotation used for the label.
    uint8_t textHeightIncludingSpacing; ///< Total height of the label (text height + spacing).
};

} // namespace touchgfx

#endif // BUTTONWITHLABEL_HPP
