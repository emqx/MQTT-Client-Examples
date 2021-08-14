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
 * @file touchgfx/widgets/graph/AbstractDataGraph.hpp
 *
 * Declares classes touchgfx::AbstractDataGraph and touchgfx::AbstractDataGraphWithY.
 */
#ifndef ABSTRACTDATAGRAPH_HPP
#define ABSTRACTDATAGRAPH_HPP

#include <touchgfx/Color.hpp>
#include <touchgfx/TypedText.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>
#include <touchgfx/widgets/canvas/Canvas.hpp>
#include <touchgfx/widgets/canvas/CanvasWidget.hpp>

namespace touchgfx
{
class AbstractGraphElement;
class AbstractGraphDecoration;

/** An abstract data graph. */
class AbstractDataGraph : public Container
{
public:
    /**
     * An object of this type is passed with each callback that is sent when the graph is
     * clicked. The object contains the data index that was pressed and the details of the click
     * event, e.g. PRESSED, RELEASED and screen coordinates.
     */
    class GraphClickEvent
    {
    public:
        /**
         * Initializes a new instance of the GraphClickEvent class.
         *
         * @param  i     The index of the item clicked.
         * @param  event The ClickEvent that caused the callback to be executed.
         *
         * @see setClickAction
         */
        GraphClickEvent(int16_t i, const ClickEvent& event)
            : index(i), clickEvent(event)
        {
        }
        int16_t index;                ///< The index of the item clicked.
        const ClickEvent& clickEvent; ///< The ClickEvent that caused the callback to be executed.
    };

    /**
     * An object of this type is passed with each callback that is sent when the graph is
     * dragged. The object contains the data index that was pressed and the details of the drag
     * event, e.g. old and new screen coordinates.
     */
    class GraphDragEvent
    {
    public:
        /**
         * Initializes a new instance of the GraphDragEvent class.
         *
         * @param  i     The index of the item where the drag has ended.
         * @param  event The DragEvent that caused the callback to be executed.
         *
         * @see setDragAction
         */
        GraphDragEvent(int16_t i, const DragEvent& event)
            : index(i), dragEvent(event)
        {
        }
        int16_t index;              ///< The index of the item where the drag has ended.
        const DragEvent& dragEvent; ///< The DragEvent that caused the callback to be executed.
    };

    /**
     * Initializes a new instance of the AbstractDataGraph class.
     *
     * @param  capacity The capacity.
     */
    AbstractDataGraph(int16_t capacity);

    /**
     * Sets a scaling factor to be multiplied on each added element. Since the graph only stores
     * integer values internally, it is possible to set a scale to e.g. 1000 and make the graph
     * work as if there are three digits of precision. The addDataPoint() will multiply the
     * argument with the scaling factor and store this value.
     *
     * By setting the scale to 1 it is possible to simply use integer values for the graph.
     *
     * @param  scale The scaling factor.
     *
     * @see getScale
     *
     * @note Calling setScale after adding points to the graph has undefined behaviour. The scale
     *       should be set as the first thing before other settings of the graph is being set.
     */
    virtual void setScale(int scale);

    /**
     * Gets the scaling factor previously set using setScale().
     *
     * @return The scaling factor.
     *
     * @see setScale
     */
    int getScale() const;

    /**
     * @copydoc Image::setAlpha()
     *
     * @note All graph elements have to take this alpha into consideration.
     */
    void setAlpha(uint8_t newAlpha);

    /** @copydoc Image::getAlpha() */
    uint8_t getAlpha() const;

    virtual void setWidth(int16_t width);

    virtual void setHeight(int16_t height);

    /**
     * Sets graph position inside the widget by reserving a margin around the graph. These
     * areas to the left, the right, above and below are used for optional axis and titles.
     *
     * @param  top    The top margin in pixels.
     * @param  left   The left margin in pixels.
     * @param  right  The right margin in pixels.
     * @param  bottom The bottom margin in pixels.
     *
     * @see GraphLabelsX, GraphLabelsY, GraphTitle
     *
     * @note The graph is automatically invalidated when the graph margins are changed.
     */
    void setGraphAreaMargin(int16_t top, int16_t left, int16_t right, int16_t bottom);

