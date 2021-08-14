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
 * @file touchgfx/containers/buttons/BoxWithBorderButtonStyle.hpp
 *
 * Declares the touchgfx::BoxWithBorderButtonStyle class.
 */
#ifndef BOXWITHBORDERBUTTONSTYLE_HPP
#define BOXWITHBORDERBUTTONSTYLE_HPP

#include <touchgfx/widgets/BoxWithBorder.hpp>

namespace touchgfx
{
/**
 * A box with border button style. This class is supposed to be used with one of the
 * ButtonTrigger classes to create a functional button. This class will show a box with
 * a border in different colors depending on the state of the button (pressed or
 * released).
 *
 * An image button style. This class is supposed to be used with one of the
 * ButtonTrigger classes to create a functional button. This class will show one of two
 * images depending on the state of the button (pressed or released).
 *
 * @tparam T Generic type parameter. Typically a AbstractButtonContainer subclass.
 *
 * @see AbstractButtonContainer, BoxWithBorder
 */
template <class T>
class BoxWithBorderButtonStyle : public T
{
public:
    BoxWithBorderButtonStyle()
        : T(), up(), down()
    {
        borderBox.setXY(0, 0);
        T::add(borderBox);
    }

    /**
     * Sets the size and position of this BoxWithBorderButtonStyle, relative to its parent.
     *
     * @param  x      The x coordinate of this BoxWithBorderButtonStyle.
     * @param  y      The y coordinate of this BoxWithBorderButtonStyle.
     * @param  width  The width of this BoxWithBorderButtonStyle.
     * @param  height The height of this BoxWithBorderButtonStyle.
     *
     * @note Changing this does not automatically yield a redraw.
     */
    void setBoxWithBorderPosition(int16_t x, int16_t y, int16_t width, int16_t height)
    {
        borderBox.setPosition(x, y, width, height);
    }

    /**
     * Sets the width.
     *
     * @param  width The width.
     */
    void setBoxWithBorderWidth(int16_t width)
    {
        borderBox.setWidth(width);
    }

    /**
     * Sets the height.
     *
     * @param  height The height.
     */
    void setBoxWithBorderHeight(int16_t height)
    {
        borderBox.setHeight(height);
    }

    /**
     * Sets the colors.
     *
     * @param  colorReleased       The color released.
     * @param  colorPressed        The color pressed.
     * @param  borderColorReleased The border color released.
     * @param  borderColorPressed  The border color pressed.
     */
    void setBoxWithBorderColors(const colortype colorReleased, const colortype colorPressed, const colortype borderColorReleased, const colortype borderColorPressed)
    {
        up = colorReleased;
        down = colorPressed;

        borderUp = borderColorReleased;
        borderDown = borderColorPressed;

        handlePressedUpdated();
    }

    /**
     * Sets border size.
     *
     * @param  size The size.
     */
    void setBorderSize(uint8_t size)
    {
        borderBox.setBorderSize(size);
    }

protected:
    BoxWithBorder borderBox; ///< The border box
    colortype up;            ///< The up
    colortype down;          ///< The down
    colortype borderUp;      ///< The border up
    colortype borderDown;    ///< The border down

    /** @copydoc AbstractButtonContainer::handlePressedUpdated() */
    virtual void handlePressedUpdated()
    {
        borderBox.setColor(T::getPressed() ? down : up);
        borderBox.setBorderColor(T::getPressed() ? borderDown : borderUp);
        T::handlePressedUpdated();
    }

    /** @copydoc AbstractButtonContainer::handleAlphaUpdated() */
    virtual void handleAlphaUpdated()
    {
        borderBox.setAlpha(T::getAlpha());
        T::handleAlphaUpdated();
    }
};

} // namespace touchgfx

#endif // BOXWITHBORDERBUTTONSTYLE_HPP
