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
 * @file touchgfx/Event.hpp
 *
 * Declares the touchgfx::Event class.
 */
#ifndef EVENT_HPP
#define EVENT_HPP

namespace touchgfx
{
/** Simple base class for events. */
class Event
{
public:
    /** The event types. */
    typedef enum
    {
        EVENT_CLICK,  ///< A click
        EVENT_DRAG,   ///< A drag
        EVENT_GESTURE ///< A gesture
    } EventType;

    /**
     * Gets event type.
     *
     * @return The type of this event.
     */
    virtual EventType getEventType() = 0;

    /** Finalizes an instance of the Event class. */
    virtual ~Event()
    {
    }
};

} // namespace touchgfx

#endif // EVENT_HPP