    /**
     * Gets graph margin top.
     *
     * @return The graph margin top.
     *
     * @see setGraphAreaMargin
     */
    int16_t getGraphAreaMarginTop() const;

    /**
     * Gets graph margin left.
     *
     * @return The graph margin left.
     *
     * @see setGraphAreaMargin
     */
    int16_t getGraphAreaMarginLeft() const;

    /**
     * Gets graph margin right.
     *
     * @return The graph margin right.
     *
     * @see setGraphAreaMargin
     */
    int16_t getGraphAreaMarginRight() const;

    /**
     * Gets graph margin bottom.
     *
     * @return The graph margin bottom.
     *
     * @see setGraphAreaMargin
     */
    int16_t getGraphAreaMarginBottom() const;

    /**
     * Adds some padding around the graph that will not be drawn in (apart from dots, boxes etc.
     * that extend around the actual data point). The set padding will also work to make a gap
     * between the graph and any labels that might have been added to the graph. To reserve an
     * area that the graph will not be drawn in, use setGraphAreaMargin.
     *
     * @param  top    The top padding in pixels.
     * @param  left   The left padding in pixels.
     * @param  right  The right padding in pixels.
     * @param  bottom The bottom padding in pixels.
     *
     * @see setGraphAreaMargin
     *
     * @note The graph is automatically invalidated when the margins are set.
     */
    void setGraphAreaPadding(int16_t top, int16_t left, int16_t right, int16_t bottom);

    /**
     * Gets graph area padding top.
     *
     * @return The graph areapadding top.
     *
     * @see setGraphAreaPadding
     */
    int16_t getGraphAreaPaddingTop() const;

    /**
     * Gets graph area padding left.
     *
     * @return The graph area padding left.
     *
     * @see setGraphAreaPadding
     */
    int16_t getGraphAreaPaddingLeft() const;

    /**
     * Gets graph area padding right.
     *
     * @return The graph area padding right.
     *
     * @see setGraphAreaPadding
     */
    int16_t getGraphAeraPaddingRight() const;

    /**
     * Gets graph area padding bottom.
     *
     * @return The graph area padding bottom.
     *
     * @see setGraphAreaPadding
     */
    int16_t getGraphAreaPaddingBottom() const;

    /**
     * Gets graph area width. This is the width of the actual graph area and is the same as the
     * width of the graph widget where graph area margin and graph area padding has been removed.
     *
     * @return The graph area width.
     */
    int16_t getGraphAreaWidth() const;

    /**
     * Gets graph area width including padding (but not margin). This is the width of the actual
     * graph area and is the same as the width of the graph widget where graph area margin has
     * been removed.
     *
     * @return The graph width including graph padding.
     */
    int16_t getGraphAreaWidthIncludingPadding() const;

    /**
     * Gets graph area height. This is the height of the actual graph area and is the same as
     * the height of the graph widget where graph area margin and graph area padding has been
     * removed.
     *
     * @return The graph area height.
     */
    int16_t getGraphAreaHeight() const;

    /**
     * Gets graph area height including padding (but not margin). This is the height of the
     * actual graph area and is the same as the height of the graph widget where graph area
     * margin has been removed.
     *
     * @return The graph area height including graph padding.
     */
    int16_t getGraphAreaHeightIncludingPadding() const;

    /**
     * Sets minimum and maximum x and y coordinate ranges for the graph. This can be used to
     * zoom in or out and only show parts of the graph.
     *
     * @param  xMin The minimum x coordinate.
     * @param  xMax The maximum x coordinate.
     * @param  yMin The minimum y coordinate.
     * @param  yMax The maximum y coordinate.
     *
     * @see setGraphRangeX, setGraphRangeY
     */
    void setGraphRange(int xMin, int xMax, int yMin, int yMax);

