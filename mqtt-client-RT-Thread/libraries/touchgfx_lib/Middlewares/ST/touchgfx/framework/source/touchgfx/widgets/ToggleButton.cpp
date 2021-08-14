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

#include <touchgfx/widgets/ToggleButton.hpp>

namespace touchgfx
{
void ToggleButton::forceState(bool activeState)
{
    if (activeState)
    {
        // up should equal originalPressed
        if (up.getId() != originalPressed.getId())
        {
            down = up;
            up = originalPressed;
        }
    }
    else
    {
        // down should equal originalPressed
        if (down.getId() != originalPressed.getId())
        {
            up = down;
            down = originalPressed;
        }
    }
}

void ToggleButton::handleClickEvent(const ClickEvent& event)
{
    if (pressed && (event.getType() == ClickEvent::RELEASED))
    {
        Bitmap tmp = up;
        up = down;
        down = tmp;
    }
    Button::handleClickEvent(event);
}
} // namespace touchgfx
