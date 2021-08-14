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
 * @file touchgfx/widgets/graph/GraphElements.hpp
 *
 * Declares the various graph element classes. Instances of these classes can be added to a graph.
 */
#ifndef GRAPHELEMENTS_HPP
#define GRAPHELEMENTS_HPP

#include <touchgfx/widgets/graph/AbstractDataGraph.hpp>

namespace touchgfx
{
/** An abstract graph element. Declares a couple of useful functions to help subclasses which use CWR (Canvas Widget Renderer). */
class AbstractGraphElement : public CanvasWidget
{
public:
    AbstractGraphElement();

    /**
     * Sets a scaling factor to be multiplied on each added element. Since only integer values
     * are stored internally, it is possible to set a scale to e.g. 1000 and make elements work
     * as if there are three digits of precision.
     *
     * By setting the scale to 1 it is possible to simply use integer values for the graph.
     *
     * @param  scale The scaling factor.
     *
     * @see getScale
     *
     * @note Calling setScale should be done as the first thing as any new scaling factor will not be applied to already set scaled values.
     */
    void setScale(int scale);

    /**
     * Gets the scaling factor set using setScale.
     *
     * @return The scaling factor.
     *
     * @see setScale
     */
    int getScale() const;

    /**
     * Invalidate the point at the given index. This allows a graph element to only invalidate
     * the minimum rectangle required for the given index. The Graph will call this function
     * before and after changing a point to ensure that both the old and the new area are
     * redrawn (invalidated).
     *
     * @param  index Zero-based index of the point.
     */
    virtual void invalidateGraphPointAt(int16_t index) = 0;

protected:
    int dataScale; ///< The scaling factor

    /**
     * Gets a pointer to the the graph containing the GraphElement.
     *
     * @return A pointer to the graph.
     */
    AbstractDataGraph* getGraph() const;

    /**
     * Gets graph screen x for x value.
     *
     * @param  graph The graph.
     * @param  x     The x value.
     *
     * @return The graph screen x for value.
     */
    CWRUtil::Q5 valueToScreenXQ5(const AbstractDataGraph* graph, int x) const;

    /**
     * Gets graph screen y for y value.
     *
     * @param  graph The graph.
     * @param  y     The y value.
     *
     * @return The graph screen y for value.
     */
    CWRUtil::Q5 valueToScreenYQ5(const AbstractDataGraph* graph, int y) const;

    /**
     * Gets screen x coordinate for a specific data point added to the graph.
     *
     * @param  graph The graph.
     * @param  index The index of the element to get the x coordinate for.
     *
     * @return The screen x coordinate for the specific data point.
     */
    CWRUtil::Q5 indexToScreenXQ5(const AbstractDataGraph* graph, int16_t index) const;

    /**
     * Gets screen y coordinate for a specific data point added to the graph.
     *
     * @param  graph The graph.
     * @param  index The index of the element to get the y coordinate for.
     *
     * @return The screen x coordinate for the specific data point.
     */
    CWRUtil::Q5 indexToScreenYQ5(const AbstractDataGraph* graph, int16_t index) const;

    /**
     * Gets graph element range for screen x coordinate range.
     *
     * @param       xLow        The low.
     * @param       xHigh       The high.
     * @param [out] elementLow  The element low.
     * @param [out] elementHigh The element high.
     *
     * @return True if it succeeds, false if it fails.
     */
    bool xScreenRangeToIndexRange(int16_t xLow, int16_t xHigh, int16_t& elementLow, int16_t& elementHigh) const;

    /**
     * Find the screen rectangle containing the Q5 screen rectangle by rounding the coordinates up/down.
     *
     * @param  screenXminQ5 The minimum screen x coordinate (in Q5).
     * @param  screenYminQ5 The maximum screen y coordinate (in Q5).
     * @param  screenXmaxQ5 The minimum screen x coordinate (in Q5).
     * @param  screenYmaxQ5 The maximum screen y coordinate (in Q5).
     *
     * @return A Rect containing the Q5 rectangle.
     */
    Rect rectFromQ5Coordinates(CWRUtil::Q5 screenXminQ5, CWRUtil::Q5 screenYminQ5, CWRUtil::Q5 screenXmaxQ5, CWRUtil::Q5 screenYmaxQ5) const;

