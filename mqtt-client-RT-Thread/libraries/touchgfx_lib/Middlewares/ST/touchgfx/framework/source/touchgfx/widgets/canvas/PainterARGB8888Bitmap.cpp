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

#include <touchgfx/widgets/canvas/PainterARGB8888Bitmap.hpp>

namespace touchgfx
{
void PainterARGB8888Bitmap::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::RGB565 || bitmap.getFormat() == Bitmap::RGB888 || bitmap.getFormat() == Bitmap::ARGB8888) && "The chosen painter only works with RGB565, RGB888 and ARGB8888 bitmaps");
    bitmapRectToFrameBuffer = bitmap.getRect();
    DisplayTransformation::transformDisplayToFrameBuffer(bitmapRectToFrameBuffer);
}

void PainterARGB8888Bitmap::render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers)
{
    uint8_t* RESTRICT p = ptr + ((x + xAdjust) * 4);

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
    if (bitmap.getFormat() == Bitmap::ARGB8888)
    {
        const uint32_t* RESTRICT src = bitmapARGB8888Pointer;
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
                    *p++ = 0xff;         // Alpha
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
                    pByte = *p;
                    cByte = (*src) >> 24;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
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
                    pByte = *p;
                    cByte = (*src) >> 24;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
                }
                else
                {
                    p += 4;
                }
                src++;
            } while (--count != 0);
        }
    }
    else if (bitmap.getFormat() == Bitmap::RGB888)
    {
        const uint8_t* RESTRICT src = bitmapRGB888Pointer;
        if (totalAlpha == 0xFF)
        {
            do
            {
                uint8_t alpha = *covers++;
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p++ = *src++; // Blue
                    *p++ = *src++; // Green
                    *p++ = *src++; // Red
                    *p++ = 0xff;   // Alpha
                }
                else
                {
                    // Non-Transparent pixel
                    uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    uint8_t cByte = *src++;
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                    pByte = *p;
                    cByte = *src++;
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                    pByte = *p;
                    cByte = *src++;
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
                }
            } while (--count != 0);
        }
        else
        {
            do
            {
                uint8_t alpha = LCD::div255((*covers) * totalAlpha);
                covers++;
                if (alpha)
                {
                    uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    uint8_t cByte = *src++;
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                    pByte = *p;
                    cByte = *src++;
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                    pByte = *p;
                    cByte = *src++;
                    *p++ = LCD::div255(cByte * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
                }
                else
                {
                    p += 4;
                    src += 3;
                }
            } while (--count != 0);
        }
    }
    else if (bitmap.getFormat() == Bitmap::RGB565)
    {
        const uint16_t* RESTRICT src = bitmapRGB565Pointer;
        if (totalAlpha == 0xFF)
        {
            do
            {
                uint8_t alpha = *covers++;
                uint16_t srcpix = *src++;
                uint16_t red = (srcpix & 0xF800) >> 11;
                uint16_t green = (srcpix & 0x07E0) >> 5;
                uint16_t blue = srcpix & 0x001F;
                red = (red * 527 + 23) >> 6;
                green = (green * 259 + 33) >> 6;
                blue = (blue * 527 + 23) >> 6;
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p++ = (uint8_t)blue;
                    *p++ = (uint8_t)green;
                    *p++ = (uint8_t)red;
                    *p++ = 0xff;
                }
                else
                {
                    // Non-Transparent pixel
                    uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    *p++ = LCD::div255(blue * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(green * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(red * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
                }
            } while (--count != 0);
        }
        else
        {
            do
            {
                uint8_t alpha = LCD::div255((*covers) * totalAlpha);
                covers++;
                uint16_t srcpix = *src++;
                uint16_t red = (srcpix & 0xF800) >> 11;
                uint16_t green = (srcpix & 0x07E0) >> 5;
                uint16_t blue = srcpix & 0x001F;
                red = (red * 527 + 23) >> 6;
                green = (green * 259 + 33) >> 6;
                blue = (blue * 527 + 23) >> 6;
                if (alpha)
                {
                    uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    *p++ = LCD::div255(blue * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(green * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(red * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
                }
                else
                {
                    p += 4;
                }
            } while (--count != 0);
        }
    }
}

bool PainterARGB8888Bitmap::renderInit()
{
    bitmapARGB8888Pointer = 0;
    bitmapRGB565Pointer = 0;
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
        bitmapRGB888Pointer = (const uint8_t*)bitmap.getData();
        if (!bitmapRGB888Pointer)
        {
            return false;
        }
        bitmapRGB888Pointer += (currentX + currentY * bitmapRectToFrameBuffer.width) * 3;
        return true;
    }

    if (bitmap.getFormat() == Bitmap::RGB565)
    {
        bitmapRGB565Pointer = (const uint16_t*)bitmap.getData();
        if (!bitmapRGB565Pointer)
        {
            return false;
        }
        bitmapRGB565Pointer += currentX + currentY * bitmapRectToFrameBuffer.width;
        return true;
    }

    return false;
}

bool PainterARGB8888Bitmap::renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha)
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
        alpha = 0xff;
    }
    else if (bitmapRGB565Pointer != 0)
    {
        uint16_t srcpix = *bitmapRGB565Pointer++;
        red = (srcpix & 0xF800) >> 11;
        green = (srcpix & 0x07E0) >> 5;
        blue = srcpix & 0x001F;
        red = (red * 527 + 23) >> 6;
        green = (green * 259 + 33) >> 6;
        blue = (blue * 527 + 23) >> 6;
        alpha = 0xff;
    }

    // Apply given alpha from setAlpha()
    alpha = LCD::div255(alpha * painterAlpha);
    return true;
}
} // namespace touchgfx
