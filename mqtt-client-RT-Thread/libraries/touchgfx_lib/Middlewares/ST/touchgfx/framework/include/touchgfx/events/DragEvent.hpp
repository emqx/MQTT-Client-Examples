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
 * @file touchgfx/events/DragEvent.hpp
 *
 * Declares the touchgfx::DragEvent class.
 */
#ifndef DRAGEVENT_HPP
#define DRAGEVENT_HPP

#include <touchgfx/Event.hpp>
#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/**
 * A drag event. The only drag event currently supported is DRAGGED, which will be issued every
 * time the input system detects a drag.
 *
 * @see Event
 */
class DragEvent : public Event
{
public:
    /** Values that represent drag event types. */
    typedef enum
    {
        DRAGGED ///< An enum constant representing the dragged option
    } DragEventType;

    /**
     * Initializes a new instance of the DragEvent class.
     *
     * @param  type The type of the drag event.
     * @param  oldX The x coordinate of the drag start position (dragged from)
     * @param  oldY The y coordinate of the drag start position (dragged from)
     * @param  newX The x coordinate of the new position (dragged to)
     * @param  newY The y coordinate of the new position (dragged to)
     */
    DragEvent(DragEventType type, int16_t oldX, int16_t oldY, int16_t newX, int16_t newY)
        : _type(type), _oldX(oldX), _oldY(oldY), _newX(newX), _newY(newY)
    {
    }

    /**
     * Gets the x coordinate where the drag operation was started (dragged from).
     *
     * @return The x coordinate where the drag operation was started (dragged from).
     */
    int16_t getOldX() const
    {
        return _oldX;
    }

    /**
     * Gets the y coordinate where the drag operation was started (dragged from).
     *
     * @return The y coordinate where the drag operation was started (dragged from).
     */
    int16_t getOldY() const
    {
        return _oldY;
    }

    /**
     * Gets the new x coordinate (dragged to).
     *
     * @return The new x coordinate (dragged to).
     */
    int16_t getNewX() const
    {
        return _newX;
    }

    /**
     * Gets the new x coordinate (dragged to).
     *
     * @return The new y coordinate (dragged to).
     */
    int16_t getNewY() const
    {
        return _newY;
    }

    /**
     * Gets the type of this drag event.
     *
     * @return The type of this drag event.
     */
    DragEventType getType() const
    {
        return _type;
    }

    /**
     * Gets the distance in x coordinates (how long was the drag).
     *
     * @return The distance of this drag event.
     */
    int16_t getDeltaX() const
    {
        return _newX - _oldX;
    }

    /**
     * Gets the distance in y coordinates (how long was the drag).
     *
     * @return The distance of this drag event.
     */
    int16_t getDeltaY() const
    {
        return _newY - _oldY;
    }

    /**
     * Gets event type.
     *
     * @return The type of this event.
     */
    virtual Event::EventType getEventType()
    {
        return Event::EVENT_DRAG;
    }

private:
    DragEventType _type;
    int16_t _oldX;
    int16_t _oldY;
    int16_t _newX;
    int16_t _newY;
};

} // namespace touchgfx

#endif // DRAGEVENT_HPP