    /**
     * Find the screen rectangle around a given point with the specified diameter.
     *
     * @param  xQ5        The screen x coordinate (in Q5).
     * @param  yQ5        The screen y coordinate (in Q5).
     * @param  diameterQ5 The diameter (in Q5).
     *
     * @return A Rect containing the point (and diameter).
     */
    Rect rectAround(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5, CWRUtil::Q5 diameterQ5) const;

    /**
     * Round the given CWRUtil::Q5 to the nearest integer and return it as a CWRUtil::Q5 instead
     * of an integer.
     *
     * @param  q5 The CWRUtil::Q5 value to round.
     *
     * @return The nearest integer as a CWRUtil::Q5 value.
     */
    CWRUtil::Q5 roundQ5(CWRUtil::Q5 q5) const;

    /**
     * @copydoc AbstractDataGraph::convertToGraphScale
     *
     * @param  graph The graph.
     */
    int convertToGraphScale(const AbstractDataGraph* graph, int value, int scale) const;

    /**
     * @copydoc AbstractDataGraph::getXAxisScaleScaled
     *
     * @param  graph The graph.
     */
    int getGraphXAxisScaleScaled(const AbstractDataGraph* graph) const;

    /**
     * @copydoc AbstractDataGraph::getXAxisOffsetScaled
     *
     * @param  graph The graph.
     */
    int getGraphXAxisOffsetScaled(const AbstractDataGraph* graph) const;

    /**
     * @copydoc AbstractDataGraph::getGraphRangeYMinScaled
     *
     * @param  graph The graph.
     */
    int getGraphRangeYMinScaled(const AbstractDataGraph* graph) const;

    /**
     * @copydoc AbstractDataGraph::getGraphRangeYMaxScaled
     *
     * @param  graph The graph.
     */
    int getGraphRangeYMaxScaled(const AbstractDataGraph* graph) const;

    /**
     * Query if the center of a given data point index is visible inside the graph area.
     *
     * @param  graph The graph.
     * @param  index The data point index.
     *
     * @return True if center invisible, false if not.
     */
    bool isCenterInvisible(const AbstractDataGraph* graph, int16_t index) const;
};

/** An abstract graph element. Declares a couple of useful functions to help subclasses which do not use CWR (Canvas Widget Renderer). */
class AbstractGraphElementNoCWR : public AbstractGraphElement
{
public:
    AbstractGraphElementNoCWR();

    /**
     * Sets the color of the graph element.
     *
     * @param  newColor The new color.
     *
     * @see getColor
     */
    virtual void setColor(colortype newColor);

    /**
     * Gets the color of the graph element.
     *
     * @return The color.
     *
     * @see setColor
     */
    virtual colortype getColor() const;

    virtual bool drawCanvasWidget(const Rect& invalidatedArea) const;

protected:
    colortype color; ///< The currently assigned color

    /**
     * Normalize rectangle by changing a rectangle with negative width or height to a rectangle
     * with positive width or height at the correct position.
     *
     * @param [in,out] rect The rectangle.
     */
    void normalizeRect(Rect& rect) const;

    /**
     * Protected function to prevent users from setting a painter.
     *
     * @param [in] painter The painter.
     */
    virtual void setPainter(AbstractPainter& painter) {};
};

/**
 * GraphElementGridBase is a helper class used to implement classed to draw grid lines in the graph.
 */
class GraphElementGridBase : public AbstractGraphElementNoCWR
{
public:
    GraphElementGridBase();

    /**
     * Sets the interval between each grid line.
     *
     * @param  interval The interval between each grid line.
     *
     * @see getIntervalAsInt, getIntervalAsFloat, setMajorGrid
     *
     * @note If interval is 0 only the axis is shown.
     */
    void setInterval(int interval);

    /** @copydoc setInterval(int) */
    void setInterval(float interval);

