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
 * @file touchgfx/widgets/Widget.hpp
 *
 * Declares the touchgfx::Widget class.
 */
#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <assert.h>
#include <touchgfx/Drawable.hpp>

namespace touchgfx
{
/**
 * A Widget is an element which can be displayed (drawn) in the framebuffer. Hence a Widget is a
 * subclass of Drawable. It implements getLastChild(), but leaves the implementation of
 * draw() and getSolidRect() to subclasses of Widget, so it is still an abstract class.
 *
 * If a Widget contains more than one logical element, consider implementing several
 * subclasses of Widget and create a Container with the Widgets.
 *
 * @see Drawable
 */
class Widget : public Drawable
{
public:
    /**
     * Since a Widget is only one Drawable, Widget::getLastChild simply yields itself as
     * result, but only if the Widget isVisible and isTouchable.
     *
     * @param       x    Not used since this Widget is the only "child".
     * @param       y    Not used since this Widget is the only "child".
     * @param [out] last Result, the address of the actual instance of the Widget.
     */
    virtual void getLastChild(int16_t x, int16_t y, Drawable** last)
    {
        if (isVisible() && isTouchable())
        {
            *last = this;
        }
    }
};

} // namespace touchgfx

#endif // WIDGET_HPP
