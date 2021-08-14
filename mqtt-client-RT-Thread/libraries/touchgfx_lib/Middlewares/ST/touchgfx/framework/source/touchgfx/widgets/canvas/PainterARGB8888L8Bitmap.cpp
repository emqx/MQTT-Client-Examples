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

#include <touchgfx/widgets/canvas/PainterARGB8888L8Bitmap.hpp>

namespace touchgfx
{
void PainterARGB8888L8Bitmap::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::L8) && "The chosen painter only works with L8 bitmaps");
    bitmapRectToFrameBuffer = bitmap.getRect();
    DisplayTransformation::transformDisplayToFrameBuffer(bitmapRectToFrameBuffer);
}

void PainterARGB8888L8Bitmap::render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers)
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
    switch ((Bitmap::ClutFormat)((const uint16_t*)bitmapExtraPointer)[-2])
    {
    case Bitmap::CLUT_FORMAT_L8_RGB565:
        if (totalAlpha == 0xFF)
        {
            do
            {
                const uint16_t srcpix = ((const uint16_t*)bitmapExtraPointer)[*bitmapPointer++];
                uint8_t alpha = *covers++;
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
                const uint16_t srcpix = ((const uint16_t*)bitmapExtraPointer)[*bitmapPointer++];
                uint8_t alpha = LCD::div255((*covers) * totalAlpha);
                covers++;
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
        break;
    case Bitmap::CLUT_FORMAT_L8_RGB888:
        if (totalAlpha == 0xFF)
        {
            do
            {
                const uint8_t* src = bitmapExtraPointer + *bitmapPointer++ * 3;
                uint8_t alpha = *covers++;
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p++ = *src++; // Blue
                    *p++ = *src++; // Green
                    *p++ = *src;   // Red
                    *p++ = 0xff;   // Alpha
                }
                else
                {
                    // Non-Transparent pixel
                    uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(*src * alpha + pByte * ialpha);
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
                    const uint8_t* src = bitmapExtraPointer + *bitmapPointer++ * 3;
                    uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(*src++ * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(*src * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
                }
                else
                {
                    p += 4;
                }
            } while (--count != 0);
        }
        break;
    case Bitmap::CLUT_FORMAT_L8_ARGB8888:
        if (totalAlpha == 0xFF)
        {
            do
            {
                uint32_t src = ((const uint32_t*)bitmapExtraPointer)[*bitmapPointer++];
                uint8_t srcAlpha = src >> 24;
                uint8_t alpha = LCD::div255((*covers) * srcAlpha);
                covers++;
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p++ = src;       // Blue
                    *p++ = src >> 8;  // Green
                    *p++ = src >> 16; // Red
                    *p++ = 0xff;      // Alpha
                }
                else
                {
                    // Non-Transparent pixel
                    uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    *p++ = LCD::div255((src & 0xFF) * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(((src >> 8) & 0xFF) * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(((src >> 16) & 0xFF) * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
                }
            } while (--count != 0);
        }
        else
        {
            do
            {
                uint32_t src = ((const uint32_t*)bitmapExtraPointer)[*bitmapPointer++];
                uint8_t srcAlpha = src >> 24;
                uint8_t alpha = LCD::div255((*covers) * LCD::div255(srcAlpha * totalAlpha));
                covers++;
                if (alpha)
                {
                    uint8_t ialpha = 0xFF - alpha;
                    uint8_t pByte = *p;
                    *p++ = LCD::div255((src & 0xFF) * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(((src >> 8) & 0xFF) * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = LCD::div255(((src >> 16) & 0xFF) * alpha + pByte * ialpha);
                    pByte = *p;
                    *p++ = pByte + alpha - LCD::div255(pByte * alpha);
                }
                else
                {
                    p += 4;
                }
            } while (--count != 0);
        }
        break;
    }
}

bool PainterARGB8888L8Bitmap::renderInit()
{
    bitmapPointer = 0;
    bitmapExtraPointer = 0;

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

    if (bitmap.getFormat() == Bitmap::L8)
    {
        bitmapPointer = bitmap.getData();
        if (!bitmapPointer)
        {
            return false;
        }
        bitmapPointer += currentX + currentY * bitmapRectToFrameBuffer.width;
        bitmapExtraPointer = bitmap.getExtraData();
        assert((bitmapExtraPointer != 0 && (*(const uint16_t*)bitmapExtraPointer == Bitmap::CLUT_FORMAT_L8_RGB565 || *(const uint16_t*)bitmapExtraPointer == Bitmap::CLUT_FORMAT_L8_RGB888 || *(const uint16_t*)bitmapExtraPointer == Bitmap::CLUT_FORMAT_L8_ARGB8888)));
        bitmapExtraPointer += 4; // Skip header
        return true;
    }

    return false;
}

bool PainterARGB8888L8Bitmap::renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha)
{
    if (currentX >= bitmapRectToFrameBuffer.width)
    {
        return false;
    }

    switch ((Bitmap::ClutFormat)((const uint16_t*)bitmapExtraPointer)[-2])
    {
    case Bitmap::CLUT_FORMAT_L8_RGB565:
        {
            uint16_t srcpix = ((const uint16_t*)bitmapExtraPointer)[*bitmapPointer++];
            red = (srcpix & 0xF800) >> 11;
            green = (srcpix & 0x07E0) >> 5;
            blue = srcpix & 0x001F;
            red = (red * 527 + 23) >> 6;
            green = (green * 259 + 33) >> 6;
            blue = (blue * 527 + 23) >> 6;
            alpha = 0xff;
        }
        break;
    case Bitmap::CLUT_FORMAT_L8_RGB888:
        {
            const uint8_t* clut = bitmapExtraPointer + *bitmapPointer++ * 3;
            blue = *clut++;
            green = *clut++;
            red = *clut;
            alpha = 0xff;
        }
        break;
    case Bitmap::CLUT_FORMAT_L8_ARGB8888:
        {
            uint32_t argb8888 = ((const uint32_t*)bitmapExtraPointer)[*bitmapPointer++];
            alpha = (argb8888 >> 24) & 0xFF;
            red = (argb8888 >> 16) & 0xFF;
            green = (argb8888 >> 8) & 0xFF;
            blue = (argb8888) & 0xFF;
        }
        break;
    }

    // Apply given alpha from setAlpha()
    alpha = LCD::div255(alpha * painterAlpha);
    return true;
}
} // namespace touchgfx
