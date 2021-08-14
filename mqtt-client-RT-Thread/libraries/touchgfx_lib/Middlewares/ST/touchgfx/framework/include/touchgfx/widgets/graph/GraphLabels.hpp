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
 * @file touchgfx/widgets/graph/GraphLabels.hpp
 *
 * Declares classes for adding labels and title to the graph.
 */
#ifndef GRAPHLABELS_HPP
#define GRAPHLABELS_HPP

#include <touchgfx/widgets/graph/AbstractDataGraph.hpp>
#include <touchgfx/widgets/graph/GraphElements.hpp>

namespace touchgfx
{
/** Helper class used for adding labels around the graph. Currently empty. */
class AbstractGraphDecoration : public AbstractGraphElementNoCWR
{
};

/**
 * Helper class for adding labels on the side of a graph.
 *
 * @see GraphLabelsX, GraphLabelsY
 */
class GraphLabelsBase : public AbstractGraphDecoration
{
public:
    GraphLabelsBase();

    /**
     * Sets the interval between each label.
     *
     * @param  interval The interval between each label.
     *
     * @see getIntervalAsInt, getIntervalAsFloat, setMajorLabel
     *
     * @note If interval is 0 only the axis is shown.
     */
    void setInterval(int interval);

    /** @copydoc setInterval(int) */
    void setInterval(float interval);

    /**
     * Gets the interval between each label.
     *
     * @return The interval between each label.
     *
     * @see setInterval
     */
    int getIntervalAsInt() const;

    /** @copydoc getIntervalAsInt() */
    float getIntervalAsFloat() const;

    /**
     * Sets "major" label that will be responsible for drawing major labels. If a label
     * would be drawn at the same position as the major label, the label will not be
     * drawn.
     *
     * @param  major Reference to a major label object.
     */
    void setMajorLabel(const GraphLabelsBase& major);

    /**
     * Sets TypedText to use for the label. The TypedText should contain exactly one wildcard.
     *
     * @param  typedText The typed text.
     *
     * @see getLabelTypedText
     */
    void setLabelTypedText(const TypedText& typedText);

    /**
     * Gets TypedText label.
     *
     * @return The label typed text.
     *
     * @see setLabelTypedText
     */
    TypedText getLabelTypedText() const;

    /**
     * Sets label rotation.
     *
     * @param  rotation The rotation or the text.
     *
     * @see getLabelRotation
     */
    void setLabelRotation(TextRotation rotation);

    /**
     * Gets label rotation.
     *
     * @return The label rotation.
     *
     * @see setLabelRotation
     */
    TextRotation getLabelRotation() const;

    /**
     * Sets number of decimals for labels, default is no decimals and no decimal point.
     *
     * @param  decimals The number of label decimals.
     *
     * @see setLabelDecimalPoint
     */
    void setLabelDecimals(uint16_t decimals);

    /**
     * Gets number of decimals for labels.
     *
     * @return The number of label decimals.
     */
    uint16_t getLabelDecimals() const;

    /**
     * Sets label decimal point. Default is to use '.' but this can be changed using this
     * function.
     *
     * @param  decimalPoint The character to use for decimal point.
     *
     * @see setLabelDecimals
     *
     * @note The decimal point is only set if the label decimals > 0.
     */
    void setLabelDecimalPoint(Unicode::UnicodeChar decimalPoint);

    /**
     * Gets label decimal point previously set.
     *
     * @return The label decimal point.
     *
     * @see setLabelDecimalPoint
     */
    Unicode::UnicodeChar getLabelDecimalPoint() const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    int labelInterval;                      ///< The interval between each label.
    TypedText labelTypedText;               ///< The TypedText to use for the label.
    TextRotation labelRotation;             ///< The TextRotation to use for the label.
    uint16_t labelDecimals;                 ///< The number of decimals on the label.
    Unicode::UnicodeChar labelDecimalPoint; ///< The label decimal point character.
    const GraphLabelsBase* majorLabel;      ///< A pointer to a major label, if any

