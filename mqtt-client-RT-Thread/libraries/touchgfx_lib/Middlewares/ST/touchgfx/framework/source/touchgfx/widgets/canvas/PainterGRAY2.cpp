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
#include <touchgfx/Color.hpp>
#include <touchgfx/widgets/canvas/PainterGRAY2.hpp>

namespace touchgfx
{
void PainterGRAY2::render(uint8_t* ptr,
                          int x,
                          int xAdjust,
                          int y,
                          unsigned count,
                          const uint8_t* covers)
{
    currentX = x + areaOffsetX;
    currentY = y + areaOffsetY;
    x += xAdjust;
    uint8_t totalAlpha = LCD::div255(widgetAlpha * painterAlpha);
    if (totalAlpha == 0xFF)
    {
        do
        {
            uint8_t alpha = *covers;
            covers++;

            if (alpha == 0xFF)
            {
                // Render a solid pixel
                LCD2setPixel(ptr, x, painterGray);
            }
            else
            {
                uint8_t ialpha = 0xFF - alpha;
                uint8_t p_gray = LCD2getPixel(ptr, x);
                LCD2setPixel(ptr, x, LCD::div255((painterGray * alpha + p_gray * ialpha) * 0x55) >> 6);
            }
            currentX++;
            x++;
        } while (--count != 0);
    }
    else if (totalAlpha != 0)
    {
        do
        {
            uint8_t alpha = LCD::div255((*covers) * totalAlpha);
            uint8_t ialpha = 0xFF - alpha;
            covers++;

            uint8_t p_gray = LCD2getPixel(ptr, x);
            LCD2setPixel(ptr, x, LCD::div255((painterGray * alpha + p_gray * ialpha) * 0x55) >> 6);
            currentX++;
            x++;
        } while (--count != 0);
    }
}

bool PainterGRAY2::renderNext(uint8_t& gray, uint8_t& alpha)
{
    gray = painterGray;
    alpha = painterAlpha;
    return true;
}
} // namespace touchgfx
