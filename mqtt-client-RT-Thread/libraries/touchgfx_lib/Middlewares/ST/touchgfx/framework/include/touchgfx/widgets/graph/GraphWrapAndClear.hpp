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
 * @file touchgfx/widgets/graph/GraphWrapAndClear.hpp
 *
 * Declares the touchgfx::DataGraphWrapAndClear and touchgfx::GraphWrapAndClear classes.
 */
#ifndef GRAPHWRAPANDCLEAR_HPP
#define GRAPHWRAPANDCLEAR_HPP

#include <touchgfx/widgets/graph/AbstractDataGraph.hpp>

namespace touchgfx
{
/**
 * The DataGraphWrapAndClear will show new points progressing across the graph. Once the graph is
 * filled, the next point added will cause the graph to be cleared and a new graph will slowly
 * be created as new values are added.
 */
class DataGraphWrapAndClear : public AbstractDataGraphWithY
{
public:
    /**
     * Initializes a new instance of the DataGraphWrapAndOverwrite class.
     *
     * @param      capacity The capacity.
     * @param [in] values   Pointer to memory with room for capacity elements of type T.
     */
    DataGraphWrapAndClear(int16_t capacity, int* values);

    virtual int32_t indexToGlobalIndex(int16_t index) const;

protected:
    virtual void beforeAddValue();

    virtual int16_t addValue(int value);
};

/**
 * The GraphWrapAndClear will show new points progressing across the graph. Once the graph is filled,
 * the next point added will cause the graph to be cleared and a new graph will slowly be
 * created as new values are added.
 */
template <int16_t CAPACITY>
class GraphWrapAndClear : public DataGraphWrapAndClear
{
public:
    GraphWrapAndClear()
        : DataGraphWrapAndClear(CAPACITY, yValues)
    {
    }

private:
    int yValues[CAPACITY];
};

} // namespace touchgfx

#endif // GRAPHWRAPANDCLEAR_HPP
