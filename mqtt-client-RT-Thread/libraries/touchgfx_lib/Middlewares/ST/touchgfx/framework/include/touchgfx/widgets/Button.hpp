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
 * @file touchgfx/widgets/Button.hpp
 *
 * Declares the touchgfx::Button class.
 */
#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/widgets/AbstractButton.hpp>

namespace touchgfx
{
/**
 * A button with two images. One image showing the unpressed button and one image showing the
 * pressed state.
 */
class Button : public AbstractButton
{
public:
    Button()
        : AbstractButton(), up(), down(), alpha(255)
    {
    }

    virtual void draw(const Rect& invalidatedArea) const;

    /**
     * Sets the two bitmaps used by this button. One bitmap for the released (normal) state
     * and one bitmap for the pressed state. The images are expected to be of the same
     * dimensions, and the Button is resized to the dimensions of the pressed Bitmap.
     *
     * @param  bmpReleased Bitmap to use when button is released.
     * @param  bmpPressed  Bitmap to use when button is pressed.
     *
     * @note It is assumed that the dimensions of the bitmaps are the same. Unexpected (visual)
     *       behavior may be observed if the bitmaps are of different sizes.
     * @note The user code must call invalidate() in order to update the button on the display.
     */
    virtual void setBitmaps(const Bitmap& bmpReleased, const Bitmap& bmpPressed);

    virtual Rect getSolidRect() const;

    /**
     * @copydoc Image::setAlpha
     */
    void setAlpha(uint8_t newAlpha)
    {
        alpha = newAlpha;
    }

    /**
     * @copydoc Image::getAlpha
     */
    uint8_t getAlpha() const
    {
        return alpha;
    }

    /**
     * Gets currently displayed bitmap. This depends on the current state of the button,
     * released (normal) or pressed.
     *
     * @return The bitmap currently displayed.
     */
    Bitmap getCurrentlyDisplayedBitmap() const
    {
        return (pressed ? down : up);
    }

protected:
    Bitmap up;     ///< The image to display when button is released (normal state).
    Bitmap down;   ///< The image to display when button is pressed.
    uint8_t alpha; ///< The current alpha value. 255=solid, 0=invisible.
};

} // namespace touchgfx

#endif // BUTTON_HPP
