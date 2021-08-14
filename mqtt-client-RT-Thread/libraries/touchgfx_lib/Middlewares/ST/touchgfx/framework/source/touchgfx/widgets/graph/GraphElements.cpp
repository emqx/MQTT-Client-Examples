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

#include <touchgfx/widgets/graph/GraphElements.hpp>

namespace touchgfx
{
AbstractGraphElement::AbstractGraphElement()
    : dataScale(1)
{
}

void AbstractGraphElement::setScale(int scale)
{
    dataScale = scale;
}

int AbstractGraphElement::getScale() const
{
    return dataScale;
}

AbstractDataGraph* AbstractGraphElement::getGraph() const
{
    return (AbstractDataGraph*)(getParent()->getParent());
}

CWRUtil::Q5 AbstractGraphElement::valueToScreenXQ5(const AbstractDataGraph* graph, int x) const
{
    return graph->valueToScreenXQ5(x);
}

CWRUtil::Q5 AbstractGraphElement::valueToScreenYQ5(const AbstractDataGraph* graph, int y) const
{
    return graph->valueToScreenYQ5(y);
}

touchgfx::CWRUtil::Q5 AbstractGraphElement::indexToScreenXQ5(const AbstractDataGraph* graph, int16_t index) const
{
    return graph->indexToScreenXQ5(index);
}

touchgfx::CWRUtil::Q5 AbstractGraphElement::indexToScreenYQ5(const AbstractDataGraph* graph, int16_t index) const
{
    return graph->indexToScreenYQ5(index);
}

bool AbstractGraphElement::xScreenRangeToIndexRange(int16_t xLow, int16_t xHigh, int16_t& elementLow, int16_t& elementHigh) const
{
    return getGraph()->xScreenRangeToIndexRange(xLow, xHigh, elementLow, elementHigh);
}

Rect AbstractGraphElement::rectFromQ5Coordinates(CWRUtil::Q5 screenXminQ5, CWRUtil::Q5 screenYminQ5, CWRUtil::Q5 screenXmaxQ5, CWRUtil::Q5 screenYmaxQ5) const
{
    if (screenXminQ5 > screenXmaxQ5)
    {
        const CWRUtil::Q5 tmp = screenXminQ5;
        screenXminQ5 = screenXmaxQ5;
        screenXmaxQ5 = tmp;
    }
    if (screenYminQ5 > screenYmaxQ5)
    {
        const CWRUtil::Q5 tmp = screenYminQ5;
        screenYminQ5 = screenYmaxQ5;
        screenYmaxQ5 = tmp;
    }
    const int16_t xMin = screenXminQ5.to<int>();
    const int16_t yMin = screenYminQ5.to<int>();
    const int16_t xMax = screenXmaxQ5.ceil();
    const int16_t yMax = screenYmaxQ5.ceil();
    return Rect(xMin, yMin, xMax - xMin, yMax - yMin);
}

Rect AbstractGraphElement::rectAround(CWRUtil::Q5 xQ5, CWRUtil::Q5 yQ5, CWRUtil::Q5 diameterQ5) const
{
    const CWRUtil::Q5 d2Q5 = CWRUtil::Q5((int32_t)diameterQ5 + 1) / 2;
    return rectFromQ5Coordinates(xQ5 - d2Q5, yQ5 - d2Q5, xQ5 + d2Q5, yQ5 + d2Q5);
}

CWRUtil::Q5 AbstractGraphElement::roundQ5(CWRUtil::Q5 q5) const
{
    return CWRUtil::toQ5(q5.round());
}

int AbstractGraphElement::convertToGraphScale(const AbstractDataGraph* graph, int value, int scale) const
{
    return graph->convertToGraphScale(value, scale);
}

int AbstractGraphElement::getGraphXAxisScaleScaled(const AbstractDataGraph* graph) const
{
    return graph->getXAxisScaleScaled();
}

int AbstractGraphElement::getGraphXAxisOffsetScaled(const AbstractDataGraph* graph) const
{
    return graph->getXAxisOffsetScaled();
}

int AbstractGraphElement::getGraphRangeYMinScaled(const AbstractDataGraph* graph) const
{
    return graph->getGraphRangeYMinScaled();
}

int AbstractGraphElement::getGraphRangeYMaxScaled(const AbstractDataGraph* graph) const
{
    return graph->getGraphRangeYMaxScaled();
}

bool AbstractGraphElement::isCenterInvisible(const AbstractDataGraph* graph, int16_t index) const
{
    const int16_t screenXCenter = indexToScreenXQ5(graph, index).round();
    const int16_t screenYCenter = indexToScreenYQ5(graph, index).round();
    return (screenXCenter < graph->getGraphAreaPaddingLeft()
            || screenXCenter >= graph->getGraphAreaPaddingLeft() + graph->getGraphAreaWidth()
            || screenYCenter < graph->getGraphAreaPaddingTop()
            || screenYCenter >= graph->getGraphAreaPaddingTop() + graph->getGraphAreaHeight());
}

AbstractGraphElementNoCWR::AbstractGraphElementNoCWR()
    : color(0)
{
}

void AbstractGraphElementNoCWR::setColor(colortype newColor)
{
    color = newColor;
}

colortype AbstractGraphElementNoCWR::getColor() const
{
    return color;
}

bool AbstractGraphElementNoCWR::drawCanvasWidget(const Rect& /*invalidatedArea*/) const
{
    return true;
}

void AbstractGraphElementNoCWR::normalizeRect(Rect& rect) const
{
    if (rect.width < 0)
    {
        rect.x += rect.width;
        rect.width = -rect.width;
    }
    if (rect.height < 0)
    {
        rect.y += rect.height;
        rect.height = -rect.height;
    }
}

GraphElementGridBase::GraphElementGridBase()
    : gridInterval(10),
      lineWidth(1),
      majorGrid(0)
{
}

int GraphElementGridBase::getCorrectlyScaledMajorInterval(const AbstractDataGraph* graph) const
{
    return majorGrid == 0 ? 0 : convertToGraphScale(graph, majorGrid->getIntervalScaled(), majorGrid->getScale());
}

void GraphElementGridBase::setIntervalScaled(int interval)
{
    gridInterval = abs(interval);
}

void GraphElementGridBase::setInterval(int interval)
{
    setIntervalScaled(abs(interval) * dataScale);
}

void GraphElementGridBase::setInterval(float interval)
{
    setIntervalScaled(AbstractDataGraph::float2scaled(abs(interval), dataScale));
}

int GraphElementGridBase::getIntervalScaled() const
{
    return gridInterval;
}

int GraphElementGridBase::getIntervalAsInt() const
{
    return AbstractDataGraph::scaled2int(getIntervalScaled(), dataScale);
}

float GraphElementGridBase::getIntervalAsFloat() const
{
    return AbstractDataGraph::scaled2float(getIntervalScaled(), dataScale);
}

void GraphElementGridBase::setMajorGrid(const GraphElementGridBase& major)
{
    majorGrid = &major;
}

void GraphElementGridBase::setLineWidth(uint8_t width)
{
    lineWidth = width;
}

uint8_t GraphElementGridBase::getLineWidth() const
{
    return lineWidth;
}

void GraphElementGridBase::invalidateGraphPointAt(int16_t index)
{
}

void GraphElementGridX::draw(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    const uint8_t alpha = LCD::div255(getAlpha() * graph->getAlpha());
    if (lineWidth == 0 || alpha == 0)
    {
        return;
    }

    Rect invalidRect = Rect(0, graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidthIncludingPadding(), graph->getGraphAreaHeight()) & invalidatedArea;

    const int minorInterval = getIntervalAsInt();
    const int majorInterval = (majorGrid == 0) ? 0 : majorGrid->getIntervalAsInt();

    const CWRUtil::Q5 lw2Q5minor = CWRUtil::toQ5(lineWidth) / 2;
    const int16_t start = graph->getGraphAreaPaddingTop();
    const int16_t length = graph->getGraphAreaHeight();
    if (minorInterval == 0 && majorInterval == 0)
    {
        const int16_t pos = (valueToScreenXQ5(graph, 0) - lw2Q5minor).round();
        const int16_t graphPos = pos - graph->getGraphAreaPaddingLeft();
        if (graphPos + lineWidth >= 0 && graphPos < graph->getGraphAreaWidth())
        {
            drawLine(invalidRect, pos, start, lineWidth, length, color, alpha);
        }
    }
    else if (minorInterval > 0)
    {
        int rangeMin = graph->getGraphRangeXMin();
        int rangeMax = graph->getGraphRangeXMax();
        if (abs(rangeMax - rangeMin) / minorInterval > 500)
        {
            return; // Too many grid lines
        }

        if (valueToScreenXQ5(graph, rangeMin) > valueToScreenXQ5(graph, rangeMax))
        {
            const int tmp = rangeMin;
            rangeMin = rangeMax;
            rangeMax = tmp;
        }
        // Now rangeMax has a higher screen coordinate than rangeMin

        const int margin = rangeMin < rangeMax ? +1 : -1;
        const int minorLo = (rangeMin / minorInterval) - margin;
        const int minorHi = (rangeMax / minorInterval) + margin;
        const int minorStep = minorLo < minorHi ? +1 : -1;
        if (majorInterval == 0)
        {
            // No major lines, simply draw all lines
            for (int minorIndex = minorLo; minorIndex != minorHi + minorStep; minorIndex += minorStep)
            {
                const int16_t pos = (valueToScreenXQ5(graph, minorInterval * minorIndex) - lw2Q5minor).round();
                const int16_t graphPos = pos - graph->getGraphAreaPaddingLeft();
                if (graphPos + lineWidth >= 0 && graphPos < graph->getGraphAreaWidth())
                {
                    drawLine(invalidRect, pos, start, lineWidth, length, color, alpha);
                }
            }
        }
        else
        {
            const int majorLo = (rangeMin / majorInterval) - margin;
            const int majorHi = (rangeMax / majorInterval) + margin;
            const int majorStep = majorLo < majorHi ? +1 : -1;
            int majorIndex = majorLo;
            int16_t majorCoord = valueToScreenXQ5(graph, majorInterval * majorIndex).round();
            int minorIndex = minorLo;
            int16_t minorCoord = valueToScreenXQ5(graph, minorInterval * minorIndex).round();
            for (;;)
            {
                // Draw minor lines up to the major line
                while (minorCoord < majorCoord)
                {
                    const int16_t pos = (valueToScreenXQ5(graph, minorInterval * minorIndex) - lw2Q5minor).round();
                    const int16_t graphPos = pos - graph->getGraphAreaPaddingLeft();
                    if (graphPos + lineWidth >= 0 && graphPos < graph->getGraphAreaWidth())
                    {
                        drawLine(invalidRect, pos, start, lineWidth, length, color, alpha);
                    }
                    minorIndex += minorStep;
                    minorCoord = valueToScreenXQ5(graph, minorInterval * minorIndex).round();
                }
                // Advance minor past the major line we are about to draw
                while (minorCoord <= majorCoord)
                {
                    minorIndex += minorStep;
                    minorCoord = valueToScreenXQ5(graph, minorInterval * minorIndex).round();
                }
                if (majorCoord <= minorCoord)
                {
                    majorIndex += majorStep;
                    if (majorIndex == majorHi + majorStep)
                    {
                        break;
                    }
                    majorCoord = valueToScreenXQ5(graph, majorInterval * majorIndex).round();
                }
            }
        }
    }
}

void GraphElementGridX::drawLine(const Rect& invalidatedArea, int16_t xMin, int16_t yMin, int16_t width, int16_t length, colortype color, uint8_t alpha) const
{
    Rect rect(xMin, yMin, width, length);
    normalizeRect(rect);
    rect = rect & invalidatedArea;
    if (!rect.isEmpty())
    {
        translateRectToAbsolute(rect);
        HAL::lcd().fillRect(rect, color, alpha);
    }
}

void GraphElementGridY::draw(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    const uint8_t alpha = LCD::div255(getAlpha() * graph->getAlpha());
    if (lineWidth == 0 || alpha == 0)
    {
        return;
    }

    Rect invalidRect = Rect(graph->getGraphAreaPaddingLeft(), 0, graph->getGraphAreaWidth(), graph->getGraphAreaHeightIncludingPadding()) & invalidatedArea;

    const int minorInterval = convertToGraphScale(graph, gridInterval, dataScale);
    int majorInterval = getCorrectlyScaledMajorInterval(graph);

    const CWRUtil::Q5 lw2Q5minor = CWRUtil::toQ5(lineWidth) / 2;
    const int16_t start = graph->getGraphAreaPaddingLeft();
    const int16_t length = graph->getGraphAreaWidth();
    if (majorInterval == 0 && minorInterval == 0)
    {
        const int16_t pos = (valueToScreenYQ5(graph, 0) - lw2Q5minor).round();
        const int16_t graphPos = pos - graph->getGraphAreaPaddingTop();
        if (graphPos + lineWidth >= 0 && graphPos < graph->getGraphAreaHeight())
        {
            drawLine(invalidRect, start, pos, lineWidth, length, color, alpha);
        }
    }
    else if (minorInterval > 0)
    {
        int rangeMin = getGraphRangeYMinScaled(graph);
        int rangeMax = getGraphRangeYMaxScaled(graph);
        if (abs(rangeMax - rangeMin) / minorInterval > 500)
        {
            return; // Too many grid lines
        }

        if (valueToScreenYQ5(graph, rangeMin) > valueToScreenYQ5(graph, rangeMax))
        {
            const int tmp = rangeMin;
            rangeMin = rangeMax;
            rangeMax = tmp;
        }
        // Now rangeMax has a higher screen coordinate than rangeMin

        const int margin = rangeMin < rangeMax ? +1 : -1;
        const int minorLo = (rangeMin / minorInterval) - margin;
        const int minorHi = (rangeMax / minorInterval) + margin;
        const int minorStep = minorLo < minorHi ? +1 : -1;
        if (majorInterval == 0)
        {
            for (int minorIndex = minorLo; minorIndex != minorHi + minorStep; minorIndex += minorStep)
            {
                const int16_t pos = (valueToScreenYQ5(graph, minorInterval * minorIndex) - lw2Q5minor).round();
                const int16_t graphPos = pos - graph->getGraphAreaPaddingTop();
                if (graphPos + lineWidth >= 0 && graphPos < graph->getGraphAreaHeight())
                {
                    drawLine(invalidRect, start, pos, lineWidth, length, color, alpha);
                }
            }
        }
        else
        {
            const int majorLo = (rangeMin / majorInterval) - margin;
            const int majorHi = (rangeMax / majorInterval) + margin;
            const int majorStep = majorLo < majorHi ? +1 : -1;
            int majorIndex = majorLo;
            int16_t majorCoord = valueToScreenYQ5(graph, majorInterval * majorIndex).round();
            int minorIndex = minorLo;
            int16_t minorCoord = valueToScreenYQ5(graph, minorInterval * minorIndex).round();
            for (;;)
            {
                // Draw minor lines up to the major line
                while (minorCoord < majorCoord)
                {
                    const int16_t pos = (valueToScreenYQ5(graph, minorInterval * minorIndex) - lw2Q5minor).round();
                    const int16_t graphPos = pos - graph->getGraphAreaPaddingTop();
                    if (graphPos + lineWidth >= 0 && graphPos < graph->getGraphAreaHeight())
                    {
                        drawLine(invalidRect, start, pos, lineWidth, length, color, alpha);
                    }
                    minorIndex += minorStep;
                    minorCoord = valueToScreenYQ5(graph, minorInterval * minorIndex).round();
                }
                // Advance minor past the major line we are about to draw
                while (minorCoord <= majorCoord)
                {
                    minorIndex += minorStep;
                    minorCoord = valueToScreenYQ5(graph, minorInterval * minorIndex).round();
                }
                if (majorCoord <= minorCoord)
                {
                    majorIndex += majorStep;
                    if (majorIndex == majorHi + majorStep)
                    {
                        break;
                    }
                    majorCoord = valueToScreenYQ5(graph, majorInterval * majorIndex).round();
                }
            }
        }
    }
}

void GraphElementGridY::drawLine(const Rect& invalidatedArea, int16_t xMin, int16_t yMin, int16_t width, int16_t length, colortype color, uint8_t alpha) const
{
    Rect rect(xMin, yMin, length, width);
    normalizeRect(rect);
    rect = rect & invalidatedArea;
    if (!rect.isEmpty())
    {
        translateRectToAbsolute(rect);
        HAL::lcd().fillRect(rect, color, alpha);
    }
}

GraphElementArea::GraphElementArea()
    : yBaseline(0)
{
}

void GraphElementArea::setBaselineScaled(int baseline)
{
    yBaseline = baseline;
    invalidate();
}

void GraphElementArea::setBaseline(int baseline)
{
    setBaselineScaled(baseline * dataScale);
}

void GraphElementArea::setBaseline(float baseline)
{
    setBaselineScaled(AbstractDataGraph::float2scaled(baseline, dataScale));
}

int GraphElementArea::getBaselineScaled() const
{
    return yBaseline;
}

int GraphElementArea::getBaselineAsInt() const
{
    return AbstractDataGraph::scaled2int(getBaselineScaled(), dataScale);
}

float GraphElementArea::getBaselineAsFloat() const
{
    return AbstractDataGraph::scaled2float(getBaselineScaled(), dataScale);
}

bool GraphElementArea::drawCanvasWidget(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    if (graph->getUsedCapacity() <= 1)
    {
        return true; // Nothing to draw, everything is fine!
    }

    int16_t indexLow;
    int16_t indexHigh;
    if (!xScreenRangeToIndexRange(invalidatedArea.x, invalidatedArea.right(), indexLow, indexHigh))
    {
        return true; // Nothing to draw, everything is fine!
    }

    const int16_t gapIndex = graph->getGapBeforeIndex();
    const int baseline = convertToGraphScale(graph, yBaseline, dataScale);
    const CWRUtil::Q5 screenYbaseQ5 = roundQ5(valueToScreenYQ5(graph, baseline));
    CWRUtil::Q5 screenXQ5;
    if (indexLow + 1 == gapIndex)
    {
        if (indexLow > 0)
        {
            indexLow--; // Draw the last line segment before the gap
        }
        else
        {
            indexLow++; // Do not draw a 1 segment line (a "dot")
        }
    }

    Rect invalidRect = Rect(graph->getGraphAreaPaddingLeft(), graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidth(), graph->getGraphAreaHeight()) & invalidatedArea;
    Canvas canvas(this, invalidRect);
    canvas.moveTo(roundQ5(indexToScreenXQ5(graph, indexLow)), screenYbaseQ5);
    for (int16_t index = indexLow; index <= indexHigh; index++)
    {
        if (index == gapIndex)
        {
            canvas.lineTo(screenXQ5, screenYbaseQ5);
            screenXQ5 = roundQ5(indexToScreenXQ5(graph, index));
            canvas.lineTo(screenXQ5, screenYbaseQ5);
        }
        else
        {
            screenXQ5 = roundQ5(indexToScreenXQ5(graph, index));
        }
        canvas.lineTo(screenXQ5, roundQ5(indexToScreenYQ5(graph, index)));
    }
    canvas.lineTo(screenXQ5, screenYbaseQ5);
    return canvas.render(graph->getAlpha());
}

void GraphElementArea::invalidateGraphPointAt(int16_t index)
{
    const AbstractDataGraph* graph = getGraph();
    const int baseline = convertToGraphScale(graph, yBaseline, dataScale);
    const CWRUtil::Q5 screenYbaseQ5 = roundQ5(valueToScreenYQ5(graph, baseline));
    CWRUtil::Q5 screenXminQ5 = indexToScreenXQ5(graph, index);
    CWRUtil::Q5 screenXmaxQ5 = screenXminQ5;
    CWRUtil::Q5 screenYminQ5 = indexToScreenYQ5(graph, index);
    CWRUtil::Q5 screenYmaxQ5 = screenYminQ5;

    if (index > 0)
    {
        screenXminQ5 = indexToScreenXQ5(graph, index - 1);
        const CWRUtil::Q5 screenYnewQ5 = indexToScreenYQ5(graph, index - 1);
        screenYminQ5 = MIN(screenYminQ5, screenYnewQ5);
        screenYmaxQ5 = MAX(screenYmaxQ5, screenYnewQ5);
    }
    if (index < getGraph()->getUsedCapacity() - 1)
    {
        screenXmaxQ5 = indexToScreenXQ5(graph, index + 1);
        const CWRUtil::Q5 screenYnewQ5 = indexToScreenYQ5(graph, index + 1);
        screenYminQ5 = MIN(screenYminQ5, screenYnewQ5);
        screenYmaxQ5 = MAX(screenYmaxQ5, screenYnewQ5);
    }
    screenYminQ5 = MIN(screenYminQ5, screenYbaseQ5);
    screenYmaxQ5 = MAX(screenYmaxQ5, screenYbaseQ5);
    Rect dirty(rectFromQ5Coordinates(screenXminQ5, screenYminQ5, screenXmaxQ5, screenYmaxQ5));
    dirty = dirty & Rect(0, graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidthIncludingPadding(), graph->getGraphAreaHeight());
    invalidateRect(dirty);
}

GraphElementLine::GraphElementLine()
    : lineWidth(2)
{
}

void GraphElementLine::setLineWidth(uint8_t width)
{
    lineWidth = width;
}

uint8_t GraphElementLine::getLineWidth() const
{
    return lineWidth;
}

bool GraphElementLine::drawCanvasWidget(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    if (graph->getUsedCapacity() <= 1)
    {
        return true; // Nothing to draw, everything is fine!
    }

    const CWRUtil::Q5 lineWidthQ5 = CWRUtil::toQ5(lineWidth);
    const uint16_t lineWidthHalf = CWRUtil::Q5(((int)lineWidthQ5 + 1) / 2).ceil();
    int16_t indexMin;
    int16_t indexMax;
    if (!xScreenRangeToIndexRange(invalidatedArea.x - lineWidthHalf, invalidatedArea.right() + lineWidthHalf, indexMin, indexMax))
    {
        return true; // Nothing to draw, everything is fine!
    }

    Rect invalidRect = Rect(0, graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidthIncludingPadding(), graph->getGraphAreaHeight()) & invalidatedArea;
    Canvas canvas(this, invalidRect);
    const int16_t gapIndex = graph->getGapBeforeIndex();
    if (gapIndex <= 0 || gapIndex <= indexMin || gapIndex > indexMax)
    {
        drawIndexRange(canvas, graph, indexMin, indexMax);
    }
    else
    {
        drawIndexRange(canvas, graph, indexMin, gapIndex - 1);
        drawIndexRange(canvas, graph, gapIndex, indexMax);
    }
    return canvas.render(graph->getAlpha());
}

void GraphElementLine::invalidateGraphPointAt(int16_t index)
{
    const AbstractDataGraph* graph = getGraph();
    CWRUtil::Q5 lineWidthQ5 = CWRUtil::toQ5(lineWidth);
    Rect dirty(rectAround(indexToScreenXQ5(graph, index), indexToScreenYQ5(graph, index), lineWidthQ5));
    if (index > 0)
    {
        Rect other(rectAround(indexToScreenXQ5(graph, index - 1), indexToScreenYQ5(graph, index - 1), lineWidthQ5));
        dirty.expandToFit(other);
    }
    if (index < graph->getUsedCapacity() - 1)
    {
        Rect other(rectAround(indexToScreenXQ5(graph, index + 1), indexToScreenYQ5(graph, index + 1), lineWidthQ5));
        dirty.expandToFit(other);
    }
    dirty = dirty & Rect(0, graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidthIncludingPadding(), graph->getGraphAreaHeight());
    invalidateRect(dirty);
}

void GraphElementLine::drawIndexRange(Canvas& canvas, const AbstractDataGraph* graph, int16_t indexMin, int16_t indexMax) const
{
    if (indexMin == indexMax)
    {
        return;
    }

    const CWRUtil::Q5 lineWidthQ5 = CWRUtil::toQ5(lineWidth);

    CWRUtil::Q5 screenXstartQ5 = roundQ5(indexToScreenXQ5(graph, indexMin));
    CWRUtil::Q5 screenYstartQ5 = roundQ5(indexToScreenYQ5(graph, indexMin));
    canvas.moveTo(screenXstartQ5, screenYstartQ5);
    int16_t index = indexMin;
    int16_t advance = 1;
    do
    {
        if (index == indexMax)
        {
            advance = -1;
        }
        index += advance;
        const CWRUtil::Q5 screenXendQ5 = roundQ5(indexToScreenXQ5(graph, index));
        const CWRUtil::Q5 screenYendQ5 = roundQ5(indexToScreenYQ5(graph, index));
        CWRUtil::Q5 dxQ5 = screenXendQ5 - screenXstartQ5;
        CWRUtil::Q5 dyQ5 = screenYendQ5 - screenYstartQ5;
        const CWRUtil::Q5 dQ5 = CWRUtil::sqrtQ10(dyQ5 * dyQ5 + dxQ5 * dxQ5);
        if (dQ5)
        {
            dyQ5 = CWRUtil::muldivQ5(lineWidthQ5, dyQ5, dQ5) / 2;
            dxQ5 = CWRUtil::muldivQ5(lineWidthQ5, dxQ5, dQ5) / 2;
            canvas.lineTo(screenXstartQ5 - dyQ5, screenYstartQ5 + dxQ5);
            canvas.lineTo(screenXendQ5 - dyQ5, screenYendQ5 + dxQ5);
            screenXstartQ5 = screenXendQ5;
            screenYstartQ5 = screenYendQ5;
        }
    } while (index > indexMin);
}

void GraphElementVerticalGapLine::setGapLineWidth(uint16_t width)
{
    lineWidth = width;
}

uint16_t GraphElementVerticalGapLine::getGapLineWidth() const
{
    return lineWidth;
}

void GraphElementVerticalGapLine::draw(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    const int16_t gapIndex = graph->getGapBeforeIndex();
    const uint8_t alpha = LCD::div255(getAlpha() * graph->getAlpha());
    if (gapIndex == 0 || alpha == 0)
    {
        return;
    }

    const int16_t screenXmin = indexToScreenXQ5(graph, gapIndex - 1).round();
    int16_t screenXmax = screenXmin + lineWidth;
    if (lineWidth == 0)
    {
        screenXmax = indexToScreenXQ5(graph, gapIndex).round();
    }
    Rect rect(screenXmin, graph->getGraphAreaPaddingTop(), screenXmax - screenXmin, graph->getGraphAreaHeight());
    normalizeRect(rect);
    rect = rect & invalidatedArea;
    if (!rect.isEmpty())
    {
        translateRectToAbsolute(rect);
        HAL::lcd().fillRect(rect, color, alpha);
    }
}

void GraphElementVerticalGapLine::invalidateGraphPointAt(int16_t /*index*/)
{
    const AbstractDataGraph* graph = getGraph();
    const int16_t gapIndex = graph->getGapBeforeIndex();
    if (gapIndex > 0)
    {
        const int16_t screenXmin = indexToScreenXQ5(graph, gapIndex - 1).round();
        int16_t screenXmax = screenXmin + lineWidth;
        if (lineWidth == 0)
        {
            screenXmax = indexToScreenXQ5(graph, gapIndex).round();
        }
        Rect dirty(screenXmin, graph->getGraphAreaPaddingTop(), screenXmax - screenXmin, graph->getGraphAreaHeight());
        normalizeRect(dirty);
        invalidateRect(dirty);
    }
}

GraphElementHistogram::GraphElementHistogram()
    : yBaseline(0),
      barWidth(2),
      barOffset(0)
{
}

void GraphElementHistogram::setBaselineScaled(int baseline)
{
    yBaseline = baseline;
    invalidate();
}

void GraphElementHistogram::setBaseline(int baseline)
{
    setBaselineScaled(baseline * dataScale);
}

void GraphElementHistogram::setBaseline(float baseline)
{
    setBaselineScaled(AbstractDataGraph::float2scaled(baseline, dataScale));
}

int GraphElementHistogram::getBaselineScaled() const
{
    return yBaseline;
}

int GraphElementHistogram::getBaselineAsInt() const
{
    return AbstractDataGraph::scaled2int(getBaselineScaled(), dataScale);
}

float GraphElementHistogram::getBaselineAsFloat() const
{
    return AbstractDataGraph::scaled2float(getBaselineScaled(), dataScale);
}

void GraphElementHistogram::setBarWidth(uint16_t width)
{
    barWidth = width;
}

uint16_t GraphElementHistogram::getBarWidth() const
{
    return barWidth;
}

void GraphElementHistogram::setBarOffset(int16_t offset)
{
    barOffset = offset;
}

int16_t GraphElementHistogram::getBarOffset() const
{
    return barOffset;
}

void GraphElementHistogram::draw(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    const uint8_t alpha = LCD::div255(getAlpha() * graph->getAlpha());
    if (graph->getUsedCapacity() == 0 || barWidth == 0 || alpha == 0)
    {
        return; // Nothing to draw, everything is fine!
    }

    const int baseline = convertToGraphScale(graph, yBaseline, dataScale);
    const CWRUtil::Q5 barOffsetQ5 = CWRUtil::toQ5(barOffset);
    const CWRUtil::Q5 barWidthQ5 = CWRUtil::toQ5(barWidth);
    const CWRUtil::Q5 barWidthHalfQ5 = CWRUtil::Q5(((int)barWidthQ5 + 1) / 2);
    const int16_t barWidthHalf = barWidthHalfQ5.ceil();
    const int16_t screenYzero = valueToScreenYQ5(graph, baseline).round();
    int16_t indexLow;
    int16_t indexHigh;
    if (!xScreenRangeToIndexRange(invalidatedArea.x + barOffset - barWidthHalf, invalidatedArea.right() + barOffset + barWidthHalf, indexLow, indexHigh))
    {
        return; // Nothing to draw, everything is fine!
    }

    Rect invalidRect = Rect(0, graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidthIncludingPadding(), graph->getGraphAreaHeight()) & invalidatedArea;
    for (int16_t index = indexLow; index <= indexHigh; index++)
    {
        const int16_t screenX = (indexToScreenXQ5(graph, index) + barOffsetQ5 - barWidthHalfQ5).round();
        const int16_t screenY = indexToScreenYQ5(graph, index).round();
        Rect rect(screenX, screenY, barWidth, screenYzero - screenY);
        normalizeRect(rect);
        rect = rect & invalidRect;
        if (!rect.isEmpty())
        {
            translateRectToAbsolute(rect);
            HAL::lcd().fillRect(rect, color, alpha);
        }
    }
}

void GraphElementHistogram::invalidateGraphPointAt(int16_t index)
{
    const AbstractDataGraph* graph = getGraph();
    const CWRUtil::Q5 screenXQ5 = indexToScreenXQ5(graph, index);
    const CWRUtil::Q5 barWidthHalfQ5 = CWRUtil::Q5((int)CWRUtil::toQ5(barWidth) + 1) / 2;
    const CWRUtil::Q5 barOffsetQ5 = CWRUtil::toQ5(barOffset);
    const int baseline = convertToGraphScale(graph, yBaseline, dataScale);
    Rect dirty(rectFromQ5Coordinates(screenXQ5 + barOffsetQ5 - barWidthHalfQ5, indexToScreenYQ5(graph, index), screenXQ5 + barOffsetQ5 + barWidthHalfQ5, valueToScreenYQ5(graph, baseline)));
    dirty = dirty & Rect(0, graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidthIncludingPadding(), graph->getGraphAreaHeight());
    invalidateRect(dirty);
}

GraphElementBoxes::GraphElementBoxes()
    : boxWidth(2)
{
}

void GraphElementBoxes::setBoxWidth(uint16_t width)
{
    boxWidth = width;
}

uint16_t GraphElementBoxes::getBoxWidth() const
{
    return boxWidth;
}

void GraphElementBoxes::draw(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    const uint8_t alpha = LCD::div255(getAlpha() * graph->getAlpha());
    if (graph->getUsedCapacity() == 0 || boxWidth == 0 || alpha == 0)
    {
        return; // Nothing to draw, everything is fine!
    }

    const CWRUtil::Q5 boxWidthQ5 = CWRUtil::toQ5(boxWidth);
    const CWRUtil::Q5 boxWidthHalfQ5 = boxWidthQ5 / 2;
    int16_t indexLow;
    int16_t indexHigh;
    const uint16_t boxWidthHalf = CWRUtil::Q5(((int)boxWidthQ5 + 1) / 2).ceil();
    if (!xScreenRangeToIndexRange(invalidatedArea.x - boxWidthHalf, invalidatedArea.right() + boxWidthHalf, indexLow, indexHigh))
    {
        return; // Nothing to draw, everything is fine!
    }

    for (int16_t index = indexLow; index <= indexHigh; index++)
    {
        if (isCenterInvisible(graph, index))
        {
            continue;
        }
        const CWRUtil::Q5 screenXQ5 = indexToScreenXQ5(graph, index);
        const CWRUtil::Q5 screenYQ5 = indexToScreenYQ5(graph, index);
        Rect rect((screenXQ5 - boxWidthHalfQ5).round(), (screenYQ5 - boxWidthHalfQ5).round(), boxWidth, boxWidth);
        rect = rect & invalidatedArea;
        if (!rect.isEmpty())
        {
            translateRectToAbsolute(rect);
            HAL::lcd().fillRect(rect, color, alpha);
        }
    }
}

void GraphElementBoxes::invalidateGraphPointAt(int16_t index)
{
    const AbstractDataGraph* graph = getGraph();
    if (isCenterInvisible(graph, index))
    {
        return;
    }
    const CWRUtil::Q5 boxWidthQ5 = CWRUtil::toQ5(boxWidth);
    const CWRUtil::Q5 boxWidthHalfQ5 = boxWidthQ5 / 2;
    const CWRUtil::Q5 screenXQ5 = indexToScreenXQ5(graph, index);
    const CWRUtil::Q5 screenYQ5 = indexToScreenYQ5(graph, index);
    Rect dirty((screenXQ5 - boxWidthHalfQ5).round(), (screenYQ5 - boxWidthHalfQ5).round(), boxWidth, boxWidth);
    invalidateRect(dirty);
}

GraphElementDots::GraphElementDots()
    : dotWidth(2)
{
}

void GraphElementDots::setDotWidth(uint8_t width)
{
    dotWidth = width;
}

uint8_t GraphElementDots::getDotWidth() const
{
    return dotWidth;
}

bool GraphElementDots::drawCanvasWidget(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    if (graph->getUsedCapacity() == 0)
    {
        return true; // Nothing to draw, everything is fine!
    }

    const CWRUtil::Q5 dotWidthQ5 = CWRUtil::toQ5(dotWidth);
    const CWRUtil::Q5 dotWidth3Q5 = CWRUtil::muldivQ5(dotWidthQ5, CWRUtil::toQ5(3), CWRUtil::toQ5(10));
    const CWRUtil::Q5 dotWidth4Q5 = CWRUtil::muldivQ5(dotWidthQ5, CWRUtil::toQ5(4), CWRUtil::toQ5(10));
    const CWRUtil::Q5 dotWidth5Q5 = CWRUtil::muldivQ5(dotWidthQ5, CWRUtil::toQ5(5), CWRUtil::toQ5(26));
    const CWRUtil::Q5 dotWidth12Q5 = CWRUtil::muldivQ5(dotWidthQ5, CWRUtil::toQ5(12), CWRUtil::toQ5(26));
    const CWRUtil::Q5 dotWidth2Q5 = CWRUtil::muldivQ5(dotWidthQ5, CWRUtil::toQ5(1), CWRUtil::toQ5(2));
    const uint16_t dotWidthHalf = CWRUtil::Q5(((int)dotWidthQ5 + 1) / 2).ceil(); // Round up
    int16_t indexLow;
    int16_t indexHigh;
    if (!xScreenRangeToIndexRange(invalidatedArea.x - dotWidthHalf, invalidatedArea.right() + dotWidthHalf, indexLow, indexHigh))
    {
        return true;
    }

    const bool bigDots = (dotWidth > 6);
    Canvas canvas(this, invalidatedArea);
    for (int16_t index = indexLow; index <= indexHigh; index++)
    {
        if (isCenterInvisible(graph, index))
        {
            continue;
        }
        const CWRUtil::Q5 screenXcenterQ5 = roundQ5(indexToScreenXQ5(graph, index));
        const CWRUtil::Q5 screenYcenterQ5 = roundQ5(indexToScreenYQ5(graph, index));
        Rect dirty(rectAround(screenXcenterQ5, screenYcenterQ5, dotWidthQ5) & invalidatedArea);
        if (!dirty.isEmpty())
        {
            if (bigDots)
            {
                canvas.moveTo(screenXcenterQ5 - dotWidth2Q5, screenYcenterQ5);
                canvas.lineTo(screenXcenterQ5 - dotWidth12Q5, screenYcenterQ5 - dotWidth5Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth4Q5, screenYcenterQ5 - dotWidth3Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth3Q5, screenYcenterQ5 - dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth5Q5, screenYcenterQ5 - dotWidth12Q5);
                canvas.lineTo(screenXcenterQ5, screenYcenterQ5 - dotWidth2Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth5Q5, screenYcenterQ5 - dotWidth12Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth3Q5, screenYcenterQ5 - dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth4Q5, screenYcenterQ5 - dotWidth3Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth12Q5, screenYcenterQ5 - dotWidth5Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth2Q5, screenYcenterQ5);
                canvas.lineTo(screenXcenterQ5 + dotWidth12Q5, screenYcenterQ5 + dotWidth5Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth4Q5, screenYcenterQ5 + dotWidth3Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth3Q5, screenYcenterQ5 + dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth5Q5, screenYcenterQ5 + dotWidth12Q5);
                canvas.lineTo(screenXcenterQ5, screenYcenterQ5 + dotWidth2Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth5Q5, screenYcenterQ5 + dotWidth12Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth3Q5, screenYcenterQ5 + dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth4Q5, screenYcenterQ5 + dotWidth3Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth12Q5, screenYcenterQ5 + dotWidth5Q5);
            }
            else
            {
                canvas.moveTo(screenXcenterQ5 - dotWidth2Q5, screenYcenterQ5);
                canvas.lineTo(screenXcenterQ5 - dotWidth4Q5, screenYcenterQ5 - dotWidth3Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth3Q5, screenYcenterQ5 - dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5, screenYcenterQ5 - dotWidth2Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth3Q5, screenYcenterQ5 - dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth4Q5, screenYcenterQ5 - dotWidth3Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth2Q5, screenYcenterQ5);
                canvas.lineTo(screenXcenterQ5 + dotWidth4Q5, screenYcenterQ5 + dotWidth3Q5);
                canvas.lineTo(screenXcenterQ5 + dotWidth3Q5, screenYcenterQ5 + dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5, screenYcenterQ5 + dotWidth2Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth3Q5, screenYcenterQ5 + dotWidth4Q5);
                canvas.lineTo(screenXcenterQ5 - dotWidth4Q5, screenYcenterQ5 + dotWidth3Q5);
            }
        }
    }
    return canvas.render(graph->getAlpha());
}

void GraphElementDots::invalidateGraphPointAt(int16_t index)
{
    const AbstractDataGraph* graph = getGraph();
    if (isCenterInvisible(graph, index))
    {
        return;
    }
    Rect dirty(rectAround(indexToScreenXQ5(graph, index), indexToScreenYQ5(graph, index), CWRUtil::toQ5(dotWidth)));
    dirty = dirty & Rect(0, graph->getGraphAreaPaddingTop(), graph->getGraphAreaWidthIncludingPadding(), graph->getGraphAreaHeight());
    invalidateRect(dirty);
}

GraphElementDiamonds::GraphElementDiamonds()
    : diamondWidth(2)
{
}

void GraphElementDiamonds::setDiamondWidth(uint8_t width)
{
    diamondWidth = width;
}

uint8_t GraphElementDiamonds::getDiamondWidth() const
{
    return diamondWidth;
}

bool GraphElementDiamonds::drawCanvasWidget(const Rect& invalidatedArea) const
{
    const AbstractDataGraph* graph = getGraph();
    if (graph->getUsedCapacity() == 0)
    {
        return true; // Nothing to draw, everything is fine!
    }

    const CWRUtil::Q5 diamondWidthQ5 = CWRUtil::toQ5(diamondWidth);
    const CWRUtil::Q5 dotWidthHalfQ5 = diamondWidthQ5 / 2;
    const uint16_t dotWidthHalf = CWRUtil::Q5(((int)diamondWidthQ5 + 1) / 2).ceil(); // Round up
    int16_t indexLow;
    int16_t indexHigh;
    if (!xScreenRangeToIndexRange(invalidatedArea.x - dotWidthHalf, invalidatedArea.right() + dotWidthHalf, indexLow, indexHigh))
    {
        return true;
    }

    Canvas canvas(this, invalidatedArea);
    for (int16_t index = indexLow; index <= indexHigh; index++)
    {
        if (isCenterInvisible(graph, index))
        {
            continue;
        }
        const CWRUtil::Q5 screenXcenterQ5 = roundQ5(indexToScreenXQ5(graph, index));
        const CWRUtil::Q5 screenYcenterQ5 = roundQ5(indexToScreenYQ5(graph, index));
        const Rect dirty(rectAround(screenXcenterQ5, screenYcenterQ5, diamondWidthQ5) & invalidatedArea);
        if (!dirty.isEmpty())
        {
            canvas.moveTo(screenXcenterQ5 - dotWidthHalfQ5, screenYcenterQ5);
            canvas.lineTo(screenXcenterQ5, screenYcenterQ5 - dotWidthHalfQ5);
            canvas.lineTo(screenXcenterQ5 + dotWidthHalfQ5, screenYcenterQ5);
            canvas.lineTo(screenXcenterQ5, screenYcenterQ5 + dotWidthHalfQ5);
        }
    }
    return canvas.render(graph->getAlpha());
}

void GraphElementDiamonds::invalidateGraphPointAt(int16_t index)
{
    const AbstractDataGraph* graph = getGraph();
    if (isCenterInvisible(graph, index))
    {
        return;
    }
    Rect dirty(rectAround(indexToScreenXQ5(graph, index), indexToScreenYQ5(graph, index), CWRUtil::toQ5(diamondWidth)));
    invalidateRect(dirty);
}

} // namespace touchgfx
