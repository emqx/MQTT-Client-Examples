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
 * @file touchgfx/mixins/Draggable.hpp
 *
 * Declares the touchgfx::Draggable class.
 */
#ifndef DRAGGABLE_HPP
#define DRAGGABLE_HPP

#include <touchgfx/Callback.hpp>

namespace touchgfx
{
/**
 * Mix-in class that extends a class to become Draggable, which means that the object on screen
 * can be freely moved around using the touch screen.
 *
 * @tparam T specifies the type to extend with the Draggable behavior.
 */
template <class T>
class Draggable : public T
{
public:
    /** Initializes a new instance of the Draggable class. Make the object touchable. */
    Draggable()
        : T()
    {
        T::setTouchable(true);
    }

    /**
     * Called when dragging the Draggable object. The object is moved according to the drag
     * event.
     *
     * @param  evt The drag event.
     */
    virtual void handleDragEvent(const DragEvent& evt)
    {
        T::handleDragEvent(evt);
        T::moveRelative(evt.getDeltaX(), evt.getDeltaY());
    }
};
} //namespace touchgfx

#endif // DRAGGABLE_HPP