    /**
     * @copydoc AbstractDataGraph::setGraphRange(int,int,int,int)
     */
    void setGraphRange(int xMin, int xMax, float yMin, float yMax);

    /**
     * Sets minimum and maximum x coordinates for the graph. This can be used to zoom in or out
     * and only show parts of the graph.
     *
     * @param  min The minimum x coordinate.
     * @param  max The maximum x coordinate.
     *
     * @note The graph as well as the area above and below are automatically redrawn (invalidated).
     */
    virtual void setGraphRangeX(int min, int max) = 0;

    /**
     * Sets minimum and maximum y coordinates for the graph. This can be used to zoom in or out
     * and only show parts of the graph.
     *
     * @param  min The minimum y coordinate.
     * @param  max The maximum y coordinate.
     *
     * @note The graph as well as the area to the left and to the right of the graph are
     *       automatically redrawn (invalidated)
     */
    virtual void setGraphRangeY(int min, int max) = 0;

    /**
     * @copydoc AbstractDataGraph::setGraphRangeY(int,int)
     */
    virtual void setGraphRangeY(float min, float max) = 0;

    /**
     * Gets the minimum x coordinate for the graph.
     *
     * @return The minimum x coordinate .
     */
    virtual int getGraphRangeXMin() const = 0;

    /**
     * Gets the maximum x coordinate for the graph.
     *
     * @return The maximum x coordinate .
     */
    virtual int getGraphRangeXMax() const = 0;

    /**
     * Gets minimum y coordinate for the graph.
     *
     * @return The minimum y coordinate.
     */
    virtual int getGraphRangeYMinAsInt() const = 0;

    /**
     * Gets minimum y coordinate for the graph.
     *
     * @return The minimum y coordinate.
     */
    virtual float getGraphRangeYMinAsFloat() const = 0;

    /**
     * Gets maximum y coordinate for the graph.
     *
     * @return The maximum y coordinate.
     */
    virtual int getGraphRangeYMaxAsInt() const = 0;

    /**
     * Gets maximum y coordinate for the graph.
     *
     * @return The maximum y coordinate.
     */
    virtual float getGraphRangeYMaxAsFloat() const = 0;

    /** Clears the graph to its blank/initial state. */
    virtual void clear();

    /**
     * Makes gap before the specified index. This can be used to split a graph in two, but for
     * some graph types, e.g. histograms, this has no effect. Only one gap can be specified at a
     * time. Specifying a new gap automatically removes the previous gap.
     *
     * @param  index Zero-based index where the gap should be placed.
     */
    void setGapBeforeIndex(int16_t index);

    /**
     * Gets gap before index as set using setGapBeforeIndex().
     *
     * @return The gap before index.
     * @see setGapBeforeIndex
     */
    int16_t getGapBeforeIndex() const;

    /**
     * Adds a graph element which will display the graph. Several graph elements can be added.
     * Examples of graph elements are lines, dots, histograms as well as horizontal and vertical
     * grid lines.
     *
     * @param [in] d an AbstractGraphElement to add.
     *
     * @see GraphElementGridX, GraphElementGridY, GraphElementArea, GraphElementBoxes,
     *      GraphElementDiamonds, GraphElementDots, GraphElementHistogram, GraphElementLine,
     */
    void addGraphElement(AbstractGraphElement& d);

    /**
     * Adds an element to be shown in the area above the graph. Labels and titles can be added
     * here.
     *
     * @param [in] d an AbstractGraphElement to add.
     *
     * @see GraphLabelsX, GraphTitle
     */
    void addTopElement(AbstractGraphDecoration& d);

    /**
     * Adds an element to be shown in the area to the left of the graph. Labels and titles can be added
     * here.
     *
     * @param [in] d an AbstractGraphElement to add.
     *
     * @see GraphLabelsY, GraphTitle
     */
    void addLeftElement(AbstractGraphDecoration& d);

