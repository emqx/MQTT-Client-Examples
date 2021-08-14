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

#include <touchgfx/widgets/SnapshotWidget.hpp>
#include <touchgfx/hal/HAL.hpp>

namespace touchgfx
{
SnapshotWidget::SnapshotWidget() : Widget(), bitmapId(BITMAP_INVALID), alpha(255)
{
}

void SnapshotWidget::draw(const Rect& invalidatedArea) const
{
    if (alpha == 0 || bitmapId == BITMAP_INVALID)
    {
        return;
    }

    Rect absRect(0, 0, Bitmap(bitmapId).getWidth(), rect.height);
    translateRectToAbsolute(absRect);
    HAL::lcd().blitCopy((const uint16_t*)Bitmap(bitmapId).getData(), absRect, invalidatedArea, alpha, false);
}

Rect SnapshotWidget::getSolidRect() const
{
    if (alpha < 255 || bitmapId == BITMAP_INVALID)
    {
        return Rect(0, 0, 0, 0);
    }
    else
    {
        return Rect(0, 0, getWidth(), getHeight());
    }
}

void SnapshotWidget::makeSnapshot()
{
    makeSnapshot(BITMAP_ANIMATION_STORAGE);
}

void SnapshotWidget::makeSnapshot(const BitmapId bmp)
{
    Rect visRect(0, 0, rect.width, rect.height);
    getVisibleRect(visRect);
    Rect absRect = getAbsoluteRect();
    bitmapId = (HAL::lcd().copyFrameBufferRegionToMemory(visRect, absRect, bmp)) ? bmp : BITMAP_INVALID;
}
} // namespace touchgfx
