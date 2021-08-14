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
 * @file touchgfx/widgets/PixelDataWidget.hpp
 *
 * Declares the touchgfx::PixelDataWidget class.
 */
#ifndef PIXELDATAWIDGET_HPP
#define PIXELDATAWIDGET_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/Widget.hpp>

namespace touchgfx
{
/**
 * A widget for displaying a buffer of pixel data. This can also be though of as a dynamic
 * bitmap where the dimensions of the bitmap is the same as the dimensions of the widget
 * and the actual bitmap data can be set and updated dynamically. The size of the buffer
 * must match the number of bytes required for the widget calculated as WIDTH x HEIGHT x
 * BYTES_PER_PIXEL. If the LCD is 16 bit per pixel the buffer must hold 2 bytes for each
 * pixel. If the LCD is 24 bit the buffer must hold 3 bytes for each pixel.
 */
class PixelDataWidget : public Widget
{
public:
    PixelDataWidget();

    virtual void draw(const Rect& invalidatedArea) const;

    virtual Rect getSolidRect() const;

    /**
     * Set the pixel data to display. The given pointer must contain WIDTH x HEIGHT x
     * BYTES_PER_PIXEL bytes of addressable image data.
     *
     * @param [in] data Image data.
     *
     * @see setBitmapFormat
     */
    void setPixelData(uint8_t* const data);

    /**
     * Set the format of the pixel data. The supported formats depend on the display type.
     * For example grayscale displays do not support color images.
     *
     * @param  format Describes the format to use when reading the pixel data.
     */
    void setBitmapFormat(Bitmap::BitmapFormat format);

    /**
     * @copydoc Image::setAlpha
     */
    void setAlpha(uint8_t newAlpha);

    /**
     * @copydoc Image::getAlpha
     */
    uint8_t getAlpha() const;

protected:
    uint8_t* buffer;             ///< The buffer where the pixels are copied from
    Bitmap::BitmapFormat format; ///< The pixel format for the data.
    uint8_t alpha;               ///< The Alpha for this widget.
};

} // namespace touchgfx

#endif // PIXELDATAWIDGET_HPP