    /**
     * Adds an element to be shown in the area to the right of the graph. Labels and titles can be added
     * here.
     *
     * @param [in] d an AbstractGraphElement to add.
     *
     * @see GraphLabelsY, GraphTitle
     */
    void addRightElement(AbstractGraphDecoration& d);

    /**
     * Adds an element to be shown in the area below the graph.  Labels and titles can be added
     * here.
     *
     * @param [in] d an AbstractGraphElement to add.
     *
     * @see GraphLabelsX, GraphTitle
     */
    void addBottomElement(AbstractGraphDecoration& d);

    /**
     * Gets the number of point used by the graph.
     *
     * @return The number of point used by the graph.
     */
    int16_t getUsedCapacity() const;

    /**
     * Gets the capacity (max number of points) of the graph.
     *
     * @return The capacity.
     */
    int16_t getMaxCapacity() const;

    /**
     * Gets graph index nearest to the given screen position. The distance to each point on the
     * graph is measured and the index of the point closest to the given position handed back.
     *
     * @param       x     The x coordinate.
     * @param       y     The y coordinate.
     * @param [out] index Zero-based index of the point closest to the given position.
     *
     * @return True if it succeeds, false if it fails.
     *
     * @see getNearestIndexForScreenX
     */
    virtual bool getNearestIndexForScreenXY(int16_t x, int16_t y, int16_t& index);

    /**
     * Gets graph index nearest to the given screen x coordinate. The index of the graph point
     * closest to the given x coordinate is handed back.
     *
     * @param       x     The x coordinate.
     * @param [out] index Zero-based index of the.
     *
     * @return True if it succeeds, false if it fails.
     *
     * @see getNearestIndexForScreenXY
     */
    virtual bool getNearestIndexForScreenX(int16_t x, int16_t& index) const;

    /**
     * Get the screen x coordinate for the given graph point index.
     *
     * @param  index Zero-based index of the point.
     *
     * @return The screen x coordinate.
     */
    int16_t indexToScreenX(int16_t index) const;

    /**
     * Get the screen y coordinate for the given graph point index.
     *
     * @param  index Zero-based index of the point.
     *
     * @return The screen x coordinate.
     */
    int16_t indexToScreenY(int16_t index) const;

    /**
     * Get the data point x value for the given graph point index.
     *
     * @param  index Zero-based index of the point.
     *
     * @return The data point x value.
     */
    int indexToDataPointXAsInt(int16_t index) const;

    /**
     * Get the data point x value for the given graph point index.
     *
     * @param  index Zero-based index of the point.
     *
     * @return The data point x value.
     */
    float indexToDataPointXAsFloat(int16_t index) const;

    /**
     * Get the data point y value for the given graph point index.
     *
     * @param  index Zero-based index of the point.
     *
     * @return The data point y value.
     */
    int indexToDataPointYAsInt(int16_t index) const;

    /**
     * Get the data point y value for the given graph point index.
     *
     * @param  index Zero-based index of the point.
     *
     * @return The data point y value.
     */
    float indexToDataPointYAsFloat(int16_t index) const;

    /**
     * Convert an index to global index. The index is the index of any data point, The global
     * index is a value that keeps growing whenever a new data point is added the the graph.
     *
     * @param  index Zero-based index of the point.
     *
     * @return The global index.
     */
    virtual int32_t indexToGlobalIndex(int16_t index) const;

    /**
     * Sets an action to be executed when the graph is clicked.
     *
     * @param  callback The callback.
     *
     * @see GraphClickEvent
     */
    void setClickAction(GenericCallback<const AbstractDataGraph&, const GraphClickEvent&>& callback);

    /**
     * Sets an action to be executed when the graph is dragged.
     *
     * @param  callback The callback.
     *
     * @see GraphDragEvent
     */
    void setDragAction(GenericCallback<const AbstractDataGraph&, const GraphDragEvent&>& callback);

    virtual void handleClickEvent(const ClickEvent& event);

    virtual void handleDragEvent(const DragEvent& event);