    /**
     * Gets correctly scaled major interval, as the major label may have a scale that differs the
     * scale of the graph and this label.
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

    /**
     * Format label according to the set number of decimals and the decimal point.
     *
     * @param [in,out] buffer       The buffer to fill with the formatted number.
     * @param          bufferSize   Size of the buffer.
     * @param          label        The label value.
     * @param          decimals     The number of decimals.
     * @param          decimalPoint The decimal point.
     * @param          scale        The scale of the label value.
     */
    void formatLabel(Unicode::UnicodeChar* buffer, int16_t bufferSize, int label, int decimals, Unicode::UnicodeChar decimalPoint, int scale) const;
};

/**
 * GraphLabelsX will draw labels along the X axis at given intervals. By combining two
 * GraphLabelsX it is possible to have different appearance for major and minor y offsets.
 */
class GraphLabelsX : public GraphLabelsBase
{
public:
    virtual void draw(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

protected:
    /**
     * Draw labels for all indexes in the given range. This is used where there is a gap in the
     * graph and the labels have to be drawn using different x scales.
     *
     * @param [in] invalidatedArea The canvas.
     * @param      fontToDraw      The font to draw.
     * @param      graph           The graph.
     * @param      indexLow        The minimum index.
     * @param      indexHigh       The maximum index.
     * @param      alpha           The alpha.
     */
    void drawIndexRange(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, int indexLow, int indexHigh, const uint8_t alpha) const;

    /**
     * Draw string using rotation and clipping to make sure it is written properly.
     *
     * @param  invalidatedArea The invalidated area.
     * @param  fontToDraw      The font to draw.
     * @param  graph           The graph.
     * @param  index           index of the data point.
     * @param  alpha           The alpha.
     */
    void drawString(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, int index, const uint8_t alpha) const;
};

/**
 * GraphLabelsY will draw labels along the Y axis at given intervals. By combining two
 * GraphLabelsY it is possible to have different appearance for major and minor y offsets.
 */
class GraphLabelsY : public GraphLabelsBase
{
public:
    virtual void draw(const Rect& invalidatedArea) const;

protected:
    /**
     * Draw string using rotation and clipping to make sure it is written properly.
     *
     * @param  invalidatedArea The invalidated area.
     * @param  fontToDraw      The font to draw.
     * @param  graph           The graph.
     * @param  valueScaled     The value (left scaled according to graph scale).
     * @param  labelScaled     The label value (left scaled according to graph label scale).
     * @param  alpha           The alpha.
     */
    void drawString(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, int valueScaled, int labelScaled, const uint8_t alpha) const;
};

/**
 * The GraphTitle is just a simple text, but it is automatically moved with the graph. Also, the
 * alpha value is combined with the alpha of the graph and so it will be faded if the graph is
 * faded.
 */
class GraphTitle : public AbstractGraphDecoration
{
public:
    GraphTitle();

    /**
     * Sets TypedText to use as a title. It can be any static text which is just added as a
     * title.
     *
     * @param  typedText The typed text.
     *
     * @see getTitleTypedText
     */
    void setTitleTypedText(const TypedText& typedText);

    /**
     * Gets title typed text.
     *
     * @return The title typed text.
     *
     * @see setTitleTypedText
     */
    TypedText getTitleTypedText() const;

    /**
     * Sets TextRotation of the title.
     *
     * @param  rotation The rotation.
     *
     * @see setTitleTypedText, getTitleRotation
     */
    void setTitleRotation(TextRotation rotation);

    /**
     * Gets title rotation.
     *
     * @return The title rotation.
     *
     * @see setTitleRotation
     */
    TextRotation getTitleRotation() const;

    virtual void draw(const Rect& invalidatedArea) const;

    virtual bool drawCanvasWidget(const Rect& invalidatedArea) const;

    virtual void invalidateGraphPointAt(int16_t index);

private:
    TypedText titleTypedText;   ///< The title typed text
    TextRotation titleRotation; ///< The title rotation
};

} // namespace touchgfx

#endif // GRAPHLABELS_HPP
