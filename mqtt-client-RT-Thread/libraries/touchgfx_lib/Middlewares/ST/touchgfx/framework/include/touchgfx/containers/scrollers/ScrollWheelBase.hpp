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
 * @file touchgfx/containers/scrollers/ScrollWheelBase.hpp
 *
 * Declares the touchgfx::ScrollWheelBase class.
 */
#ifndef SCROLLWHEELBASE_HPP
#define SCROLLWHEELBASE_HPP

#include <touchgfx/Callback.hpp>
#include <touchgfx/containers/scrollers/ScrollBase.hpp>

namespace touchgfx
{
/**
 * A base class for a scroll wheel. A scroll wheel is very much like the digit selector on a
 * padlock with numbers. The digits always snap into place and exactly one number is
 * always the "selected" number. Using ScrollWheel, all elements look the same, but an
 * underlying bitmap can help emphasize the "selected" element. The
 * ScrollWheelWithSelectionStyle can have a completely different style on the "selected"
 * item - the font can be larger or bold and images can change color - this can help to
 * give a kind of 3D effect using very few resources.
 *
 * @see ScrollWheel, ScrollWheelWithSelectionStyle
 */
class ScrollWheelBase : public ScrollBase
{
public:
    ScrollWheelBase();

    /**
     * Sets selected item offset, measured in pixels, from the edge of the widget. The
     * offset is the relative x coordinate if the ScrollWheel is horizontal, otherwise it is
     * the relative y coordinate. If this value is zero, the selected item is placed at the
     * very start of the widget.
     *
     * @param  offset The offset.
     *
     * @see getSelectedItemOffset
     */
    virtual void setSelectedItemOffset(int16_t offset);

    /**
     * Gets offset of selected item measured in pixels relative to the start of the widget.
     *
     * @return The selected item offset.
     *
     * @see setSelectedItemOffset
     */
    virtual int16_t getSelectedItemOffset() const;

    /**
     * Gets selected item. If an animation is in progress, the item that is being scrolled
     * to is returned, not the item that happens to be flying by at the time.
     *
     * @return The selected item.
     */
    int getSelectedItem() const;

    virtual int32_t keepOffsetInsideLimits(int32_t newOffset, int16_t overShoot) const;

    virtual void handleClickEvent(const ClickEvent& evt);

    virtual void handleDragEvent(const DragEvent& evt);

    virtual void handleGestureEvent(const GestureEvent& evt);

    /**
     * Sets Callback which will be called when the ScrollWheel animates to a new item.
     *
     * @param [in] callback The callback.
     */
    void setAnimateToCallback(GenericCallback<int16_t>& callback);

protected:
    virtual int32_t getPositionForItem(int16_t itemIndex);

    virtual void animateToPosition(int32_t position, int16_t steps = -1);

    GenericCallback<int16_t>* animateToCallback; ///< The animate to callback
};

} // namespace touchgfx

#endif // SCROLLWHEELBASE_HPP