    /**
     * Multiply an integer value with a constant.
     *
     * @param  i     the value to scale.
     * @param  scale The scale.
     *
     * @return The product of the two numbers.
     */
    static int int2scaled(int i, int scale)
    {
        return i * scale;
    }

    /**
     * Multiply a floating point value with a constant and round the result.
     *
     * @param  f     the value to scale.
     * @param  scale The scale.
     *
     * @return The product of the two numbers, rounded to nearest integer value.
     */
    static int float2scaled(float f, int scale)
    {
        const float fs = f * scale;
        if (fs >= 0)
        {
            return int(fs + 0.5f);
        }
        return (int(fs) - 1) + int(1.5f + (fs - int(fs)));
    }

    /**
     * Divide an integer with a constant and round the result.
     *
     * @param  i     The number to divide.
     * @param  scale The divisor (scale).
     *
     * @return The number divided by the scale, rounded to nearest integer.
     */
    static int scaled2int(int i, int scale)
    {
        if (i >= 0)
        {
            return ((i * 2 + scale) / scale) / 2;
        }
        return -(((-i * 2 + scale - 1) / scale) / 2);
    }

    /**
     * Divide a floating point number with a constant.
     *
     * @param  i     The number to divide.
     * @param  scale The divisor (scale).
     *
     * @return The number divided by the scale.
     */
    static float scaled2float(int i, int scale)
    {
        return (float)i / (float)scale;
    }

protected:
    int dataScale;          ///< The data scale applied to all values
    uint8_t alpha;          ///< The alpha of the entire graph
    Container graphArea;    ///< The graph area (the center area)
    Container leftArea;     ///< The area to the left of the graph
    Container rightArea;    ///< The area to the right of the graph
    Container topArea;      ///< The area above the graph
    Container bottomArea;   ///< The area below the graph
    int16_t topPadding;     ///< The graph area top padding
    int16_t leftPadding;    ///< The graph area left padding
    int16_t rightPadding;   ///< The graph area right padding
    int16_t bottomPadding;  ///< The graph area bottom padding
    int16_t maxCapacity;    ///< Maximum number of points in the graph
    int16_t usedCapacity;   ///< The number of used points in the graph
    int16_t gapBeforeIndex; ///< The graph is disconnected (there is a gap) before this element index.

    GenericCallback<const AbstractDataGraph&, const GraphClickEvent&>* clickAction; ///< The callback to be executed when this Graph is clicked
    GenericCallback<const AbstractDataGraph&, const GraphDragEvent&>* dragAction;   ///< The callback to be executed when this Graph is dragged

    /**
     * Invalidate point at a given index. This will call the function invalidateGraphPointAt()
     * on every element added to the graphArea which in turn is responsible for invalidating the
     * part of the screen occupied by its element.
     *
     * @param  index Zero-based index of the element to invalidate.
     */
    void invalidateGraphPointAt(int16_t index);

    /**
     * Invalidate entire graph area (the center of the graph). This is often useful when a graph
     * is cleared or the X or Y range is changed.
     */
    void invalidateGraphArea();

    /**
     * Invalidate x axis point at the given index. Since the y axis is often static, the x axis
     * can change, and all labels need to be updated wihtout redrawing the entire graph.
     *
     * @param  index The x index to invalidate.
     *
     * @see invalidateAllXAxisPoints
     */
    void invalidateXAxisPointAt(int16_t index);

    /**
     * Invalidate all x axis points. Similar to invalidateXAxisPointAt, this function will
     * iterate all visible x values and invalidate them in turn.
     *
     * @see invalidateXAxisPointAt
     */
    void invalidateAllXAxisPoints();

    /**
     * Updates the position of all elements in all area after a change in size of the
     * graph area and/or label padding.
     *
     * @note The entire graph area is invalidated.
     */
    void updateAreasPosition();

