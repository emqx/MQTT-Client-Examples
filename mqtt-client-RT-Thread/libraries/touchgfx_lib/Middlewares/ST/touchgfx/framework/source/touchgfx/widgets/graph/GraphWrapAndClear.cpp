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

#include <touchgfx/widgets/graph/GraphWrapAndClear.hpp>

namespace touchgfx
{
DataGraphWrapAndClear::DataGraphWrapAndClear(int16_t capacity, int* values)
    : AbstractDataGraphWithY(capacity, values)
{
}

int32_t DataGraphWrapAndClear::indexToGlobalIndex(int16_t index) const
{
    return (this->dataCounter - this->usedCapacity) + index;
}

void DataGraphWrapAndClear::beforeAddValue()
{
    if (usedCapacity >= maxCapacity)
    {
        invalidateAllXAxisPoints();
        clear();
        invalidateGraphArea();
    }
}

int16_t DataGraphWrapAndClear::addValue(int value)
{
    const bool clearGraph = (usedCapacity == 0);
    const int16_t index = usedCapacity;
    usedCapacity++;
    yValues[realIndex(index)] = value;
    if (clearGraph)
    {
        // Label sizes might have grown, also invalidate new sizes
        invalidateAllXAxisPoints();
    }
    invalidateGraphPointAt(index);
    return index;
}

} // namespace touchgfx
