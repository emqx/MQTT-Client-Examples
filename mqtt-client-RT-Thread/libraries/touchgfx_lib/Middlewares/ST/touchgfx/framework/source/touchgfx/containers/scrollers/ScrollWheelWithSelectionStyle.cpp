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

#include <touchgfx/containers/scrollers/ScrollWheelWithSelectionStyle.hpp>

namespace touchgfx
{
ScrollWheelWithSelectionStyle::ScrollWheelWithSelectionStyle()
    : ScrollWheelBase(),
      drawablesInFirstList(0),
      extraSizeBeforeSelectedItem(0),
      extraSizeAfterSelectedItem(0),
      marginBeforeSelectedItem(0),
      marginAfterSelectedItem(0),
      drawables(0),
      centerDrawables(0),
      originalUpdateDrawableCallback(0),
      originalUpdateCenterDrawableCallback(0)
{
    ScrollWheelBase::add(list2);
    ScrollWheelBase::add(list1); // Put center list at top of the first/last list.
}

void ScrollWheelWithSelectionStyle::setWidth(int16_t width)
{
    ScrollWheelBase::setWidth(width);
    if (getHorizontal())
    {
        refreshDrawableListsLayout();
    }
}

void ScrollWheelWithSelectionStyle::setHeight(int16_t height)
{
    ScrollWheelBase::setHeight(height);
    if (!getHorizontal())
    {
        refreshDrawableListsLayout();
    }
}

void ScrollWheelWithSelectionStyle::setHorizontal(bool horizontal)
{
    ScrollWheelBase::setHorizontal(horizontal);
    list1.setHorizontal(horizontal);
    list2.setHorizontal(horizontal);
    refreshDrawableListsLayout();
}

void ScrollWheelWithSelectionStyle::setCircular(bool circular)
{
    ScrollWheelBase::setCircular(circular);
    list1.setCircular(circular);
    list2.setCircular(circular);
}

void ScrollWheelWithSelectionStyle::setNumberOfItems(int16_t numberOfItems)
{
    if (numberOfItems != getNumberOfItems())
    {
        ScrollWheelBase::setNumberOfItems(numberOfItems);
        list1.setNumberOfItems(numberOfItems);
        list2.setNumberOfItems(numberOfItems);
    }
}

void ScrollWheelWithSelectionStyle::setSelectedItemOffset(int16_t offset)
{
    ScrollWheelBase::setSelectedItemOffset(offset);
    refreshDrawableListsLayout();
}

void ScrollWheelWithSelectionStyle::setSelectedItemExtraSize(int16_t extraSizeBefore, int16_t extraSizeAfter)
{
    extraSizeBeforeSelectedItem = extraSizeBefore;
    extraSizeAfterSelectedItem = extraSizeAfter;
    refreshDrawableListsLayout();
}

int16_t ScrollWheelWithSelectionStyle::getSelectedItemExtraSizeBefore() const
{
    return extraSizeBeforeSelectedItem;
}

int16_t ScrollWheelWithSelectionStyle::getSelectedItemExtraSizeAfter() const
{
    return extraSizeAfterSelectedItem;
}

void ScrollWheelWithSelectionStyle::setSelectedItemMargin(int16_t marginBefore, int16_t marginAfter)
{
    marginBeforeSelectedItem = marginBefore;
    marginAfterSelectedItem = marginAfter;
    refreshDrawableListsLayout();
}

int16_t ScrollWheelWithSelectionStyle::getSelectedItemMarginBefore() const
{
    return marginBeforeSelectedItem;
}

int16_t ScrollWheelWithSelectionStyle::getSelectedItemMarginAfter() const
{
    return marginAfterSelectedItem;
}

void ScrollWheelWithSelectionStyle::setSelectedItemPosition(int16_t offset, int16_t extraSizeBefore, int16_t extraSizeAfter, int16_t marginBefore, int16_t marginAfter)
{
    setSelectedItemOffset(offset);
    setSelectedItemExtraSize(extraSizeBefore, extraSizeAfter);
    setSelectedItemMargin(marginBefore, marginAfter);
}

void ScrollWheelWithSelectionStyle::setDrawableSize(int16_t drawableSize, int16_t drawableMargin)
{
    ScrollWheelBase::setDrawableSize(drawableSize, drawableMargin);
    list1.setDrawableSize(drawableSize, drawableMargin);
    list2.setDrawableSize(drawableSize, drawableMargin);

    // Resize the three lists
    setSelectedItemOffset(distanceBeforeAlignedItem);

    // Changing the drawable size alters number of required drawables, so refresh this
    refreshDrawableListsLayout();
}

void ScrollWheelWithSelectionStyle::setDrawables(DrawableListItemsInterface& drawableListItems, GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>& updateDrawableCallback,
                                                 DrawableListItemsInterface& centerDrawableListItems, GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>& updateCenterDrawableCallback)
{
    drawables = &drawableListItems;
    centerDrawables = &centerDrawableListItems;

    currentAnimationState = NO_ANIMATION;

    originalUpdateDrawableCallback = &updateDrawableCallback;
    originalUpdateCenterDrawableCallback = &updateCenterDrawableCallback;

    refreshDrawableListsLayout();

    setOffset(0);
}

void ScrollWheelWithSelectionStyle::setOffset(int32_t offset)
{
    ScrollWheelBase::setOffset(offset);
    list1.setOffset((distanceBeforeAlignedItem - (distanceBeforeAlignedItem - extraSizeBeforeSelectedItem)) + offset);
    list2.setOffset((distanceBeforeAlignedItem - (distanceBeforeAlignedItem + itemSize + extraSizeAfterSelectedItem + marginAfterSelectedItem)) + offset);
}

void ScrollWheelWithSelectionStyle::itemChanged(int itemIndex)
{
    ScrollWheelBase::itemChanged(itemIndex);
    list1.itemChanged(itemIndex);
    list2.itemChanged(itemIndex);
}

void ScrollWheelWithSelectionStyle::refreshDrawableListsLayout()
{
    if (drawables != 0 && centerDrawables != 0)
    {
        int32_t currentOffset = getOffset();

        int16_t list1Pos = distanceBeforeAlignedItem - extraSizeBeforeSelectedItem;
        int16_t list2Pos = distanceBeforeAlignedItem + itemSize + (extraSizeAfterSelectedItem + marginAfterSelectedItem);
        int16_t list0Size = list1Pos - marginBeforeSelectedItem;
        int16_t list1Size = itemSize + extraSizeBeforeSelectedItem + extraSizeAfterSelectedItem;

        if (getHorizontal())
        {
            int16_t list2Size = getWidth() - list2Pos;
            list.setPosition(0, 0, list0Size, getHeight());
            list1.setPosition(list1Pos, 0, list1Size, getHeight());
            list2.setPosition(list2Pos, 0, list2Size, getHeight());
        }
        else
        {
            int16_t list2Size = getHeight() - list2Pos;
            list.setPosition(0, 0, getWidth(), list0Size);
            list1.setPosition(0, list1Pos, getWidth(), list1Size);
            list2.setPosition(0, list2Pos, getWidth(), list2Size);
        }

        list.setDrawables(*drawables, 0, *originalUpdateDrawableCallback);
        drawablesInFirstList = list.getNumberOfDrawables();
        list1.setDrawables(*centerDrawables, 0, *originalUpdateCenterDrawableCallback);
        list2.setDrawables(*drawables, drawablesInFirstList, *originalUpdateDrawableCallback);

        setOffset(keepOffsetInsideLimits(currentOffset, 0));
    }
}
} // namespace touchgfx