    /**
     * Same as setGraphRange(int,int,int,int) except the passed arguments are assumed scaled.
     *
     * @param  xMin The minimum x coordinate.
     * @param  xMax The maximum x coordinate.
     * @param  yMin The minimum y coordinate.
     * @param  yMax The maximum y coordinate.
     *
     * @see setGraphRange
     *
     * @note For internal use.
     */
    void setGraphRangeScaled(int xMin, int xMax, int yMin, int yMax);

    /**
     * Same as setGraphRangeY(int,int) except the passed arguments are assumed scaled.
     *
     * @param  min The minimum y coordinate.
     * @param  max The maximum y coordinate.
     *
     * @see setGraphRangeY
     *
     * @note For internal use.
     */
    virtual void setGraphRangeYScaled(int min, int max) = 0;

    /**
     * @copydoc AbstractDataGraph::getGraphRangeYMinAsInt()
     *
     * @see AbstractDataGraph::getGraphRangeYMinAsInt, AbstractDataGraph::getGraphRangeYMinAsFloat
     *
     * @note The returned value is left scaled.
     *
     * @note For internal use.
     */
    virtual int getGraphRangeYMinScaled() const = 0;

    /**
     * @copydoc AbstractDataGraph::getGraphRangeYMaxAsInt()
     *
     * @see AbstractDataGraph::getGraphRangeYMaxAsInt, AbstractDataGraph::getGraphRangeYMaxAsFloat
     *
     * @note The returned value is left scaled.
     *
     * @note For internal use.
     */
    virtual int getGraphRangeYMaxScaled() const = 0;

    /**
     * Same as indexToDataPointXAsInt(int16_t) except the returned value is left scaled.
     *
     * @param  index Zero-based index of the data point.
     *
     * @return The data point x value scaled.
     *
     * @see indexToDataPointXAsInt, indexToDataPointXAsFloat
     *
     * @note For internal use.
     */
    virtual int indexToDataPointXScaled(int16_t index) const = 0;

    /**
     * Same as indexToDataPointYAsInt(int16_t) except the returned value is left scaled.
     *
     * @param  index Zero-based index of the data point.
     *
     * @return The data point y value scaled.
     *
     * @see indexToDataPointYAsInt, indexToDataPointYAsFloat
     *
     * @note For internal use.
     */
    virtual int indexToDataPointYScaled(int16_t index) const = 0;

    /**
     * Gets screen x coordinate for an absolute value.
     *
     * @param  x The x value.
     *
     * @return The screen x coordinate for the given value.
     */
    virtual CWRUtil::Q5 valueToScreenXQ5(int x) const = 0;

    /**
     * Gets screen y coordinate for an absolute value.
     *
     * @param  y The y value.
     *
     * @return The screen y coordinate for the given value.
     */
    virtual CWRUtil::Q5 valueToScreenYQ5(int y) const = 0;

    /**
     * Gets screen x coordinate for a specific data point added to the graph.
     *
     * @param  index The index of the element to get the x coordinate for.
     *
     * @return The screen x coordinate for the specific data point.
     */
    virtual CWRUtil::Q5 indexToScreenXQ5(int16_t index) const = 0;

    /**
     * Gets screen y coordinate for a specific data point added to the graph.
     *
     * @param  index The index of the element to get the y coordinate for.
     *
     * @return The screen x coordinate for the specific data point.
     */
    virtual CWRUtil::Q5 indexToScreenYQ5(int16_t index) const = 0;

    /**
     * Gets index range for screen x coordinate range taking the current graph range into
     * account.
     *
     * @param       xLo       The low screen x coordinate.
     * @param       xHi       The high screen x coordinate.
     * @param [out] indexLow  The low element index.
     * @param [out] indexHigh The high element index.
     *
     * @return True if the range from low index to high index is legal.
     *
     * @note For internal use.
     */
    virtual bool xScreenRangeToIndexRange(int16_t xLo, int16_t xHi, int16_t& indexLow, int16_t& indexHigh) const = 0;

    /**
     * Converts a number with one scale to a number that has the same scale as the graph.
     *
     * @param  value The value to convert.
     * @param  scale The scale.
     *
     * @return The given data converted to the graph scale.
     *
     * @note For internal use.
     */
    int convertToGraphScale(int value, int scale) const;

