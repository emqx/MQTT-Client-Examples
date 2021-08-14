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

#include <touchgfx/widgets/canvas/PainterRGB565L8Bitmap.hpp>

namespace touchgfx
{
void PainterRGB565L8Bitmap::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::L8) && "The chosen painter only works with appropriate L8 bitmaps");
    bitmapRectToFrameBuffer = bitmap.getRect();
    DisplayTransformation::transformDisplayToFrameBuffer(bitmapRectToFrameBuffer);
}

void PainterRGB565L8Bitmap::render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers)
{
    uint16_t* p = reinterpret_cast<uint16_t*>(ptr) + (x + xAdjust);

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
    if ((Bitmap::ClutFormat)((const uint16_t*)bitmapExtraPointer)[-2] == Bitmap::CLUT_FORMAT_L8_RGB565)
    {
        const uint8_t* src = bitmapPointer;
        if (totalAlpha == 0xFF)
        {
            do
            {
                //use alpha from covers directly
                uint8_t alpha = *covers++;
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p = ((const uint16_t*)bitmapExtraPointer)[*src];
                }
                else
                {
                    // Non-Transparent pixel
                    *p = mixColors(((const uint16_t*)bitmapExtraPointer)[*src], *p, alpha);
                }
                p++;
                src++;
            } while (--count != 0);
        }
        else
        {
            do
            {
                uint8_t alpha = LCD::div255((*covers) * totalAlpha);
                covers++;

                *p = mixColors(((const uint16_t*)bitmapExtraPointer)[*src], *p, alpha);

                p++;
                src++;
            } while (--count != 0);
        }
    }
    else if ((Bitmap::ClutFormat)((const uint16_t*)bitmapExtraPointer)[-2] == Bitmap::CLUT_FORMAT_L8_RGB888)
    {
        if (totalAlpha == 0xFF)
        {
            do
            {
                const uint8_t* src = &bitmapExtraPointer[*bitmapPointer++ * 3];
                // Use alpha from covers directly
                uint8_t alpha = *covers++;
                uint8_t blue = *src++;
                uint8_t green = *src++;
                uint8_t red = *src;
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p++ = ((red << 8) & RMASK) | ((green << 3) & GMASK) | ((blue >> 3) & BMASK);
                }
                else
                {
                    uint8_t ialpha = 0xFF - alpha;
                    uint16_t bufpix = *p;
                    uint8_t fbr = (bufpix & RMASK) >> 11;
                    uint8_t fbg = (bufpix & GMASK) >> 5;
                    uint8_t fbb = bufpix & BMASK;
                    fbr = (fbr * 527 + 23) >> 6;
                    fbg = (fbg * 259 + 33) >> 6;
                    fbb = (fbb * 527 + 23) >> 6;
                    *p++ = ((LCD::div255(red * alpha + fbr * ialpha) << 8) & RMASK) | ((LCD::div255(green * alpha + fbg * ialpha) << 3) & GMASK) | ((LCD::div255(blue * alpha + fbb * ialpha) >> 3) & BMASK);
                }
            } while (--count != 0);
        }
        else
        {
            do
            {
                const uint8_t* src = &bitmapExtraPointer[*bitmapPointer++ * 3];
                uint8_t blue = *src++;
                uint8_t green = *src++;
                uint8_t red = *src;
                uint8_t alpha = LCD::div255((*covers) * totalAlpha);
                uint8_t ialpha = 0xFF - alpha;
                covers++;
                uint16_t bufpix = *p;
                uint8_t fbr = (bufpix & RMASK) >> 11;
                uint8_t fbg = (bufpix & GMASK) >> 5;
                uint8_t fbb = bufpix & BMASK;
                fbr = (fbr * 527 + 23) >> 6;
                fbg = (fbg * 259 + 33) >> 6;
                fbb = (fbb * 527 + 23) >> 6;
                *p++ = ((LCD::div255(red * alpha + fbr * ialpha) << 8) & RMASK) | ((LCD::div255(green * alpha + fbg * ialpha) << 3) & GMASK) | ((LCD::div255(blue * alpha + fbb * ialpha) >> 3) & BMASK);
            } while (--count != 0);
        }
    }
    else // Bitmap::CLUT_FORMAT_L8_ARGB8888
    {
        const uint8_t* src = bitmapPointer;
        if (totalAlpha == 0xFF)
        {
            do
            {
                uint32_t newpix = ((const uint32_t*)bitmapExtraPointer)[*src];
                uint8_t srcAlpha = newpix >> 24;
                uint8_t alpha = LCD::div255((*covers) * srcAlpha);
                covers++;
                if (alpha == 0xFF)
                {
                    // Solid pixel
                    *p = ((newpix >> 8) & RMASK) | ((newpix >> 5) & GMASK) | ((newpix >> 3) & BMASK);
                }
                else if (alpha)
                {
                    // Non-Transparent pixel
                    *p = mixColors((newpix >> 8) & RMASK, (newpix >> 5) & GMASK, (newpix >> 3) & BMASK, *p, alpha);
                }
                p++;
                src++;
            } while (--count != 0);
        }
        else
        {
            do
            {
                uint32_t newpix = ((const uint32_t*)bitmapExtraPointer)[*src];
                uint8_t srcAlpha = newpix >> 24;
                uint8_t alpha = LCD::div255((*covers) * LCD::div255(srcAlpha * totalAlpha));
                covers++;
                if (alpha)
                {
                    // Non-Transparent pixel
                    *p = mixColors((newpix >> 8) & RMASK, (newpix >> 5) & GMASK, (newpix >> 3) & BMASK, *p, alpha);
                }
                p++;
                src++;
            } while (--count != 0);
        }
    }
}

