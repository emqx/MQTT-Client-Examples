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

#include <platform/driver/lcd/LCD2bpp.hpp>
#include <touchgfx/widgets/canvas/PainterGRAY2Bitmap.hpp>

namespace touchgfx
{
void PainterGRAY2Bitmap::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    assert((bitmap.getId() == BITMAP_INVALID || bitmap.getFormat() == Bitmap::GRAY2) && "The chosen painter only works with GRAY2 bitmaps");
    bitmapRectToFrameBuffer = bitmap.getRect();
    DisplayTransformation::transformDisplayToFrameBuffer(bitmapRectToFrameBuffer);
}

void PainterGRAY2Bitmap::render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers)
{
    currentX = x + areaOffsetX;
    currentY = y + areaOffsetY;
    x += xAdjust;

    if (!renderInit())
    {
        return;
    }

    if (currentX + (int)count > bitmapRectToFrameBuffer.width)
    {
        count = bitmapRectToFrameBuffer.width - currentX;
    }

    uint8_t totalAlpha = LCD::div255(widgetAlpha * painterAlpha);
    if (bitmapAlphaPointer)
    {
        if (totalAlpha == 0xFF)
        {
            do
            {
                uint8_t gray = LCD2getPixel(bitmapGRAY2Pointer, currentX);
                uint8_t alpha = LCD::div255((*covers) * (LCD2getPixel(bitmapAlphaPointer, currentX) * 0x55));
                covers++;

                if (alpha == 0xFF)
                {
                    // Render a solid pixel
                    LCD2setPixel(ptr, x, gray);
                }
                else
                {
                    uint8_t ialpha = 0xFF - alpha;
                    uint8_t p_gray = LCD2getPixel(ptr, x);
                    LCD2setPixel(ptr, x, LCD::div255((gray * alpha + p_gray * ialpha) * 0x55) >> 6);
                }
                currentX++;
                x++;
            } while (--count != 0);
        }
        else
        {
            do
            {
                uint8_t gray = LCD2getPixel(bitmapGRAY2Pointer, currentX);
                uint8_t alpha = LCD::div255((*covers) * LCD::div255(totalAlpha * (LCD2getPixel(bitmapAlphaPointer, currentX) * 0x55)));
                uint8_t ialpha = 0xFF - alpha;
                covers++;

                uint8_t p_gray = LCD2getPixel(ptr, x);
                LCD2setPixel(ptr, x, LCD::div255((gray * alpha + p_gray * ialpha) * 0x55) >> 6);
                currentX++;
                x++;
            } while (--count != 0);
        }
    }
    else
    {
        if (totalAlpha == 255)
        {
            do
            {
                uint8_t gray = LCD2getPixel(bitmapGRAY2Pointer, currentX);
                uint8_t alpha = (*covers);
                covers++;

                if (alpha == 255)
                {
                    // Render a solid pixel
                    LCD2setPixel(ptr, x, gray);
                }
                else
                {
                    uint8_t ialpha = 0xFF - alpha;
                    uint8_t p_gray = LCD2getPixel(ptr, x);
                    LCD2setPixel(ptr, x, LCD::div255((gray * alpha + p_gray * ialpha) * 0x55) >> 6);
                }
                currentX++;
                x++;
            } while (--count != 0);
        }
        else
        {
            do
            {
                uint8_t gray = LCD2getPixel(bitmapGRAY2Pointer, currentX);
                uint8_t alpha = LCD::div255((*covers) * totalAlpha);
                uint8_t ialpha = 0xFF - alpha;
                covers++;

                uint8_t p_gray = LCD2getPixel(ptr, x);
                LCD2setPixel(ptr, x, LCD::div255((gray * alpha + p_gray * ialpha) * 0x55) >> 6);
                currentX++;
                x++;
            } while (--count != 0);
        }
    }
}

bool PainterGRAY2Bitmap::renderInit()
{
    bitmapGRAY2Pointer = 0;
    bitmapAlphaPointer = 0;

    if (bitmap.getId() == BITMAP_INVALID)
    {
        return false;
    }

    if ((currentX >= bitmapRectToFrameBuffer.width) || (currentY >= bitmapRectToFrameBuffer.height))
    {
        // Outside bitmap area, do not draw anything
        return false;
    }

    if (bitmap.getFormat() == Bitmap::GRAY2)
    {
        bitmapGRAY2Pointer = (const uint8_t*)bitmap.getData();
        if (!bitmapGRAY2Pointer)
        {
            return false;
        }
        bitmapGRAY2Pointer += currentY * ((bitmapRectToFrameBuffer.width + 3) / 4);
        bitmapAlphaPointer = (const uint8_t*)bitmap.getExtraData();
        if (bitmapAlphaPointer)
        {
            bitmapAlphaPointer += currentY * ((bitmapRectToFrameBuffer.width + 3) / 4);
        }
        return true;
    }

    return false;
}

bool PainterGRAY2Bitmap::renderNext(uint8_t& gray, uint8_t& alpha)
{
    if (currentX >= bitmapRectToFrameBuffer.width)
    {
        return false;
    }

    if (bitmapGRAY2Pointer != 0)
    {
        gray = LCD2getPixel(bitmapGRAY2Pointer, currentX);
        if (bitmapAlphaPointer)
        {
            alpha = LCD2getPixel(bitmapAlphaPointer, currentX);
            alpha *= 0x55; // Upscale from 00-03 to 00-FF
        }
        else
        {
            alpha = 0xFF; // No alpha per pixel in the image, it is solid
        }
    }
    // Apply given alpha from setAlpha()
    alpha = LCD::div255(alpha * painterAlpha);
    return true;
}
} // namespace touchgfx