    /**
     * Get x axis scale as a scaled value.
     *
     * @return The x axis scale (left scaled).
     *
     * @note For internal use.
     */
    virtual int getXAxisScaleScaled() const;

    /**
     * Get x axis offset as a scaled value.
     *
     * @return The x axis offset (left scaled).
     *
     * @note For internal use.
     */
    virtual int getXAxisOffsetScaled() const;

    /**
     * Same as int2scaled(int,int) using the graph's scale.
     *
     * @param  i The integer value to scale.
     *
     * @return The scaled integer.
     *
     * @note For internal use.
     */
    int int2scaled(int i) const
    {
        return int2scaled(i, dataScale);
    }

    /**
     * Same as float2scaled(float,int) using the graph's scale.
     *
     * @param  f The floating point value to scale.
     *
     * @return The scaled value.
     *
     * @note For internal use.
     */
    int float2scaled(float f) const
    {
        return float2scaled(f, dataScale);
    }

    /**
     * Same as scaled2int(int,int) using the graph's scale.
     *
     * @param  i The scaled value to convert to an integer.
     *
     * @return The unscaled value.
     *
     * @note For internal use.
     */
    int scaled2int(int i) const
    {
        return scaled2int(i, dataScale);
    }

    /**
     * Same as scaled2float(int,int) using the graph's scale.
     *
     * @param  i The scaled value to convert to a floating point value.
     *
     * @return The unscaled value.
     *
     * @note For internal use.
     */
    float scaled2float(int i) const
    {
        return scaled2float(i, dataScale);
    }

    friend class AbstractGraphElement;
};

/**
 * Abstract helper class used to implement graphs with the same distance between the x values (i.e. x is ignored).
 */
class AbstractDataGraphWithY : public AbstractDataGraph
{
public:
    /**
     * Initializes a new instance of the AbstractDataGraphWithY class.
     *
     * @param      capacity The capacity.
     * @param [in] values   Address where to store the y values of the graph.
     */

    AbstractDataGraphWithY(int16_t capacity, int* values);

    /**
     * Adds a new data point to the end of the graph. The part of the graph that is changed,
     * is automatically redrawn (invalidated).
     *
     * @param  y The new data point.
     *
     * @return The index of the just added value.
     */
    int16_t addDataPoint(int y);

    /** @copydoc addDataPoint(int) */
    int16_t addDataPoint(float y);

    virtual void setGraphRangeX(int min, int max);
    virtual void setGraphRangeY(int min, int max);
    virtual void setGraphRangeY(float min, float max);
    virtual int getGraphRangeXMin() const;
    virtual int getGraphRangeXMax() const;
    virtual int getGraphRangeYMinAsInt() const;
    virtual float getGraphRangeYMinAsFloat() const;
    virtual int getGraphRangeYMaxAsInt() const;
    virtual float getGraphRangeYMaxAsFloat() const;

    /**
     * Automatic adjust min and max y coordinate to show entire graph. It is possible to ensure
     * that the x axis (i.e. y=0) is included in the new range. If the graph range is changed,
     * the graph is automatically redrawn (invalidated).
     *
     * @param  showXaxis (Optional) True to ensure that the x axis is visible (default is true).
     * @param  margin    (Optional) The margin to add above/below the max/min y value (default is
     *                   no margin).
     *
     * @note This takes the current visible x coordinate range into account.
     */
    void setGraphRangeYAuto(bool showXaxis = true, int margin = 0);

    virtual void setScale(int scale);

    /**
     * Set x coordinate axis scale value. This is the real x value increment between two data
     * points added to the graph.
     *
     * @param  scale The x axis scale.
     */
    virtual void setXAxisScale(int scale);

    /** @copydoc setXAxisScale(int) */
    virtual void setXAxisScale(float scale);

    /**
     * Get x coordinate axis scale value. This is the real x value increment between two data
     * points added to the graph.
     *
     * @return The x axis scale.
     */
    virtual int getXAxisScaleAsInt() const;

