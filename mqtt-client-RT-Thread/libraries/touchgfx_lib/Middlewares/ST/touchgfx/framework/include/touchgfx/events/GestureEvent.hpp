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
 * @file touchgfx/events/GestureEvent.hpp
 *
 * Declares the touchgfx::GestureEvent class.
 */
#ifndef GESTUREEVENT_HPP
#define GESTUREEVENT_HPP

#include <touchgfx/Event.hpp>
#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/**
 * A gesture event. The only gesture events currently supported is #SWIPE_HORIZONTAL and
 * #SWIPE_VERTICAL, which will be issued every time the input system detects a swipe.
 *
 * @see Event
 */
class GestureEvent : public Event
{
public:
    /** Values that represent gesture event types. */
    typedef enum
    {
        SWIPE_HORIZONTAL, ///< An enum constant representing a horizontal swipe
        SWIPE_VERTICAL    ///< An enum constant representing a vertical swipe
    } GestureEventType;

    ///@cond
    /**
     * Values that represent gesture event types.
     *
     * @deprecated Use GestureEvent::GestureEventType.
     */
    TOUCHGFX_DEPRECATED(
        "Use GestureEvent::GestureEventType.",
        typedef GestureEventType GestureType);
    ///@endcond

    /**
     * Constructor. Create a gesture event of the specified type with the specified
     * coordinates.
     *
     * @param  t       The type of the gesture event.
     * @param  v       The velocity of this gesture (swipe)
     * @param  x_coord The x coordinate of the gesture.
     * @param  y_coord The y coordinate of the gesture.
     */
    GestureEvent(GestureEventType t, int16_t v, int16_t x_coord, int16_t y_coord)
        : type(t),
          velocity(v),
          x(x_coord),
          y(y_coord)
    {
    }

    /**
     * Gets the velocity of this gesture event.
     *
     * @return The velocity of this gesture event.
     */
    int16_t getVelocity() const
    {
        return velocity;
    }

    /**
     * Gets the type of this gesture event.
     *
     * @return The type of this gesture event.
     */
    GestureEventType getType() const
    {
        return type;
    }

    /**
     * Gets the x coordinate of this gesture event.
     *
     * @return The x coordinate of this gesture event.
     */
    int16_t getX() const
    {
        return x;
    }

    /**
     * Gets the y coordinate of this gesture event.
     *
     * @return The y coordinate of this gesture event.
     */
    int16_t getY() const
    {
        return y;
    }

    /**
     * Gets event type.
     *
     * @return The type of this event.
     */
    virtual Event::EventType getEventType()
    {
        return Event::EVENT_GESTURE;
    }

private:
    /** Initializes a new instance of the GestureEvent class. */
    GestureEvent()
    {
    }

    GestureEventType type;
    int16_t velocity;
    int16_t x;
    int16_t y;
};

} // namespace touchgfx

#endif // GESTUREEVENT_HPP
