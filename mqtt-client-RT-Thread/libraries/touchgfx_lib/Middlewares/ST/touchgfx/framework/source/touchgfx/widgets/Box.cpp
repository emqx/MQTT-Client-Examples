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

#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/hal/HAL.hpp>

namespace touchgfx
{
Rect Box::getSolidRect() const
{
    Rect solidRect(0, 0, 0, 0);
    if (reportAsSolid || alpha == 255)
    {
        solidRect.width = rect.width;
        solidRect.height = rect.height;
    }
    return solidRect;
}

void Box::draw(const Rect& area) const
{
    Rect dirty = area;
    translateRectToAbsolute(dirty);
    HAL::lcd().fillRect(dirty, color, alpha);
}
} // namespace touchgfx