    /**
     * Gets the interval between each grid line.
     *
     * @return The interval between each grid line.
     *
     * @see setInterval
     */
    int getIntervalAsInt() const;

    /** @copydoc getIntervalAsInt() */
    float getIntervalAsFloat() const;

    /**
     * Sets "major" grid that will be responsible for drawing major grid lines. If a grid line
     * would be drawn at the same position as the major grid line, the grid line will not be
     * drawn.
     *
     * @param  major Reference to a major grid line object.
     */
    void setMajorGrid(const GraphElementGridBase& major);

    /**
     * Sets line width of the grid lines.
     *
     * @param  width The width of the grid lines.
     *
     * @see getLineWidth
     */
    void setLineWidth(uint8_t width);

    /**
     * Gets line width.
     *
     * @return The line width.
     *
     * @see setLineWidth
     */
    uint8_t getLineWidth() const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    int gridInterval;                      ///< The grid line interval.
    uint8_t lineWidth;                     ///< Width of the line.
    const GraphElementGridBase* majorGrid; ///< A pointer to a major grid, if any

    /**
     * Gets correctly scaled major interval, as the major grid may have a scale that differs the
     * scale of the graph and this grid line.
     *
     * @param  graph The graph.
     *
     * @return The correctly scaled major interval.
     */
    int getCorrectlyScaledMajorInterval(const AbstractDataGraph* graph) const;

    /**
     * @copydoc setInterval(int)
     *
     * @note The interval set here must already be scaled. For internal use.
     */
    void setIntervalScaled(int interval);

    /**
     * @copydoc getIntervalAsInt()
     *
     * @note The interval returned here is left unscaled. For internal use.
     */
    int getIntervalScaled() const;
};

/**
 * GraphElementGridX draws vertical lines at selected intervals along the x axis. By combining two
 * GraphElementGridX instances, it is possible to have minor and major grid lines.
 *
 * @note The grid lines are drawn using LCD::fillRect for higher performance.
 */
class GraphElementGridX : public GraphElementGridBase
{
public:
    virtual void draw(const Rect& invalidatedArea) const;

protected:
    /**
     * Draw vertical line using LCD::fillRect and handles negative dimensions properly.
     *
     * @param  invalidatedArea The invalidated area to intersect the line with.
     * @param  xMin            The minimum x coordinate.
     * @param  yMin            The minimum y coordinate.
     * @param  width           The width of the line.
     * @param  length          The length of the line.
     * @param  color           The color of the line.
     * @param  alpha           The alpha of the line.
     */
    void drawLine(const Rect& invalidatedArea, int16_t xMin, int16_t yMin, int16_t width, int16_t length, colortype color, uint8_t alpha) const;
};

/**
 * GraphElementGridY draws horizontal lines at selected intervals along the y axis. By combining
 * two GraphElementGridY instances, it is possible to have minor and major grid lines.
 *
 * @note The grid lines are drawn using LCD::fillRect for higher performance.
 */
class GraphElementGridY : public GraphElementGridBase
{
public:
    virtual void draw(const Rect& invalidatedArea) const;

protected:
    /**
     * Draw horizontal line using LCD::fillRect and handles negative dimensions properly.
     *
     * @param  invalidatedArea The invalidated area to intersect the line with.
     * @param  xMin            The minimum x coordinate.
     * @param  yMin            The minimum y coordinate.
     * @param  width           The width of the line.
     * @param  length          The length of the line.
     * @param  color           The color of the line.
     * @param  alpha           The alpha of the line.
     */
    void drawLine(const Rect& invalidatedArea, int16_t xMin, int16_t yMin, int16_t width, int16_t length, colortype color, uint8_t alpha) const;
};

/**
 * GraphElementArea will fill the area below the line connecting the data points in the graph.
 *
 * @note The Area is drawn using Canvas Widget Renderer which is slower but produces much nicer
 *       graphics.
 */
class GraphElementArea : public AbstractGraphElement
{
public:
    GraphElementArea();

