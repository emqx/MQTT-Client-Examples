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

#include <touchgfx/hal/OSWrappers.hpp>
#include <SDL2/SDL.h>

namespace touchgfx
{
static SDL_mutex* s_FrameBufferLock;

void OSWrappers::initialize()
{
    // Setup framebuffer lock
    s_FrameBufferLock = SDL_CreateMutex();
}

void OSWrappers::takeFrameBufferSemaphore()
{
    SDL_LockMutex(s_FrameBufferLock);
}

void OSWrappers::giveFrameBufferSemaphore()
{
    SDL_UnlockMutex(s_FrameBufferLock);
}

void OSWrappers::waitForVSync()
{}

void OSWrappers::tryTakeFrameBufferSemaphore()
{}

void OSWrappers::giveFrameBufferSemaphoreFromISR()
{}
} // namespace touchgfx
