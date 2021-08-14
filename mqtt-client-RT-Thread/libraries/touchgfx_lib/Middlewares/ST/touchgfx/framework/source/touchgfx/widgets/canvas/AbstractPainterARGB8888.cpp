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
#include <touchgfx/widgets/canvas/AbstractPainterARGB8888.hpp>

namespace touchgfx
{
void AbstractPainterARGB8888::render(uint8_t* ptr,
                                     int x,
                                     int xAdjust,
                                     int y,
                                     unsigned count,
                                     const uint8_t* covers)
{
    uint8_t* p = ptr + ((x + xAdjust) * 4);

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
                    renderPixel(reinterpret_cast<uint16_t*>(p), red, green, blue);
                }
                else
                {
                    uint8_t ialpha = 0xFF - combinedAlpha;
                    uint8_t p_blue = p[0];
                    uint8_t p_green = p[1];
                    uint8_t p_red = p[2];
                    uint8_t p_alpha = p[3];
                    renderPixel(reinterpret_cast<uint16_t*>(p),
                                LCD::div255(red * combinedAlpha + p_red * ialpha),
                                LCD::div255(green * combinedAlpha + p_green * ialpha),
                                LCD::div255(blue * combinedAlpha + p_blue * ialpha),
                                p_alpha + combinedAlpha - LCD::div255(p_alpha * combinedAlpha));
                }
            }
            p += 4;
            currentX++;
        } while (--count != 0);
    }
}

void AbstractPainterARGB8888::renderPixel(uint16_t* p, uint8_t red, uint8_t green, uint8_t blue)
{
    renderPixel(p, red, green, blue, 0xFF);
}

void AbstractPainterARGB8888::renderPixel(uint16_t* p, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    uint8_t* p8 = reinterpret_cast<uint8_t*>(p);
    p8[0] = blue;
    p8[1] = green;
    p8[2] = red;
    p8[3] = alpha;
}
} // namespace touchgfx
