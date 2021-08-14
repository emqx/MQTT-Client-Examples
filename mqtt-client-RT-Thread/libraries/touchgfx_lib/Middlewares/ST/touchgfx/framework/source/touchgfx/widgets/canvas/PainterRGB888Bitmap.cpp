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

#include <touchgfx/widgets/canvas/PainterRGB888Bitmap.hpp>

namespace touchgfx
{
void PainterRGB888Bitmap::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::RGB888 || bitmap.getFormat() == Bitmap::ARGB8888) && "The chosen painter only works with RGB888 and ARGB8888 bitmaps");
    bitmapRectToFrameBuffer = bitmap.getRect();
    DisplayTransformation::transformDisplayToFrameBuffer(bitmapRectToFrameBuffer);
}

void PainterRGB888Bitmap::render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers)
{
    uint8_t* p = ptr + ((x + xAdjust) * 3);

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
    if (bitmapRGB888Pointer)
    {
        const uint8_t* src = bitmapRGB888Pointer;
        uint8_t pByte;
        if (totalAlpha == 0xFF)
        {
            do
            {
                // Use alpha from covers directly
                uint8_t alpha = *covers++;
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p++ = *src++;
                    *p++ = *src++;
                    *p++ = *src++;
                }
                else
                {
                    uint8_t ialpha = 0xFF - alpha;
                    pByte = *p;
                    *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                }
            } while (--count != 0);
        }
        else
        {
            do
            {
                uint8_t alpha = LCD::div255((*covers) * totalAlpha);
                uint8_t ialpha = 0xFF - alpha;
                covers++;
                pByte = *p;
                *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                pByte = *p;
                *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                pByte = *p;
                *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
            } while (--count != 0);
        }
    }
    else if (bitmapARGB8888Pointer)
    {
        const uint32_t* src = bitmapARGB8888Pointer;
        if (totalAlpha == 0xFF)
        {
            do
            {
                uint8_t srcAlpha = (*src) >> 24;
                uint8_t alpha = LCD::div255((*covers) * srcAlpha);
                covers++;
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p++ = (*src);       // Blue
                    *p++ = (*src) >> 8;  // Green
                    *p++ = (*src) >> 16; // Red
                }
                else
                {
                    // Non-Transparent pixel
                    uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    uint8_t cByte = (*src);
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                    pByte = *p;
                    cByte = (*src) >> 8;
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                    pByte = *p;
                    cByte = (*src) >> 16;
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                }
                src++;
            } while (--count != 0);
        }
        else
        {
            do
            {
                uint8_t srcAlpha = (*src) >> 24;
                uint8_t alpha = LCD::div255((*covers) * LCD::div255(srcAlpha * totalAlpha));
                covers++;
                if (alpha)
                {
                    uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    uint8_t cByte = (*src);
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                    pByte = *p;
                    cByte = (*src) >> 8;
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                    pByte = *p;
                    cByte = (*src) >> 16;
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                }
                else
                {
                    p += 3;
                }
                src++;
            } while (--count != 0);
        }
    }
}

bool PainterRGB888Bitmap::renderInit()
{
    bitmapARGB8888Pointer = 0;
    bitmapRGB888Pointer = 0;

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

    if (bitmap.getFormat() == Bitmap::ARGB8888)
    {
        bitmapARGB8888Pointer = (const uint32_t*)bitmap.getData();
        if (!bitmapARGB8888Pointer)
        {
            return false;
        }
        bitmapARGB8888Pointer += currentX + currentY * bitmapRectToFrameBuffer.width;
        return true;
    }

    if (bitmap.getFormat() == Bitmap::RGB888)
    {
        bitmapRGB888Pointer = bitmap.getData();
        if (!bitmapRGB888Pointer)
        {
            return false;
        }
        bitmapRGB888Pointer += (currentX + currentY * bitmapRectToFrameBuffer.width) * 3;
        return true;
    }

    return false;
}

bool PainterRGB888Bitmap::renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha)
{
    if (currentX >= bitmapRectToFrameBuffer.width)
    {
        return false;
    }

    if (bitmapARGB8888Pointer != 0)
    {
        uint32_t argb8888 = *bitmapARGB8888Pointer++;
        alpha = (argb8888 >> 24) & 0xFF;
        red = (argb8888 >> 16) & 0xFF;
        green = (argb8888 >> 8) & 0xFF;
        blue = (argb8888) & 0xFF;
    }
    else if (bitmapRGB888Pointer != 0)
    {
        blue = *bitmapRGB888Pointer++;
        green = *bitmapRGB888Pointer++;
        red = *bitmapRGB888Pointer++;
        alpha = 0xFF; // No alpha per pixel in the image, it is solid
    }
    // Apply given alpha from setAlpha()
    alpha = LCD::div255(alpha * painterAlpha);
    return true;
}
} // namespace touchgfx
