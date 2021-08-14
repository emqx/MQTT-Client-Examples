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
 * @file touchgfx/containers/scrollers/ScrollWheel.hpp
 *
 * Declares the touchgfx::ScrollWheel class.
 */
#ifndef SCROLLWHEEL_HPP
#define SCROLLWHEEL_HPP

#include <touchgfx/containers/scrollers/DrawableList.hpp>
#include <touchgfx/containers/scrollers/ScrollWheelBase.hpp>

namespace touchgfx
{
/**
 * A scroll wheel is very much like the digit selector on a padlock with numbers. The digits
 * always snap into place and exactly one number is always the "selected" number. Thus,
 * a scroll wheel is a list of identically styled drawables which can be scrolled
 * through. One of the items in the list is the "selected" one, and scrolling through
 * the list can be done in various ways. The ScrollWheel uses the DrawableList to make
 * it possible to handle a huge number of items using only a limited number of drawables
 * by reusing drawables that are no longer in view.
 *
 * @see ScrollWheelBase, DrawableList, ScrollWheelWithSelectionStyle
 */
class ScrollWheel : public ScrollWheelBase
{
public:
    /**
     * Sets the drawables used by the scroll wheel. The drawables are updated through a
     * callback will put the right data in the drawable.
     *
     * @param [in] drawableListItems      Number of drawables.
     * @param [in] updateDrawableCallback The update drawable callback.
     */
    virtual void setDrawables(DrawableListItemsInterface& drawableListItems, GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>& updateDrawableCallback);
};

} // namespace touchgfx

#endif // SCROLLWHEEL_HPP