    /**
     * Sets the base of the area drawn. Normally, the base is 0 which means that the area is
     * drawn below positive y values and above negative y values. Setting the base to a very
     * high number will cause the area above the graph to be drawn. Setting the base to a very
     * low number will cause the area below the graph to be drawn (even for negative numbers,
     * which are higher than the base value).
     *
     * @param  baseline The baseline value.
     *
     * @see getBaselineAsInt, getBaselineAsFloat
     */
    void setBaseline(int baseline);

    /** @copydoc setBaseline(int) */
    void setBaseline(float baseline);

    /**
     * Gets the base previously set using setBase.
     *
     * @return The base value.
     *
     * @see setBaseline
     */
    int getBaselineAsInt() const;

    /** @copydoc getBaselineAsInt() */
    float getBaselineAsFloat() const;

    virtual bool drawCanvasWidget(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    int yBaseline; ///< The base value.

    /**
     * @copydoc setBaseline(int)
     *
     * @note The baseline set here must already be scaled. For internal use.
     */
    void setBaselineScaled(int baseline);

    /**
     * @copydoc getBaselineAsInt()
     *
     * @note The baseline returned here is left unscaled. For internal use.
     */
    int getBaselineScaled() const;
};

/**
 * GraphElementLine will draw a line with a given thickness through the data points in the graph.
 *
 * @note The Line is drawn using Canvas Widget Renderer which is slower but produces much nicer
 *       graphics.
 */
class GraphElementLine : public AbstractGraphElement
{
public:
    GraphElementLine();

    /**
     * Sets line width.
     *
     * @param  width The width.
     *
     * @see getLineWidth
     */
    void setLineWidth(uint8_t width);

    /**
     * Gets line width.
     *
     * @return The line width.
     *
     * @see setLineWidth
     */
    uint8_t getLineWidth() const;

    virtual bool drawCanvasWidget(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    uint8_t lineWidth; ///< Width of the line

    /**
     * Draw a line between all indexes in the given range. This is used where there is a gap in
     * the graph and the line has to be drawn as two separate lines.
     *
     * @param [in] canvas   The canvas.
     * @param      graph    The graph.
     * @param      indexMin The minimum index.
     * @param      indexMax The maximum index.
     */
    void drawIndexRange(Canvas& canvas, const AbstractDataGraph* graph, int16_t indexMin, int16_t indexMax) const;
};

/**
 * The GraphElementVerticalGapLine is used to draw a vertical line where the gap in the graph is.
 * This only makes sense to add to a GraphWrapAndOverwrite (or DataGraphWrapAndOverwrite).
 *
 * @note The vertical line is drawn using LCD::fillRect for higher performance.
 */
class GraphElementVerticalGapLine : public AbstractGraphElementNoCWR
{
public:
    /**
     * Sets the width of the gap line in pixels. If the gap line is set to 0 the gap line will
     * extend to the next point in the graph.
     *
     * @param  width The width.
     *
     * @see getGapLineWidth
     */
    void setGapLineWidth(uint16_t width);

    /**
     * Gets the width of the gap line as set using setGapLineWidth().
     *
     * @return The gap line width.
     */
    uint16_t getGapLineWidth() const;

    virtual void draw(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    uint16_t lineWidth; ///< Width of the line
};

/**
 * The GraphElementHistogram is used to draw blocks from the x axis to the data point in the graph.
 * If more graphs are placed on top of each other, the histogram can be moved slightly to the
 * left/right.
 *
 * @note Historgram boxes are drawn using LCD::fillRect for higher performance.
 */
class GraphElementHistogram : public AbstractGraphElementNoCWR
{
public:
    GraphElementHistogram();

    /**
     * Sets the base of the area drawn. Normally, the base is 0 which means that the area is
     * drawn below positive y values and above negative y values. Setting the base to a very
     * high number will cause the area above the graph to be drawn. Setting the base to a very
     * low number will cause the area below the graph to be drawn (even for negative numbers,
     * which are higher than the base value).
     *
     * @param  baseline The base value.
     *
     * @see getBaselineAsInt, getBaselineAsFloat
     */
    void setBaseline(int baseline);

    /** @copydoc setBaseline(int) */
    void setBaseline(float baseline);

