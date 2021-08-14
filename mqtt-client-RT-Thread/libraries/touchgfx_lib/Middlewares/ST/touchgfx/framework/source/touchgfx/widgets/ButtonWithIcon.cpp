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

#include <touchgfx/widgets/ButtonWithIcon.hpp>
#include <touchgfx/hal/HAL.hpp>

namespace touchgfx
{
ButtonWithIcon::ButtonWithIcon() :
    Button(),
    iconX(0),
    iconY(0)
{
}

void ButtonWithIcon::setBitmaps(const Bitmap& newBackgroundReleased, const Bitmap& newBackgroundPressed,
                                const Bitmap& newIconReleased, const Bitmap& newIconPressed)
{
    Button::setBitmaps(newBackgroundReleased, newBackgroundPressed);

    iconReleased = newIconReleased;
    iconPressed = newIconPressed;

    iconX = (getWidth() / 2) - (newIconPressed.getWidth() / 2);
    iconY = (getHeight() / 2) - (newIconPressed.getHeight() / 2);
}

void ButtonWithIcon::draw(const Rect& invalidatedArea) const
{
    Button::draw(invalidatedArea);

    Bitmap bmp((pressed ? iconPressed : iconReleased));
    Rect iconRect(iconX, iconY, bmp.getWidth(), bmp.getHeight());
    Rect dirty = invalidatedArea & iconRect;
    if ((bmp.getId() != BITMAP_INVALID) && !dirty.isEmpty())
    {
        Rect r;
        translateRectToAbsolute(r);
        dirty.x -= iconX;
        dirty.y -= iconY;
        HAL::lcd().drawPartialBitmap(bmp, r.x + iconX, r.y + iconY, dirty, alpha);
    }
}
} // namespace touchgfx
