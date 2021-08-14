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
 * @file touchgfx/containers/buttons/IconButtonStyle.hpp
 *
 * Declares the touchgfx::IconButtonStyle class.
 */
#ifndef ICONBUTTONSTYLE_HPP
#define ICONBUTTONSTYLE_HPP

#include <touchgfx/widgets/Image.hpp>

namespace touchgfx
{
/**
 * An icon button style. This class is supposed to be used with one of the ButtonTrigger classes
 * to create a functional button. This class will show one of two icons depending on the
 * state of the button (pressed or released).
 *
 * To get a background behind the icon, use IconButtonStyle together with e.g.
 * ImageButtonStyle: IconButtonStyle<ImageButtonStyle<ClickButtonTrigger> >
 * myButton;
 *
 * The IconButtonStyle will center the icon on the enclosing container (normally
 * AbstractButtonContainer). Set the size of the button before setting the icons.
 *
 * The position of the icon can be adjusted with setIconXY.
 *
 * @see AbstractButtonContainer
 */
template <class T>
class IconButtonStyle : public T
{
public:
    IconButtonStyle()
        : T()
    {
        T::add(iconImage);
    }

    /**
     * Sets icon bitmaps.
     *
     * @param  newIconReleased The new icon released.
     * @param  newIconPressed  The new icon pressed.
     */
    virtual void setIconBitmaps(const Bitmap& newIconReleased, const Bitmap& newIconPressed)
    {
        iconReleased = newIconReleased;
        iconPressed = newIconPressed;

        iconImage.setXY((T::getWidth() / 2) - (newIconPressed.getWidth() / 2), (T::getHeight() / 2) - (newIconPressed.getHeight() / 2));

        handlePressedUpdated();
    }

    /**
     * Sets icon x coordinate.
     *
     * @param  x The x coordinate.
     */
    void setIconX(int16_t x)
    {
        iconImage.setX(x);
    }

    /**
     * Sets icon y coordinate.
     *
     * @param  y The y coordinate.
     */
    void setIconY(int16_t y)
    {
        iconImage.setY(y);
    }

    /**
     * Sets the position of the icon.
     *
     * @param  x The x coordinate.
     * @param  y The y coordinate.
     */
    void setIconXY(int16_t x, int16_t y)
    {
        setIconX(x);
        setIconY(y);
    }

    /**
     * Gets currently displayed icon.
     *
     * @return The currently displayed icon.
     */
    Bitmap getCurrentlyDisplayedIcon() const
    {
        return (T::getPressed() ? iconPressed : iconReleased);
    }

    /**
     * Gets icon x coordinate.
     *
     * @return The icon x coordinate.
     */
    int16_t getIconX() const
    {
        return iconImage.getX();
    }

    /**
     * Gets icon y coordinate.
     *
     * @return The icon y coordinate.
     */
    int16_t getIconY() const
    {
        return iconImage.getY();
    }

protected:
    Bitmap iconReleased; ///< Icon to display when button is not pressed.
    Bitmap iconPressed;  ///< Icon to display when button is pressed.
    Image iconImage;     ///< The icon image

    /** @copydoc AbstractButtonContainer::handlePressedUpdated() */
    virtual void handlePressedUpdated()
    {
        iconImage.setBitmap(T::getPressed() ? iconPressed : iconReleased);
        T::handlePressedUpdated();
    }

    /** @copydoc AbstractButtonContainer::handleAlphaUpdated() */
    virtual void handleAlphaUpdated()
    {
        iconImage.setAlpha(T::getAlpha());
        T::handleAlphaUpdated();
    }
};

} // namespace touchgfx

#endif // ICONBUTTONSTYLE_HPP
