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
 * @file touchgfx/widgets/graph/GraphScroll.hpp
 *
 * Declares the touchgfx::DataGraphScroll and touchgfx::GraphScroll classes.
 */
#ifndef GRAPHSCROLL_HPP
#define GRAPHSCROLL_HPP

#include <touchgfx/widgets/graph/AbstractDataGraph.hpp>

namespace touchgfx
{
/**
 * DataGraphScroll is used to display a graph that continuously scrolls to the left every
 * time a new value is added to the graph. Because the graph is scrolled every time a new value
 * is added, the graph has to be re-drawn which can be quite demanding for the hardware
 * depending on the graph elements used in the graph.
 */
class DataGraphScroll : public AbstractDataGraphWithY
{
public:
    /**
     * Initializes a new instance of the DataGraphScroll class.
     *
     * @param          capacity The capacity.
     * @param [in] values   Pointer to memory with room for capacity elements of type T.
     */
    DataGraphScroll(int16_t capacity, int* values);

    virtual void clear();

    virtual int32_t indexToGlobalIndex(int16_t index) const;

protected:
    int16_t current; ///< The current position used for inserting new elements

    virtual void beforeAddValue();

    virtual int16_t addValue(int value);

    virtual int16_t realIndex(int16_t index) const;

private:
    virtual CWRUtil::Q5 indexToXQ5(int16_t index) const;
};

/**
 * A Widget capable of drawing a graph with various visual styles and different appearances for
 * the new values added to the graph.
 */
template <int16_t CAPACITY>
class GraphScroll : public DataGraphScroll
{
public:
    GraphScroll()
        : DataGraphScroll(CAPACITY, yValues)
    {
    }

private:
    int yValues[CAPACITY];
};
} // namespace touchgfx

#endif // GRAPHSCROLL_HPP
