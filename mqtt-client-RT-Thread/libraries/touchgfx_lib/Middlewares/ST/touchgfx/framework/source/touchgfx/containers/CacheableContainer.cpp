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

#include <touchgfx/containers/CacheableContainer.hpp>
#include <touchgfx/hal/HAL.hpp>

namespace touchgfx
{
void CacheableContainer::setCacheBitmap(BitmapId bitmapId)
{
    Bitmap bitmap(bitmapId);

    cachedBitmapId = BITMAP_INVALID;

    /* Accept only dynamic bitmaps */
    if (!Bitmap::dynamicBitmapGetAddress(bitmapId))
    {
        return;
    }

    /* Retrieve the auxiliary LCD implementation attached to HA */
    LCD* auxLCD = HAL::getInstance()->getAuxiliaryLCD();

    /* Dynamic bitmap and framebuffer should be of the same format */
    if (bitmap.getFormat() == HAL::lcd().framebufferFormat()
            || (auxLCD && bitmap.getFormat() == auxLCD->framebufferFormat()))
    {
        cachedBitmapId = bitmapId;
        cachedImage.setBitmap(Bitmap(cachedBitmapId));
    }
}

void CacheableContainer::updateCache()
{
    updateCache(Rect());
}

void CacheableContainer::updateCache(const Rect& invalidatedArea)
{
    if (isCachedMode && (cachedBitmapId != BITMAP_INVALID))
    {
        /* will call Container::draw(invalidatedArea) to render all widgets into the dynamic bitmap */
        HAL::getInstance()->drawDrawableInDynamicBitmap(*this, cachedBitmapId, invalidatedArea);
        childWasInvalidated = false;
    }
}

void CacheableContainer::enableCachedMode(bool enable)
{
    isCachedMode = enable;
}

void CacheableContainer::invalidateRect(Rect& invalidatedArea) const
{
    Container::invalidateRect(invalidatedArea);

    if (isCachedMode && (cachedBitmapId != BITMAP_INVALID))
    {
        const_cast<CacheableContainer*>(this)->childWasInvalidated = true;
    }
}

bool CacheableContainer::isChildInvalidated() const
{
    return childWasInvalidated;
}

void CacheableContainer::setupDrawChain(const Rect& invalidatedArea, Drawable** nextPreviousElement)
{
    if (isCachedMode && (cachedBitmapId != BITMAP_INVALID) && isVisible())
    {
        Rect r = getAbsoluteRect();
        cachedImage.setPosition(r.x, r.y, r.width, r.height);
        cachedImage.setupDrawChain(invalidatedArea, nextPreviousElement);
    }
    else
    {
        Container::setupDrawChain(invalidatedArea, nextPreviousElement);
    }
}
} // namespace touchgfx
