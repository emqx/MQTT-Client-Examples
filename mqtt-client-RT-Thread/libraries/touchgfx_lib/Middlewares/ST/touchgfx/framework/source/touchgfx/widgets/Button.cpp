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

#include <touchgfx/widgets/Button.hpp>
#include <touchgfx/hal/HAL.hpp>

namespace touchgfx
{
void Button::draw(const Rect& invalidatedArea) const
{
    Bitmap bmp(AbstractButton::pressed ? down : up);
    Rect dirty(0, 0, bmp.getWidth(), bmp.getHeight());
    dirty &= invalidatedArea;
    if ((bmp.getId() != BITMAP_INVALID) && !dirty.isEmpty())
    {
        Rect r;
        translateRectToAbsolute(r);
        HAL::lcd().drawPartialBitmap(bmp, r.x, r.y, dirty, alpha);
    }
}

void Button::setBitmaps(const Bitmap& bmpReleased, const Bitmap& bmpPressed)
{
    up = bmpReleased;
    down = bmpPressed;
    // Adjust width and height of this widget to match bitmap. It is assumed
    // that the two bitmaps have same dimensions.
    Drawable::setWidth(down.getWidth());
    Drawable::setHeight(down.getHeight());
}

Rect Button::getSolidRect() const
{
    if (alpha < 255)
    {
        return Rect(0, 0, 0, 0);
    }

    return (AbstractButton::pressed ? down.getSolidRect() : up.getSolidRect());
}
} // namespace touchgfx
