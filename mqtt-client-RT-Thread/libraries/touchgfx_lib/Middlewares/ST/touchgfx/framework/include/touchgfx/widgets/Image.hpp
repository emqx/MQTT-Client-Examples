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
 * @file touchgfx/widgets/Image.hpp
 *
 * Declares the touchgfx::Image class.
 */
#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/Widget.hpp>

namespace touchgfx
{
/**
 * Simple widget capable of showing a bitmap on the display. The bitmap can be alpha-blended
 * with the background (or whichever other Drawable might be "underneath" the Image).
 * The bitmap can and have areas of varying opacity.
 *
 * The conversion from image.bmp or image.png to a bitmap that can be used in TouchGFX
 * is handled by the Image Converter as part of compiling the project. Each image is
 * assigned a unique BITMAP identifier which.
 *
 * @see Bitmap
 */
class Image : public Widget
{
public:
    /**
     * Constructs a new Image with a default alpha value of 255 (solid) and a default Bitmap
     * (undefined) if none is specified. If a Bitmap is passed to the constructor, the width
     * and height of this widget is set to those of the bitmap.
     *
     * @param  bmp (Optional) The bitmap to display.
     *
     * @see setBitmap
     */
    Image(const Bitmap& bmp = Bitmap())
        : Widget(), alpha(255)
    {
        setBitmap(bmp);
    }

    /**
     * Sets the bitmap for this Image and updates the width and height of this widget to
     * match those of the Bitmap.
     *
     * @param  bmp The bitmap instance.
     * @note The user code must call invalidate() in order to update the image on the display.
     */
    virtual void setBitmap(const Bitmap& bmp);

    /**
     * Sets the opacity (alpha value). This can be used to fade it away by gradually
     * decreasing the alpha value from 255 (solid) to 0 (invisible).
     *
     * @param  newAlpha The new alpha value. 255=solid, 0=invisible.
     *
     * @see getAlpha
     *
     * @note The user code must call invalidate() in order to update the display.
     */
    void setAlpha(uint8_t newAlpha)
    {
        alpha = newAlpha;
    }

    virtual void draw(const Rect& invalidatedArea) const;

    /**
     * Gets the BitmapId currently assigned to the Image widget.
     *
     * @return The current BitmapId of the widget.
     */
    BitmapId getBitmap() const
    {
        return bitmap.getId();
    }

    /**
     * Gets the current alpha value of the widget. The alpha value is in range 255
     * (solid) to 0 (invisible).
     *
     * @return The current alpha value.
     *
     * @see setAlpha
     */
    uint8_t getAlpha() const
    {
        return alpha;
    }

    virtual Rect getSolidRect() const;

protected:
    Bitmap bitmap;             ///< The Bitmap to display.
    uint8_t alpha;             ///< The Alpha for this image.
};

} // namespace touchgfx

#endif // IMAGE_HPP
