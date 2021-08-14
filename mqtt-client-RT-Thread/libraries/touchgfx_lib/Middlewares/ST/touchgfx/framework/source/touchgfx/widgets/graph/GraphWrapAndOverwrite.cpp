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

#include <touchgfx/widgets/graph/GraphWrapAndOverwrite.hpp>

namespace touchgfx
{
DataGraphWrapAndOverwrite::DataGraphWrapAndOverwrite(int16_t capacity, int* values)
    : AbstractDataGraphWithY(capacity, values), current(0)
{
}

void DataGraphWrapAndOverwrite::clear()
{
    AbstractDataGraphWithY::clear();
    current = 0;
}

int32_t DataGraphWrapAndOverwrite::indexToGlobalIndex(int16_t index) const
{
    if (this->usedCapacity < this->maxCapacity)
    {
        return index;
    }
    const int16_t gapIndex = this->getGapBeforeIndex();
    if (index < gapIndex)
    {
        return (this->dataCounter - gapIndex) + index;
    }
    return ((this->dataCounter - gapIndex) - this->maxCapacity) + index;
}

void DataGraphWrapAndOverwrite::beforeAddValue()
{
    if (current == 0 && usedCapacity >= maxCapacity)
    {
        int xMin = getGraphRangeXMin();
        int xMax = getGraphRangeXMax();
        for (int i = xMin; i < 0; i++)
        {
            invalidateXAxisPointAt(i);
        }
        for (int i = maxCapacity; i <= xMax; i++)
        {
            invalidateXAxisPointAt(i);
        }
    }
    if (usedCapacity >= maxCapacity)
    {
        invalidateGraphPointAt(current);
        invalidateXAxisPointAt(current);
    }
}

int16_t DataGraphWrapAndOverwrite::addValue(int value)
{
    const int16_t index = current++;
    current %= maxCapacity;
    if (index == usedCapacity)
    {
        usedCapacity++;
    }
    yValues[realIndex(index)] = value;
    setGapBeforeIndex(index + 1);
    invalidateGraphPointAt(index);
    if (usedCapacity >= maxCapacity)
    {
        invalidateXAxisPointAt(index);
    }
    if (index == 0 && usedCapacity >= maxCapacity)
    {
        int xMin = getGraphRangeXMin();
        int xMax = getGraphRangeXMax();
        for (int i = xMin; i < 0; i++)
        {
            invalidateXAxisPointAt(i);
        }
        for (int i = maxCapacity; i <= xMax; i++)
        {
            invalidateXAxisPointAt(i);
        }
    }
    return index;
}

} // namespace touchgfx
