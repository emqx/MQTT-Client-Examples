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

#include <touchgfx/containers/scrollers/ScrollWheelBase.hpp>

namespace touchgfx
{
ScrollWheelBase::ScrollWheelBase()
    : ScrollBase(),
      animateToCallback(0)
{
    ScrollWheelBase::setHorizontal(false);
    setTouchable(true);
}

void ScrollWheelBase::setSelectedItemOffset(int16_t offset)
{
    int32_t currentOffset = getOffset();
    distanceBeforeAlignedItem = offset;
    setOffset(currentOffset);
}

int16_t ScrollWheelBase::getSelectedItemOffset() const
{
    return distanceBeforeAlignedItem;
}

int32_t ScrollWheelBase::getPositionForItem(int16_t itemIndex)
{
    int32_t newOffset = -itemIndex * itemSize;
    if (getCircular())
    {
        // Check if it is closer to scroll backwards
        int32_t otherOffset = newOffset + getNumberOfItems() * itemSize;
        int32_t offset = getOffset();
        if (abs(otherOffset - offset) < abs(newOffset - offset))
        {
            newOffset = otherOffset;
        }
    }
    return newOffset;
}

void ScrollWheelBase::animateToPosition(int32_t position, int16_t steps)
{
    if (itemSize == 0)
    {
        return;
    }
    if (animateToCallback && animateToCallback->isValid() && itemSize > 0)
    {
        position = getNearestAlignedOffset(position);
        int16_t itemIndex = (-position) / itemSize;
        animateToCallback->execute(itemIndex);
    }
    ScrollBase::animateToPosition(position, steps);
}

int ScrollWheelBase::getSelectedItem() const
{
    if (itemSize == 0)
    {
        return 0;
    }
    if (currentAnimationState == ANIMATING_GESTURE)
    {
        // Scroll in progress, get the destination value
        return (-getNormalizedOffset(gestureEnd)) / itemSize;
    }
    return (-getNormalizedOffset(getOffset())) / itemSize;
}

int32_t ScrollWheelBase::keepOffsetInsideLimits(int32_t newOffset, int16_t overShoot) const
{
    if (!getCircular())
    {
        newOffset = MIN(newOffset, overShoot);
        int16_t numberOfItems = getNumberOfItems();
        newOffset = MAX(newOffset, -(itemSize * (numberOfItems - 1)) - overShoot);
    }
    return newOffset;
}

void ScrollWheelBase::handleClickEvent(const ClickEvent& evt)
{
    if (itemSize == 0)
    {
        return;
    }
    int32_t offset = getOffset();
    if (evt.getType() == ClickEvent::PRESSED)
    {
        xClick = evt.getX();
        yClick = evt.getY();
        initialSwipeOffset = offset;

        if (itemPressedCallback && itemPressedCallback->isValid())
        {
            itemPressedCallback->execute(getSelectedItem());
        }
    }
    else if (evt.getType() == ClickEvent::RELEASED)
    {
        if (currentAnimationState == NO_ANIMATION)
        {
            int16_t click = getHorizontal() ? xClick : yClick;
            // Click => move to clicked position
            if (click < distanceBeforeAlignedItem)
            {
                animateToPosition(offset + ((distanceBeforeAlignedItem - click) / itemSize + 1) * itemSize);
            }
            else if (click > distanceBeforeAlignedItem + itemSize)
            {
                animateToPosition(offset - ((click - distanceBeforeAlignedItem) / itemSize) * itemSize);
            }
            else
            {
                animateToPosition(offset);
            }
        }
        else if (currentAnimationState == ANIMATING_DRAG)
        {
            // click + drag + release. Find best Y to scroll to
            animateToPosition(offset);
        }

        if (itemSelectedCallback && itemSelectedCallback->isValid())
        {
            itemSelectedCallback->execute(getSelectedItem());
        }
    }
}

void ScrollWheelBase::handleDragEvent(const DragEvent& evt)
{
    currentAnimationState = ANIMATING_DRAG;
    int newOffset = getOffset() + (getHorizontal() ? evt.getDeltaX() : evt.getDeltaY()) * dragAcceleration / 10;
    if (!getCircular())
    {
        newOffset = MIN(newOffset, itemSize * 3 / 4);
        int16_t numberOfItems = getNumberOfItems();
        newOffset = MAX(newOffset, -(itemSize * (numberOfItems - 1)) - itemSize * 3 / 4);
    }
    setOffset(newOffset);
}

void ScrollWheelBase::handleGestureEvent(const GestureEvent& evt)
{
    if (evt.getType() == (getHorizontal() ? GestureEvent::SWIPE_HORIZONTAL : GestureEvent::SWIPE_VERTICAL))
    {
        int32_t newOffset = getOffset() + evt.getVelocity() * swipeAcceleration / 10;
        if (maxSwipeItems > 0)
        {
            int32_t maxDistance = maxSwipeItems * itemSize;
            newOffset = MIN(newOffset, initialSwipeOffset + maxDistance);
            newOffset = MAX(newOffset, initialSwipeOffset - maxDistance);
        }
        animateToPosition(newOffset);
    }
}

void ScrollWheelBase::setAnimateToCallback(GenericCallback<int16_t>& callback)
{
    animateToCallback = &callback;
}
} // namespace touchgfx
