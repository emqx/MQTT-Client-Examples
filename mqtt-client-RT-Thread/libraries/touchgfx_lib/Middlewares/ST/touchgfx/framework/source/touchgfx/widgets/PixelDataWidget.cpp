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

#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/widgets/PixelDataWidget.hpp>

namespace touchgfx
{
PixelDataWidget::PixelDataWidget()
    : Widget(),
      buffer(0),
      format(Bitmap::RGB888),
      alpha(255)
{
}

void PixelDataWidget::setPixelData(uint8_t* const data)
{
    buffer = data;
}

void PixelDataWidget::setBitmapFormat(Bitmap::BitmapFormat f)
{
    format = f;
}

void PixelDataWidget::setAlpha(uint8_t newAlpha)
{
    alpha = newAlpha;
}

uint8_t PixelDataWidget::getAlpha() const
{
    return alpha;
}

void PixelDataWidget::draw(const touchgfx::Rect& invalidatedArea) const
{
    if (buffer != 0)
    {
        //convert to lcd coordinates
        const touchgfx::Rect absolute = getAbsoluteRect();
        //copy to LCD
        HAL::lcd().blitCopy(buffer, format, absolute, invalidatedArea, alpha, false);
    }
}

touchgfx::Rect PixelDataWidget::getSolidRect() const
{
    Rect solidRect(0, 0, 0, 0);
    if (alpha == 255)
    {
        // There are at least some solid pixels
        switch (format)
        {
        case Bitmap::BW:     ///< 1-bit, black / white, no alpha channel
        case Bitmap::BW_RLE: ///< 1-bit, black / white, no alpha channel compressed with horizontal RLE
        case Bitmap::GRAY2:  ///< 2-bit, gray scale, no alpha channel
        case Bitmap::GRAY4:  ///< 4-bit, gray scale, no alpha channel
        case Bitmap::RGB565: ///< 16-bit, 5 bits for red, 6 bits for green, 5 bits for blue, no alpha channel
        case Bitmap::RGB888: ///< 16-bit, 5 bits for red, 6 bits for green, 5 bits for blue, no alpha channel
            //All solid pixels
            solidRect.width = getWidth();
            solidRect.height = getHeight();
            break;
        case Bitmap::ARGB8888: ///< 32-bit, 8 bits for each of red, green, blue and alpha channel
        case Bitmap::ARGB2222: ///< 8-bit color
        case Bitmap::ABGR2222: ///< 8-bit color
        case Bitmap::RGBA2222: ///< 8-bit color
        case Bitmap::BGRA2222: ///< 8-bit color
        case Bitmap::L8:       ///< 8-bit indexed color
        case Bitmap::A4:       ///< 4-bit alpha level
            //No knowledge about solid pixels
            break;
        }
    }
    return solidRect;
}
} // namespace touchgfx
