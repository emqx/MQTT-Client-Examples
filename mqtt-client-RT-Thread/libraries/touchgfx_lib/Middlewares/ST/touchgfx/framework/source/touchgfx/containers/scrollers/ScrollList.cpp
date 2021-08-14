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

#include <touchgfx/containers/scrollers/ScrollList.hpp>

namespace touchgfx
{
ScrollList::ScrollList()
    : ScrollBase(),
      paddingAfterLastItem(0),
      snapping(false),
      windowSize(1)
{
}

void ScrollList::setDrawables(DrawableListItemsInterface& drawableListItems, GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>& updateDrawableCallback)
{
    stopAnimation();
    numberOfDrawables = drawableListItems.getNumberOfDrawables();
    list.setDrawables(drawableListItems, 0, updateDrawableCallback);
    setOffset(0);
}

void ScrollList::setWindowSize(int16_t items)
{
    windowSize = MAX(1, items);
    animateToPosition(keepOffsetInsideLimits(getOffset(), 0));
}

void ScrollList::setPadding(int16_t paddingBefore, int16_t paddingAfter)
{
    int32_t currentOffset = getOffset();
    distanceBeforeAlignedItem = paddingBefore;
    paddingAfterLastItem = paddingAfter;
    setOffset(currentOffset);
    list.refreshDrawables();
}

int16_t ScrollList::getPaddingBefore() const
{
    return distanceBeforeAlignedItem;
}

int16_t ScrollList::getPaddingAfter() const
{
    return paddingAfterLastItem;
}

void ScrollList::setSnapping(bool snap)
{
    snapping = snap;
    if (snapping)
    {
        setOffset(getNearestAlignedOffset(getOffset()));
    }
}

bool ScrollList::getSnapping() const
{
    return snapping;
}

int32_t ScrollList::getPositionForItem(int16_t itemIndex)
{
    int32_t currentOffset = getNormalizedOffset(getOffset());
    if (itemIndex < 0 || itemIndex >= list.getNumberOfItems() || itemSize == 0)
    {
        return currentOffset;
    }
    int32_t itemOffset = -itemIndex * itemSize;
    // Get the visible size
    int16_t widgetSize = getHorizontal() ? getWidth() : getHeight();
    int16_t activeWidgetSize = widgetSize - (distanceBeforeAlignedItem + paddingAfterLastItem);
    if (list.getCircular())
    {
        int32_t offset = currentOffset;
        // Important this is a do-while of visibleSize < itemSize in which case we need to check at least one time
        do
        {
            int16_t i = (-getNormalizedOffset(offset)) / itemSize; // Item index of first
            if (itemIndex == i)
            {
                return currentOffset;
            }
            offset -= itemSize;
        } while (offset >= currentOffset - (activeWidgetSize - itemSize));
        int32_t allItemsSize = list.getNumberOfItems() * itemSize;
        // Either scroll left from the first item or right from the last item. Find out which is closest
        int32_t leftScrollDistance = itemOffset - currentOffset;
        int32_t leftScrollDistance2 = (itemOffset + allItemsSize) - currentOffset;
        int32_t rightItemOffset = getNormalizedOffset(currentOffset - (activeWidgetSize - itemSize));
        int32_t rightScrollDistance = rightItemOffset - itemOffset;
        int32_t rightScrollDistance2 = rightItemOffset - (itemOffset - allItemsSize);
        if (abs(leftScrollDistance2) < abs(leftScrollDistance))
        {
            leftScrollDistance = leftScrollDistance2;
        }
        if (abs(rightScrollDistance2) < abs(rightScrollDistance))
        {
            rightScrollDistance = rightScrollDistance2;
        }
        if (abs(rightScrollDistance) < abs(leftScrollDistance))
        {
            return currentOffset - rightScrollDistance;
        }
        return currentOffset + leftScrollDistance;
    }
    else
    {
        if (itemOffset > currentOffset)                      // First item on screen is higher than the itemIndex. Scroll itemIndex to top position
        {
            return itemOffset;
        }
        int16_t numberOfVisibleItems = activeWidgetSize / itemSize;
        int32_t itemOffsetAtEnd = itemOffset;
        if (numberOfVisibleItems > 0)
        {
            if (snapping)
            {
                itemOffsetAtEnd = itemOffset + (numberOfVisibleItems - 1) * itemSize;
            }
            else
            {
                itemOffsetAtEnd = itemOffset + activeWidgetSize - itemSize;
            }
        }
        if (itemOffsetAtEnd < currentOffset)
        {
            return itemOffsetAtEnd;
        }
    }
    return currentOffset;
}

void ScrollList::handleClickEvent(const ClickEvent& evt)
{
    ScrollBase::handleClickEvent(evt);
    if (evt.getType() == ClickEvent::PRESSED)
    {
        xClick = evt.getX();
        yClick = evt.getY();
        initialSwipeOffset = getOffset();

        setOffset(getNearestAlignedOffset(initialSwipeOffset));
        if (itemPressedCallback && itemPressedCallback->isValid())
        {
            int16_t click = (getHorizontal() ? xClick : yClick);
            int32_t offset = click - getOffset();
            int32_t listSize = getNumberOfItems() * itemSize;
            if (getCircular())
            {
                offset += listSize;
                offset %= listSize;
            }
            if (offset >= 0 && offset < listSize)
            {
                int16_t item = offset / itemSize;
                itemPressedCallback->execute(item);
            }
        }
    }
    else if (evt.getType() == ClickEvent::RELEASED)
    {
        if (currentAnimationState == NO_ANIMATION)
        {
            // For a tiny drag, start by re-aligning (no animation(!))
            setOffset(getNearestAlignedOffset(getOffset()));
            if (itemSelectedCallback && itemSelectedCallback->isValid())
            {
                int16_t click = (getHorizontal() ? xClick : yClick);
                int32_t offset = click - getOffset();
                int32_t listSize = getNumberOfItems() * itemSize;
                if (getCircular())
                {
                    offset += listSize;
                    offset %= listSize;
                }
                else
                {
                    offset -= distanceBeforeAlignedItem;
                }
                if (offset >= 0 && offset < listSize)
                {
                    int16_t item = offset / itemSize;
                    itemSelectedCallback->execute(item);
                }
            }
        }
        else if (currentAnimationState == ANIMATING_DRAG)
        {
            // click + drag + release. Find best Y to scroll to
            animateToPosition(getNearestAlignedOffset(getOffset()));
        }
    }
}

int32_t ScrollList::getNearestAlignedOffset(int32_t offset) const
{
    if (snapping)
    {
        // ScrollBase implementation will snap
        return ScrollBase::getNearestAlignedOffset(offset);
    }

    return keepOffsetInsideLimits(offset, 0);
}

int32_t ScrollList::keepOffsetInsideLimits(int32_t newOffset, int16_t overShoot) const
{
    if (!getCircular())
    {
        newOffset = MIN(newOffset, overShoot);
        int maxOffToTheStart = windowSize < getNumberOfItems() ? getNumberOfItems() - windowSize : 0;
        newOffset = MAX(newOffset, -(itemSize * maxOffToTheStart) - overShoot);
    }
    return newOffset;
}
} // namespace touchgfx
