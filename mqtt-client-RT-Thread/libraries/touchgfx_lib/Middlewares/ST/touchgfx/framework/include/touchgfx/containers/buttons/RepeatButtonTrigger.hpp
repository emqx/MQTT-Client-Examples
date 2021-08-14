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

/**
 * @file touchgfx/containers/buttons/RepeatButtonTrigger.hpp
 *
 * Declares the touchgfx::RepeatButtonTrigger class.
 */
#ifndef REPEATBUTTONTRIGGER_HPP
#define REPEATBUTTONTRIGGER_HPP

#include <touchgfx/containers/buttons/AbstractButtonContainer.hpp>

namespace touchgfx
{
/**
 * A repeat button trigger. This trigger will create a button that reacts to a consistent touch.
 * This means it will call the set action repeatedly as long as it is touched. The
 * RepeatButtonTrigger can be combined with one or more of the ButtonStyle classes to
 * create a fully functional button.
 */
class RepeatButtonTrigger : public AbstractButtonContainer
{
public:
    RepeatButtonTrigger()
        : AbstractButtonContainer(), ticksDelay(30), ticksInterval(15), ticks(0), ticksBeforeContinuous(0)
    {
    }

    /**
     * Sets the delay (in number of ticks) from the first button activation until the next
     * time it will be automatically activated.
     *
     * @param  delay The delay, measured in ticks, between first activation and second activation.
     *
     * @see setInterval, getDelay
     */
    void setDelay(int delay)
    {
        ticksDelay = delay;
    }

    /**
     * Gets the delay in ticks from first button activation until next activation.
     *
     * @return The delay, measured in ticks, between first activation and second activation.
     *
     * @see setDelay
     */
    int getDelay()
    {
        return ticksDelay;
    }

    /**
     * Sets the interval in number of ticks between each each activation of the pressed
     * button after the second activation.
     *
     * @param  interval The interval between repeated activations, measured in ticks.
     *
     * @see setDelay, getInterval
     */
    void setInterval(int interval)
    {
        ticksInterval = interval;
    }

    /**
     * The interval between repeated activations, measured in ticks. This is the number of
     * ticks between the an activation beyond the first and the following activation.
     *
     * @return The interval between repeated activations, measured in ticks.
     *
     * @see setInterval
     */
    int getInterval()
    {
        return ticksInterval;
    }

    void handleClickEvent(const ClickEvent& event)
    {
        bool wasPressed = getPressed();
        bool newPressedValue = (event.getType() == ClickEvent::PRESSED);
        if ((newPressedValue && !wasPressed) || (!newPressedValue && wasPressed))
        {
            setPressed(newPressedValue);
            invalidate();
        }

        if (event.getType() == ClickEvent::PRESSED)
        {
            if (action && action->isValid())
            {
                action->execute(*this);
            }
            ticks = 0;
            ticksBeforeContinuous = ticksDelay;
            Application::getInstance()->registerTimerWidget(this);
        }
        else
        {
            Application::getInstance()->unregisterTimerWidget(this);
        }
    }

    void handleTickEvent()
    {
        AbstractButtonContainer::handleTickEvent();

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

private:
    int16_t ticksDelay;
    int16_t ticksInterval;

    int16_t ticks;
    int16_t ticksBeforeContinuous;
};

} // namespace touchgfx

#endif // REPEATBUTTONTRIGGER_HPP
