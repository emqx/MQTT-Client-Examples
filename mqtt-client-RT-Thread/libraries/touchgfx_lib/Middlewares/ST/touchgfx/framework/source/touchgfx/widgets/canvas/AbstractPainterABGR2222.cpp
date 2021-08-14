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

#include <touchgfx/Color.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterABGR2222.hpp>

namespace touchgfx
{
void AbstractPainterABGR2222::render(uint8_t* ptr,
                                     int x,
                                     int xAdjust,
                                     int y,
                                     unsigned count,
                                     const uint8_t* covers)
{
    uint8_t* p = ptr + (x + xAdjust);

    currentX = x + areaOffsetX;
    currentY = y + areaOffsetY;
    if (renderInit())
    {
        do
        {
            uint8_t red, green, blue, alpha;
            if (renderNext(red, green, blue, alpha))
            {
                uint8_t combinedAlpha = LCD::div255((*covers) * LCD::div255(alpha * widgetAlpha));
                covers++;

                if (combinedAlpha == 0xFF) // max alpha=0xFF on "*covers" and max alpha=0xFF on "widgetAlpha"
                {
                    // Render a solid pixel
                    renderPixel(p, red, green, blue);
                }
                else
                {
                    uint8_t ialpha = 0xFF - combinedAlpha;
                    uint8_t p_red = LCD8bpp_ABGR2222::getRedFromColor(*p);
                    uint8_t p_green = LCD8bpp_ABGR2222::getGreenFromColor(*p);
                    uint8_t p_blue = LCD8bpp_ABGR2222::getBlueFromColor(*p);
                    renderPixel(p,
                                LCD::div255(red * combinedAlpha + p_red * ialpha),
                                LCD::div255(green * combinedAlpha + p_green * ialpha),
                                LCD::div255(blue * combinedAlpha + p_blue * ialpha));
                }
            }
            p++;
            currentX++;
        } while (--count != 0);
    }
}

void AbstractPainterABGR2222::renderPixel(uint8_t* p, uint8_t red, uint8_t green, uint8_t blue)
{
    *p = LCD8bpp_ABGR2222::getColorFromRGB(red, green, blue);
}
} // namespace touchgfx
