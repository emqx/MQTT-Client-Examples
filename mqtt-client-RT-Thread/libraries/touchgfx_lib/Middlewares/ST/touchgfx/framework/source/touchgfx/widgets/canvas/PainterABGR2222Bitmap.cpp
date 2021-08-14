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

#include <touchgfx/widgets/canvas/PainterABGR2222Bitmap.hpp>

namespace touchgfx
{
void PainterABGR2222Bitmap::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::ABGR2222) && "The chosen painter only works with ABGR2222 bitmaps");
    bitmapRectToFrameBuffer = bitmap.getRect();
    DisplayTransformation::transformDisplayToFrameBuffer(bitmapRectToFrameBuffer);
}

void PainterABGR2222Bitmap::render(uint8_t* ptr,
                                   int x,
                                   int xAdjust,
                                   int y,
                                   unsigned count,
                                   const uint8_t* covers)
{
    uint8_t* p = ptr + (x + xAdjust);

    currentX = x + areaOffsetX;
    currentY = y + areaOffsetY;

    if (!renderInit())
    {
        return;
    }

    if (currentX + (int)count > bitmapRectToFrameBuffer.width)
    {
        count = bitmapRectToFrameBuffer.width - currentX;
    }

    uint8_t totalAlpha = LCD::div255(widgetAlpha * painterAlpha);
    const uint8_t* src = bitmapABGR2222Pointer;
    if (totalAlpha == 0xFF)
    {
        do
        {
            uint8_t srcAlpha = ((*src) >> 6) * 0x55;
            uint8_t alpha = LCD::div255((*covers) * srcAlpha);
            covers++;
            if (alpha == 0xFF)
            {
                // Solid pixel
                *p = *src;
            }
            else if (alpha)
            {
                // Non-Transparent pixel
                *p = mixColors(*src, *p, alpha);
            }
            p++;
            src++;
        } while (--count != 0);
    }
    else
    {
        do
        {
            uint8_t srcAlpha = ((*src) >> 6) * 0x55;
            uint8_t alpha = LCD::div255((*covers) * srcAlpha);
            covers++;
            if (alpha) // This can never get to max=0xFF*0xFF as totalAlpha<255
            {
                // Non-Transparent pixel
                *p = mixColors(*src, *p, alpha);
            }
            p++;
            src++;
        } while (--count != 0);
    }
}

bool PainterABGR2222Bitmap::renderInit()
{
    bitmapABGR2222Pointer = 0;

    if (bitmap.getId() == BITMAP_INVALID)
    {
        return false;
    }

    if ((currentX >= bitmapRectToFrameBuffer.width) || (currentY >= bitmapRectToFrameBuffer.height))
    {
        // Outside bitmap area, do not draw anything
        // Consider the following instead of "return" to get a tiled image:
        //   currentX %= bitmapRectToFrameBuffer.width
        //   currentY %= bitmapRectToFrameBuffer.height
        return false;
    }

    if (bitmap.getFormat() == Bitmap::ABGR2222)
    {
        bitmapABGR2222Pointer = bitmap.getData();
        if (!bitmapABGR2222Pointer)
        {
            return false;
        }
        bitmapABGR2222Pointer += currentX + currentY * bitmapRectToFrameBuffer.width;
        return true;
    }

    return false;
}

bool PainterABGR2222Bitmap::renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha)
{
    if (currentX >= bitmapRectToFrameBuffer.width)
    {
        return false;
    }
    else if (bitmapABGR2222Pointer != 0)
    {
        uint16_t abgr2222 = *bitmapABGR2222Pointer++;
        red = LCD8bpp_ABGR2222::getRedFromColor(abgr2222);
        green = LCD8bpp_ABGR2222::getGreenFromColor(abgr2222);
        blue = LCD8bpp_ABGR2222::getBlueFromColor(abgr2222);
        alpha = (abgr2222 >> 6) * 0x55; // To get full range 0-0xFF
    }
    // Apply given alpha from setAlpha()
    alpha = LCD::div255(alpha * painterAlpha);
    return true;
}
} // namespace touchgfx
