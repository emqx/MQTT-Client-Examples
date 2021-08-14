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

#include <touchgfx/widgets/TouchArea.hpp>

namespace touchgfx
{
void TouchArea::handleClickEvent(const ClickEvent& event)
{
    AbstractButton::handleClickEvent(event);
    if (pressedAction && pressedAction->isValid() && pressed)
    {
        pressedAction->execute(*this);
    }
}
} // namespace touchgfx