bool PainterRGB565L8Bitmap::renderInit()
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
        assert((bitmapExtraPointer != 0 && (*(const uint16_t*)bitmapExtraPointer == Bitmap::CLUT_FORMAT_L8_RGB565 || *(const uint16_t*)bitmapExtraPointer == Bitmap::CLUT_FORMAT_L8_ARGB8888 || *(const uint16_t*)bitmapExtraPointer == Bitmap::CLUT_FORMAT_L8_RGB888)));
        bitmapExtraPointer += 4; // Skip header
        return true;
    }

    return false;
}

bool PainterRGB565L8Bitmap::renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha)
{
    if (currentX >= bitmapRectToFrameBuffer.width)
    {
        return false;
    }

    if ((Bitmap::ClutFormat)((const uint16_t*)bitmapExtraPointer)[-2] == Bitmap::CLUT_FORMAT_L8_RGB565)
    {
        const uint16_t* clut = ((const uint16_t*)bitmapExtraPointer) + (*bitmapPointer++);
        uint16_t rgb565 = *clut;
        red = (rgb565 & RMASK) >> 8;
        red |= red >> 5; // To get full range 0-0xFF, not just 0-0xF8
        green = (rgb565 & GMASK) >> 3;
        green |= green >> 6; // To get full range 0-0xFF, not just 0-0xFC
        blue = (rgb565 & BMASK) << 3;
        blue |= (blue >> 5); // To get full range 0-0xFF, not just 0-0xF8
        alpha = 0xFF;
    }
    else if ((Bitmap::ClutFormat)((const uint16_t*)bitmapExtraPointer)[-2] == Bitmap::CLUT_FORMAT_L8_RGB888)
    {
        const uint8_t* clut = ((const uint8_t*)bitmapExtraPointer) + (*bitmapPointer++) * 3;
        alpha = 0xFF;
        blue = *clut++ & 0xF8;
        blue |= (blue >> 5); // To get full range 0-0xFF, not just 0-0xF8
        green = *clut++ & 0xFC;
        green |= green >> 6; // To get full range 0-0xFF, not just 0-0xFC
        red = *clut & 0xF8;
        red |= red >> 5; // To get full range 0-0xFF, not just 0-0xF8
    }
    else // Bitmap::CLUT_FORMAT_L8_ARGB8888
    {
        const uint32_t* clut = ((const uint32_t*)bitmapExtraPointer) + (*bitmapPointer++);
        uint32_t argb8888 = *(const uint32_t*)clut;
        alpha = (argb8888 >> 24) & 0xFF;
        red = (argb8888 >> 16) & 0xF8;
        red |= red >> 5; // To get full range 0-0xFF, not just 0-0xF8
        green = (argb8888 >> 8) & 0xFC;
        green |= green >> 6; // To get full range 0-0xFF, not just 0-0xFC
        blue = argb8888 & 0xF8;
        blue |= (blue >> 5); // To get full range 0-0xFF, not just 0-0xF8
    }
    // Apply given alpha from setAlpha()
    alpha = LCD::div255(alpha * painterAlpha);
    return true;
}
} // namespace touchgfx