    /**
     * Gets the base previously set using setBaseline.
     *
     * @return The base value.
     *
     * @see setBaseline
     */
    int getBaselineAsInt() const;

    /** @copydoc getBaselineAsInt() */
    float getBaselineAsFloat() const;

    /**
     * Sets bar width of each histogram column.
     *
     * @param  width The width.
     *
     * @see getBarWidth
     */
    void setBarWidth(uint16_t width);

    /**
     * Gets bar width of the histogram columns.
     *
     * @return The bar width.
     *
     * @see setBarWidth
     */
    uint16_t getBarWidth() const;

    /**
     * Sets bar offset (horizontally). This can be used when there are two different histogram
     * graphs on top of each other to prevent one histogram from covering the other.
     *
     * @param  offset The offset.
     *
     * @see getBarOffset
     */
    void setBarOffset(int16_t offset);

    /**
     * Gets bar offset (horizontally). Bar offset can be used when there are two different
     * histogram graphs on top of each other to prevent one histogram from covering the other.
     *
     * @return The bar offset.
     *
     * @see setBarOffset
     */
    int16_t getBarOffset() const;

    virtual void draw(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    int yBaseline;     ///< The baseline
    uint16_t barWidth; ///< Width of each bar
    int16_t barOffset; ///< The horizontal bar offset

    /**
     * @copydoc setBaseline(int)
     *
     * @note The baseline set here must already be scaled. For internal use.
     */
    void setBaselineScaled(int baseline);

    /**
     * @copydoc getBaselineAsInt()
     *
     * @note The baseline returned here is left unscaled. For internal use.
     */
    int getBaselineScaled() const;
};

/**
 * GraphElementBoxes will draw square box for every data point in graph.
 *
 * @note The boxes are drawn using LCD::fillRect for higher performance. This also means that
 *       boxes with an odd width will not align properly if combined with a GraphElementLine or any
 *       other GraphElement that uses Canvas Widget Renderer. Use an even number for box width in
 *       these cases.
 */
class GraphElementBoxes : public AbstractGraphElementNoCWR
{
public:
    GraphElementBoxes();

    /**
     * Sets box width.
     *
     * @param  width The width.
     *
     * @see getBoxWidth
     */
    void setBoxWidth(uint16_t width);

    /**
     * Gets box width.
     *
     * @return The box width.
     *
     * @see setBoxWidth
     */
    uint16_t getBoxWidth() const;

    virtual void draw(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    uint16_t boxWidth; ///< Width of the box
};

/**
 * GraphElementDots will draw a circular dot for every data point in graph.
 *
 * @note The Dots are drawn using Canvas Widget Renderer which is slower but produces much nicer
 *       graphics.
 */
class GraphElementDots : public AbstractGraphElement
{
public:
    GraphElementDots();

    /**
     * Sets dot width.
     *
     * @param  width The width.
     *
     * @see getDotWidth
     */
    void setDotWidth(uint8_t width);

    /**
     * Gets dot width.
     *
     * @return The dot width.
     *
     * @see setDotWidth
     */
    uint8_t getDotWidth() const;

    virtual bool drawCanvasWidget(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    uint8_t dotWidth; ///< Width of the dot
};

/**
 * GraphElementDiamonds will draw a diamond (a square with the corners up/down/left/right) for
 * every data point in graph.
 *
 * @note The Diamonds are drawn using Canvas Widget Renderer which is slower but produces much
 *       nicer graphics.
 */
class GraphElementDiamonds : public AbstractGraphElement
{
public:
    GraphElementDiamonds();

    /**
     * Sets diamond width.
     *
     * @param  width The width.
     *
     * @see getDiamondWidth
     */
    void setDiamondWidth(uint8_t width);

    /**
     * Gets diamond width.
     *
     * @return The diamond width.
     *
     * @see setDiamondWidth
     */
    uint8_t getDiamondWidth() const;

    virtual bool drawCanvasWidget(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    uint8_t diamondWidth; ///< Width of the diamond
};
} // namespace touchgfx

#endif // GRAPHELEMENTS_HPP
