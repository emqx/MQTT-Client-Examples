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
 * @file touchgfx/widgets/ButtonWithIcon.hpp
 *
 * Declares the touchgfx::ButtonWithIcon class.
 */
#ifndef BUTTONWITHICON_HPP
#define BUTTONWITHICON_HPP

#include <touchgfx/widgets/Button.hpp>

namespace touchgfx
{
/**
 * A Button that has a bitmap with an icon on top of it. It is possible to have two different
 * icons depending on the current state of the button (released or pressed).
 *
 * Typical use case could be a blue button with a released and a pressed image. Those
 * images can be reused across several buttons. The ButtonWithIcon will then allow an
 * image to superimposed on top of the blue button.
 */
class ButtonWithIcon : public Button
{
public:
    ButtonWithIcon();

#ifdef __IAR_SYSTEMS_ICC__    // Only include in IAR compilation
#pragma diag_suppress = Pe997 // Suppress warning for intentional virtual function override
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION < 6000000)
#pragma diag_suppress 997
#endif

    /**
     * Sets the four bitmaps used by this button. The last two bitmaps are drawn on top of
     * the first two, again depending on the current state of the Button. This means that
     * when the button state is released (normal), the newIconReleased is drawn on top of
     * the newBackgroundReleased, and when the button state is pressed, the newIconPressed
     * is drawn on top of the newBackgroundPressed.
     *
     * The default position if the icons is set to the center of the bitmaps. The two icons
     * are assumed to have the same dimensions. The size of the released icon is used to
     * position the icons centered on the Button.
     *
     * @param  newBackgroundReleased Bitmap to use when button is released.
     * @param  newBackgroundPressed  Bitmap to use when button is pressed.
     * @param  newIconReleased       The bitmap for the icon in the released (normal) button state.
     * @param  newIconPressed        The bitmap for the icon in the pressed button state.
     *
     * @note The user code must call invalidate() in order to update the button on the display.
     */
    virtual void setBitmaps(const Bitmap& newBackgroundReleased, const Bitmap& newBackgroundPressed,
                            const Bitmap& newIconReleased, const Bitmap& newIconPressed);
#ifdef __IAR_SYSTEMS_ICC__ // Only include in IAR compilation
#pragma diag_default = Pe997
#endif

    /**
     * Sets the x coordinate of the icon bitmaps. The same x coordinate is used for both
     * icons (released and pressed).
     *
     * @param  x The new x value, relative to the background bitmap. A negative value is
     *           allowed.
     *
     * @note The user code must call invalidate() in order to update the button on the display.
     * @note The value set is overwritten on a subsequent call to setBitmaps.
     */
    void setIconX(int16_t x)
    {
        iconX = x;
    }

    /**
     * Sets the y coordinate of the icon bitmaps. The same y coordinate is used for both
     * icons (released and pressed).
     *
     * @param  y The new y value, relative to the background bitmap. A negative value is
     *           allowed.
     *
     * @note The user code must call invalidate() in order to update the button on the display.
     * @note The value set is overwritten on a subsequent call to setBitmaps.
     */
    void setIconY(int16_t y)
    {
        iconY = y;
    }

    /**
     * Sets the x and y coordinates of the icon bitmap. Same as calling setIconX and
     * setIconY.
     *
     * @param  x The new x value, relative to the background bitmap. A negative value is
     *           allowed.
     * @param  y The new y value, relative to the background bitmap. A negative value is
     *           allowed.
     *
     * @note The user code must call invalidate() in order to update the button on the display.
     * @note The values set are overwritten on a subsequent call to setBitmaps.
     */
    void setIconXY(int16_t x, int16_t y)
    {
        setIconX(x);
        setIconY(y);
    }

    /**
     * Gets currently displayed icon. This depends on the current state of the button,
     * released (normal) or pressed.
     *
     * @return The icon currently displayed.
     */
    Bitmap getCurrentlyDisplayedIcon() const
    {
        return (pressed ? iconPressed : iconReleased);
    }

    /**
     * Gets the x coordinate of the icon bitmap.
     *
     * @return The x coordinate of the icon bitmap.
     */
    int16_t getIconX() const
    {
        return iconX;
    }

    /**
     * Gets the y coordinate of the icon bitmap.
     *
     * @return The y coordinate of the icon bitmap.
     */
    int16_t getIconY() const
    {
        return iconY;
    }

    virtual void draw(const Rect& invalidatedArea) const;

protected:
    Bitmap iconReleased; ///< Icon to display when button is not pressed.
    Bitmap iconPressed;  ///< Icon to display when button is pressed.
    int16_t iconX;       ///< x coordinate offset for icon.
    int16_t iconY;       ///< y coordinate offset for icon.
};

} // namespace touchgfx

#endif // BUTTONWITHICON_HPP
