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

#include <touchgfx/containers/scrollers/DrawableList.hpp>

namespace touchgfx
{
DrawableList::DrawableList()
    : Container(),
      isHorizontal(false),
      isCircular(false),
      offset(0),
      itemSize(0),
      itemMargin(0),
      numItems(0),
      numDrawables(0),
      firstItem(0),
      firstDrawable(0),
      drawablesInitialized(false),
      firstDrawableIndex(0),
      drawableItems(0),
      updateDrawable(0)
{
}

void DrawableList::setWidth(int16_t width)
{
    Container::setWidth(width);
    refreshDrawables();
}

void DrawableList::setHeight(int16_t height)
{
    Container::setHeight(height);
    refreshDrawables();
}

void DrawableList::setHorizontal(bool horizontal)
{
    if ((horizontal && !isHorizontal) || (!horizontal && isHorizontal))
    {
        isHorizontal = horizontal;
        refreshDrawables();
    }
}

bool DrawableList::getHorizontal() const
{
    return isHorizontal;
}

void DrawableList::setCircular(bool circular)
{
    if ((circular && !isCircular) || (!circular && isCircular))
    {
        isCircular = circular;
        refreshDrawables();
    }
}

bool DrawableList::getCircular() const
{
    return isCircular;
}

void DrawableList::setDrawableSize(int16_t drawableSize, int16_t drawableMargin)
{
    itemSize = drawableSize + 2 * drawableMargin;
    itemMargin = drawableMargin;
}

void DrawableList::setDrawables(DrawableListItemsInterface& drawableListItems,
                                int16_t drawableItemIndexOffset,
                                GenericCallback<DrawableListItemsInterface*, int16_t, int16_t>& updateDrawableCallback)
{
    drawableItems = &drawableListItems;
    firstDrawableIndex = drawableItemIndexOffset;
    updateDrawable = &updateDrawableCallback;

    refreshDrawables();
}

int16_t DrawableList::getNumberOfDrawables() const
{
    return numDrawables;
}

int16_t DrawableList::getItemSize() const
{
    return itemSize;
}

int16_t DrawableList::getDrawableSize() const
{
    return itemSize - 2 * itemMargin;
}

int16_t DrawableList::getDrawableMargin() const
{
    return itemMargin;
}

void DrawableList::setNumberOfItems(int16_t numberOfItems)
{
    numItems = numberOfItems;
    refreshDrawables();
}

int16_t DrawableList::getNumberOfItems() const
{
    return numItems;
}

int16_t DrawableList::getRequiredNumberOfDrawables() const
{
    if (drawableItems == 0 || itemSize <= 0)
    {
        return 0;
    }

    // Calculate number of required drawables. Worst case is one pixel visible of drawable at top and rest stacked tightly
    int16_t requiredDrawables = 1 + (((isHorizontal ? getWidth() : getHeight()) - 1) + (itemSize - 1)) / itemSize;
    if (!isCircular)
    {
        // We never require more drawables than the number of elements on non-circular list.
        if (requiredDrawables > numItems)
        {
            requiredDrawables = numItems;
        }
    }

    int16_t numberOfDrawables = drawableItems->getNumberOfDrawables();
    return MIN((numberOfDrawables - firstDrawableIndex), requiredDrawables);
}

void DrawableList::setOffset(int32_t ofs)
{
    offset = ofs;

    if (numDrawables == 0 || numItems == 0 || itemSize == 0)
    {
        return;
    }
    if (!updateDrawable || !updateDrawable->isValid())
    {
        return;
    }

    // ofs is the offset of item[0]
    // 0 => item[0] is perfectly selected, -itemSize => item[1] is perfectly selected, itemSize => item[N-1] is perfectly selected etc.
    int16_t newFirstItem = 0;
    if (ofs > 0)
    {
        int numberOfItems = ofs / itemSize + 1;
        newFirstItem -= numberOfItems;
        ofs -= numberOfItems * itemSize;
    }
    if (ofs <= -itemSize)
    {
        int numberOfItems = ofs / itemSize;
        newFirstItem -= numberOfItems;
        ofs -= numberOfItems * itemSize;
    }
    if (isCircular)
    {
        // Make sure that firstIndex is "in range"
        newFirstItem %= numItems;
        newFirstItem = (newFirstItem + numItems) % numItems;
    }
    else
    {
        if (newFirstItem < 0)
        {
            ofs -= newFirstItem * itemSize;
            newFirstItem = 0;
        }
        else if (newFirstItem + numDrawables > numItems)
        {
            int x = numItems - (newFirstItem + numDrawables);
            ofs += x * itemSize;
            newFirstItem += x;
        }
    }

    int drawableDelta = 0;
    if (drawablesInitialized && firstItem != newFirstItem)
    {
        drawableDelta = numDrawables;
        for (int i = 1; i < numDrawables; i++)
        {
            int fi = (firstItem + i);
            int nfi = (newFirstItem + i);
            if (isCircular)
            {
                fi %= numItems;
                nfi %= numItems;
            }
            if (fi == newFirstItem)
            {
                drawableDelta = -i;
                break;
            }
            if (nfi == firstItem)
            {
                drawableDelta = i;
                break;
            }
        }
    }
    firstDrawable = ((firstDrawable - drawableDelta) + numDrawables) % numDrawables;
    firstItem = newFirstItem;

    for (int i = 0; i < numDrawables; i++)
    {
        int drawableIndex = (firstDrawable + i) % numDrawables;
        Drawable* drawable = drawableItems->getDrawable(drawableIndex + firstDrawableIndex);
        if (isHorizontal)
        {
            drawable->moveTo(ofs + i * itemSize + itemMargin, 0);
        }
        else
        {
            drawable->moveTo(0, ofs + i * itemSize + itemMargin);
        }

        int itemIndex = i + firstItem;
        if (isCircular)
        {
            itemIndex %= numItems;
        }
        else
        {
            if (itemIndex < 0 || itemIndex >= numItems)
            {
                itemIndex = -1;
            }
        }
        if (itemIndex < 0)
        {
            drawable->setVisible(false);
        }
        else
        {
            drawable->setVisible(true);
            // Only fill if first time or outside old range
            if (!drawablesInitialized || (i < drawableDelta || i >= numDrawables + drawableDelta))
            {
                if (updateDrawable->isValid())
                {
                    updateDrawable->execute(drawableItems, drawableIndex + firstDrawableIndex, itemIndex);
                    drawable->invalidate();
                }
            }
        }
    }
    drawablesInitialized = true;
}

int32_t DrawableList::getOffset() const
{
    return offset;
}

int16_t DrawableList::getItemIndex(int16_t drawableIndex) const
{
    if (drawableIndex < 0 || drawableIndex >= numDrawables || numDrawables == 0 || numItems == 0)
    {
        return -1;
    }
    int16_t itemNumber = ((drawableIndex + numDrawables - firstDrawable) % numDrawables) + firstItem;
    if (isCircular)
    {
        itemNumber %= numItems;
    }
    if (itemNumber < 0 || itemNumber >= numItems)
    {
        return -1;
    }
    return itemNumber;
}

int16_t DrawableList::getDrawableIndices(int16_t itemIndex, int16_t* drawableIndexArray, int16_t arraySize) const
{
    int16_t numFound = 0;
    int16_t drawableIndex = -1;
    while ((drawableIndex = getDrawableIndex(itemIndex, drawableIndex)) >= 0)
    {
        if (numFound < arraySize)
        {
            drawableIndexArray[numFound] = drawableIndex;
            numFound++;
        }
    }
    return numFound;
}

int16_t DrawableList::getDrawableIndex(int16_t itemIndex, int16_t prevDrawableIndex /*= -1*/) const
{
    if (prevDrawableIndex < -1 || prevDrawableIndex >= numDrawables || numDrawables == 0 || numItems == 0)
    {
        return -1;
    }
    if (prevDrawableIndex >= 0)
    {
        prevDrawableIndex = (prevDrawableIndex - firstDrawable + numDrawables) % numDrawables;
    }
    for (int16_t i = prevDrawableIndex + 1; i < numDrawables; i++)
    {
        int16_t currentItemIndex = firstItem + i;
        if (isCircular)
        {
            currentItemIndex %= numItems;
        }
        if (itemIndex == currentItemIndex)
        {
            int16_t drawableIndex = (firstDrawable + i) % numDrawables;
            return drawableIndex;
        }
    }
    return -1;
}

void DrawableList::refreshDrawables()
{
    numDrawables = 0;
    if (drawableItems == 0)
    {
        return;
    }
    numDrawables = getRequiredNumberOfDrawables();
    // Remove everything
    Container::removeAll();
    // Add the itemDrawables
    for (int drawableIndex = 0; drawableIndex < numDrawables; drawableIndex++)
    {
        Drawable* drawable = drawableItems->getDrawable(drawableIndex + firstDrawableIndex);
        // Resize the drawables, X/Y ignored for now.
        if (isHorizontal)
        {
            drawable->setPosition(0, 0, itemSize - 2 * itemMargin, getHeight());
        }
        else
        {
            drawable->setPosition(0, 0, getWidth(), itemSize - 2 * itemMargin);
        }
        // Add each drawable for later positioning
        if (drawable->getParent() != 0)
        {
            // Remove drawable from the current parent
            Container* parent = static_cast<Container*>(drawable->getParent());
            parent->remove(*drawable);
        }
        Container::add(*drawable);
    }

    drawablesInitialized = false;
    firstItem = 0;
    firstDrawable = 0;
    setOffset(offset);
}

void DrawableList::itemChanged(int16_t itemIndex)
{
    if (updateDrawable && updateDrawable->isValid())
    {
        int16_t drawableIndex = -1;
        while ((drawableIndex = getDrawableIndex(itemIndex, drawableIndex)) != -1)
        {
            updateDrawable->execute(drawableItems, drawableIndex + firstDrawableIndex, itemIndex);
        }
    }
}
} // namespace touchgfx
