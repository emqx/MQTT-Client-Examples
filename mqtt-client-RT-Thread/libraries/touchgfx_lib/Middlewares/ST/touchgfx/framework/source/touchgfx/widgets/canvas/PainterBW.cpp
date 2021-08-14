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

#include <touchgfx/widgets/canvas/PainterBW.hpp>

namespace touchgfx
{
void PainterBW::render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* /*covers*/)
{
    currentX = x + areaOffsetX;
    currentY = y + areaOffsetY;
    x += xAdjust;
    unsigned char* p = ptr + (x / 8);

    do
    {
        unsigned pixel = 1 << (7 - (x % 8));
        if (!painterColor)
        {
            *p &= ~pixel;
        }
        else
        {
            *p |= pixel;
        }
        if (((++x) % 8) == 0)
        {
            p++;
        }
        currentX++;
    } while (--count);
}

bool PainterBW::renderNext(uint8_t& color)
{
    color = painterColor;
    return true;
}
} // namespace touchgfx
