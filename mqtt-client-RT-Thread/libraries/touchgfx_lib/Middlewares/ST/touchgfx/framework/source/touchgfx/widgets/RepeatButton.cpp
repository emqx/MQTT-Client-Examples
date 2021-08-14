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

#include <touchgfx/widgets/RepeatButton.hpp>

namespace touchgfx
{
RepeatButton::RepeatButton() : Button(), ticksDelay(30), ticksInterval(15), ticks(0), ticksBeforeContinuous(0)
{
}

void RepeatButton::setDelay(int delay)
{
    ticksDelay = delay;
}

int RepeatButton::getDelay()
{
    return ticksDelay;
}

void RepeatButton::setInterval(int interval)
{
    ticksInterval = interval;
}

int RepeatButton::getInterval()
{
    return ticksInterval;
}

void RepeatButton::handleClickEvent(const touchgfx::ClickEvent& event)
{
    pressed = false; // To prevent AbstractButton from calling action->execute().
    invalidate(); // Force redraw after forced state change
    Button::handleClickEvent(event);
    if (event.getType() == touchgfx::ClickEvent::PRESSED)
    {
        if (action && action->isValid())
        {
            action->execute(*this);
        }
        ticks = 0;
        ticksBeforeContinuous = ticksDelay;
        touchgfx::Application::getInstance()->registerTimerWidget(this);
    }
    else
    {
        touchgfx::Application::getInstance()->unregisterTimerWidget(this);
    }
}

void RepeatButton::handleTickEvent()
{
    Button::handleTickEvent();

    if (pressed)
    {
        if (ticks == ticksBeforeContinuous)
        {
            if (action && action->isValid())
            {
                action->execute(*this);
            }

            ticks = 0;
            ticksBeforeContinuous = ticksInterval;
        }
        else
        {
            ticks++;
        }
    }
}
} // namespace touchgfx
