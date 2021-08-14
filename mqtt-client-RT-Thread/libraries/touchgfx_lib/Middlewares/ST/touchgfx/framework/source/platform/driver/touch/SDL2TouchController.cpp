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

#include <platform/driver/touch/SDL2TouchController.hpp>
#include <platform/hal/simulator/sdl2/HALSDL2.hpp>

namespace touchgfx
{
void touchgfx::SDL2TouchController::init()
{
}

bool touchgfx::SDL2TouchController::sampleTouch(int32_t& x, int32_t& y)
{
    return static_cast<HALSDL2*>(HAL::getInstance())->doSampleTouch(x, y); //lint !e1774
}
} // namespace touchgfx