    /** @copydoc getXAxisScaleAsInt() */
    virtual float getXAxisScaleAsFloat() const;

    /**
     * Set x coordinate axis offset value. This is the real x value of the first data point
     * added to the graph (i.e. the data point at index 0).
     *
     * @param  offset The x axis offset.
     */
    virtual void setXAxisOffset(int offset);

    /** @copydoc setXAxisOffset(int) */
    virtual void setXAxisOffset(float offset);

    /**
     * Get x coordinate axis offset value. This is the real x value of the first data point
     * added to the graph (i.e. the data point at index 0).
     *
     * @return The x axis offset.
     */
    virtual int getXAxisOffsetAsInt() const;

    /** @copydoc getXAxisOffsetAsInt() */
    virtual float getXAxisOffsetAsFloat() const;

protected:
    int* yValues;         ///< The values of the graph
    uint32_t dataCounter; ///< The data counter of how many times addDataPoint() has been called
    int xOffset;          ///< The x axis offset (real value of data point at index 0)
    int xScale;           ///< The x axis scale (increment between two data points)

    /**
     * Same as addDataPoint(int) except the passed argument is assumed scaled. Adds a data point
     * scaled.
     *
     * @param  y The y coordinate.
     *
     * @return The index of the added data point.
     */
    int16_t addDataPointScaled(int y);

    /**
     * This function is called before a new value (data point) is added. This allows for
     * invalidation to be calculated based on the global data counter before it is increased as
     * a result of adding the new point.
     */
    virtual void beforeAddValue();

    /**
     * Adds a value to the internal data array and keeps track of when graph points, graph axis
     * and the entire graph needs to be redrawn (invalidated).
     *
     * @param  value The value to add to the array.
     *
     * @return The index of the newly added value.
     */
    virtual int16_t addValue(int value) = 0;

    /**
     * Get the real index in the yValues array of the given index. Normally this is just the 'i'
     * but e.g. DataGraphScroll does not, for performance reasons.
     *
     * @param  index Zero-based index.
     *
     * @return The index in the yValues array.
     */
    virtual int16_t realIndex(int16_t index) const;

    virtual int indexToDataPointXScaled(int16_t index) const;

    virtual int indexToDataPointYScaled(int16_t index) const;

    virtual bool xScreenRangeToIndexRange(int16_t xLo, int16_t xHi, int16_t& indexLow, int16_t& indexHigh) const;

    /**
     * Set x coordinate axis scale value using a pre-scaled value. This is the real x value
     * increment between two data points added to the graph.
     *
     * @param  scale The x axis scale.
     *
     * @see setXAxisScale
     *
     * @note For internal use.
     */
    virtual void setXAxisScaleScaled(int scale);

    virtual int getXAxisScaleScaled() const;

    virtual void setGraphRangeYScaled(int min, int max);

    virtual int getGraphRangeYMinScaled() const;

    virtual int getGraphRangeYMaxScaled() const;

    /**
     * Set x coordinate axis offset value with a pre-scaled offset value. This is the real x
     * value of the first data point added to the graph (i.e. the data point at index 0).
     *
     * @param  offset The x axis offset.
     *
     * @see setXAxisOffset
     *
     * @note For internal use.
     */
    virtual void setXAxisOffsetScaled(int offset);

    virtual int getXAxisOffsetScaled() const;

    virtual CWRUtil::Q5 valueToScreenXQ5(int x) const;

    virtual CWRUtil::Q5 valueToScreenYQ5(int y) const;

    virtual CWRUtil::Q5 indexToScreenXQ5(int16_t index) const;

    virtual CWRUtil::Q5 indexToScreenYQ5(int16_t index) const;

private:
    int graphRangeMinX;
    int graphRangeMaxX;
    int graphRangeMinY;
    int graphRangeMaxY;
};

} // namespace touchgfx

#endif // ABSTRACTDATAGRAPH_HPP
