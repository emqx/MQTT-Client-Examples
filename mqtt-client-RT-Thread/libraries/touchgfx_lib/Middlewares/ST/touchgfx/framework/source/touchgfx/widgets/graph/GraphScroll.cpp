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

#include <touchgfx/widgets/graph/GraphScroll.hpp>

namespace touchgfx
{
DataGraphScroll::DataGraphScroll(int16_t capacity, int* values)
    : AbstractDataGraphWithY(capacity, values), current(0)
{
}

void DataGraphScroll::clear()
{
    AbstractDataGraphWithY::clear();
    current = 0;
}

int32_t DataGraphScroll::indexToGlobalIndex(int16_t index) const
{
    if (usedCapacity < maxCapacity)
    {
        return realIndex(index);
    }
    return (dataCounter - maxCapacity) + index;
}

void DataGraphScroll::beforeAddValue()
{
    if (usedCapacity == maxCapacity)
    {
        invalidateAllXAxisPoints();
    }
}

int16_t DataGraphScroll::addValue(int value)
{
    const bool graphFull = usedCapacity == maxCapacity;
    const int16_t index = current++;
    current %= maxCapacity;
    if (index == usedCapacity)
    {
        usedCapacity++;
    }
    yValues[index] = value;
    if (graphFull)
    {
        invalidateGraphArea();
        invalidateAllXAxisPoints();
    }
    else
    {
        invalidateGraphPointAt(index);
    }
    return index;
}

int16_t DataGraphScroll::realIndex(int16_t index) const
{
    return usedCapacity < maxCapacity ? index : (index + current) % maxCapacity;
}

CWRUtil::Q5 DataGraphScroll::indexToXQ5(int16_t index) const
{
    return CWRUtil::toQ5(index);
}

} // namespace touchgfx
