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
 * @file touchgfx/containers/buttons/TextButtonStyle.hpp
 *
 * Declares the touchgfx::TextButtonStyle class.
 */
#ifndef TEXTBUTTONSTYLE_HPP
#define TEXTBUTTONSTYLE_HPP

#include <touchgfx/widgets/TextArea.hpp>

namespace touchgfx
{
/**
 * A text button style. This class is supposed to be used with one of the ButtonTrigger classes
 * to create a functional button. This class will show a text in one of two colors
 * depending on the state of the button (pressed or released).
 *
 * The TextButtonStyle does not set the size of the enclosing container (normally
 * AbstractButtonContainer). The size must be set manually.
 *
 * To get a background behind the text, use TextButtonStyle together with e.g.
 * ImageButtonStyle: TextButtonStyle<ImageButtonStyle<ClickButtonTrigger> >
 * myButton;
 *
 * The position of the text can be adjusted with setTextXY (default is centered).
 *
 * @see AbstractButtonContainer
 */
template <class T>
class TextButtonStyle : public T
{
public:
    TextButtonStyle()
        : T()
    {
        T::add(text);
    }

    /**
     * Sets a text.
     *
     * @param  t A TypedText to process.
     */
    void setText(TypedText t)
    {
        text.setTypedText(t);
        text.setWidth(T::getWidth());
        text.setHeight(T::getHeight());
    }

    /**
     * Sets text x coordinate.
     *
     * @param  x The x coordinate.
     */
    void setTextX(int16_t x)
    {
        text.setX(x);
    }

    /**
     * Sets text y coordinate.
     *
     * @param  y The y coordinate.
     */
    void setTextY(int16_t y)
    {
        text.setY(y);
    }

    /**
     * Sets text x and y.
     *
     * @param  x The x coordinate.
     * @param  y The y coordinate.
     */
    void setTextXY(int16_t x, int16_t y)
    {
        setTextX(x);
        setTextY(y);
    }

    /**
     * Sets text position.
     *
     * @param  x      The x coordinate.
     * @param  y      The y coordinate.
     * @param  width  The width of the text.
     * @param  height The height of the text.
     */
    void setTextPosition(int16_t x, int16_t y, int16_t width, int16_t height)
    {
        text.setPosition(x, y, width, height);
    }

    /**
     * Sets text rotation.
     *
     * @param  rotation The rotation.
     */
    void setTextRotation(TextRotation rotation)
    {
        text.setRotation(rotation);
    }

    /**
     * Sets text colors.
     *
     * @param  newColorReleased The new color released.
     * @param  newColorPressed  The new color pressed.
     */
    void setTextColors(colortype newColorReleased, colortype newColorPressed)
    {
        colorReleased = newColorReleased;
        colorPressed = newColorPressed;

        handlePressedUpdated();
    }

protected:
    TextArea text;           ///< The text
    colortype colorReleased; ///< The color released
    colortype colorPressed;  ///< The color pressed

    /** @copydoc AbstractButtonContainer::handlePressedUpdated() */
    virtual void handlePressedUpdated()
    {
        text.setColor(T::getPressed() ? colorPressed : colorReleased);
        T::handlePressedUpdated();
    }

    /** @copydoc AbstractButtonContainer::handleAlphaUpdated() */
    virtual void handleAlphaUpdated()
    {
        text.setAlpha(T::getAlpha());
        T::handleAlphaUpdated();
    }
};

} // namespace touchgfx

#endif // TEXTBUTTONSTYLE_HPP
