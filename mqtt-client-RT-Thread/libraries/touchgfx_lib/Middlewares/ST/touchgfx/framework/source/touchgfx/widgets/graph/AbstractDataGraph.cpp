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

#include <touchgfx/widgets/graph/AbstractDataGraph.hpp>
#include <touchgfx/widgets/graph/GraphElements.hpp>
#include <touchgfx/widgets/graph/GraphLabels.hpp>

namespace touchgfx
{
AbstractDataGraph::AbstractDataGraph(int16_t capacity)
    : dataScale(1), alpha(255), topPadding(0), leftPadding(0), rightPadding(0), bottomPadding(0),
      maxCapacity(capacity), usedCapacity(0), gapBeforeIndex(0), clickAction()
{
    add(graphArea);
    add(topArea);
    add(leftArea);
    add(rightArea);
    add(bottomArea);
    // Place all areas properly:
    setGraphAreaMargin(0, 0, 0, 0);
    setTouchable(true);
}

void AbstractDataGraph::setScale(int scale)
{
    dataScale = scale;
}

int AbstractDataGraph::getScale() const
{
    return dataScale;
}

void AbstractDataGraph::setAlpha(uint8_t newAlpha)
{
    alpha = newAlpha;
}

uint8_t AbstractDataGraph::getAlpha() const
{
    return alpha;
}

void AbstractDataGraph::setWidth(int16_t width)
{
    topArea.setWidth(width);
    bottomArea.setWidth(width);
    rightArea.setX(width - rightArea.getWidth());
    graphArea.setWidth(width - (leftArea.getWidth() + rightArea.getWidth()));
    Container::setWidth(width);
    updateAreasPosition();
}

void AbstractDataGraph::setHeight(int16_t height)
{
    leftArea.setHeight(height);
    rightArea.setHeight(height);
    bottomArea.setY(height - bottomArea.getHeight());
    graphArea.setHeight(height - (topArea.getHeight() + bottomArea.getHeight()));
    Container::setHeight(height);
    updateAreasPosition();
}

void AbstractDataGraph::setGraphAreaMargin(int16_t top, int16_t left, int16_t right, int16_t bottom)
{
    graphArea.setPosition(left, top, getWidth() - (left + right), getHeight() - (top + bottom));
    topArea.setPosition(0, 0, getWidth(), top);
    leftArea.setPosition(0, 0, left, getHeight());
    rightArea.setPosition(getWidth() - right, 0, right, getHeight());
    bottomArea.setPosition(0, getHeight() - bottom, getWidth(), bottom);
    updateAreasPosition();
}

int16_t AbstractDataGraph::getGraphAreaMarginTop() const
{
    return topArea.getHeight();
}

int16_t AbstractDataGraph::getGraphAreaMarginLeft() const
{
    return leftArea.getWidth();
}

int16_t AbstractDataGraph::getGraphAreaMarginRight() const
{
    return rightArea.getWidth();
}

int16_t AbstractDataGraph::getGraphAreaMarginBottom() const
{
    return bottomArea.getHeight();
}

void AbstractDataGraph::setGraphAreaPadding(int16_t top, int16_t left, int16_t right, int16_t bottom)
{
    topPadding = top;
    leftPadding = left;
    rightPadding = right;
    bottomPadding = bottom;
}

int16_t AbstractDataGraph::getGraphAreaPaddingTop() const
{
    return topPadding;
}

int16_t AbstractDataGraph::getGraphAreaPaddingLeft() const
{
    return leftPadding;
}

int16_t AbstractDataGraph::getGraphAeraPaddingRight() const
{
    return rightPadding;
}

int16_t AbstractDataGraph::getGraphAreaPaddingBottom() const
{
    return bottomPadding;
}

int16_t AbstractDataGraph::getGraphAreaWidth() const
{
    return graphArea.getWidth() - (leftPadding + rightPadding);
}

int16_t AbstractDataGraph::getGraphAreaWidthIncludingPadding() const
{
    return graphArea.getWidth();
}

int16_t AbstractDataGraph::getGraphAreaHeight() const
{
    return graphArea.getHeight() - (topPadding + bottomPadding);
}

int16_t AbstractDataGraph::getGraphAreaHeightIncludingPadding() const
{
    return graphArea.getHeight();
}

void AbstractDataGraph::setGraphRange(int xMin, int xMax, int yMin, int yMax)
{
    setGraphRangeX(xMin, xMax);
    setGraphRangeY(yMin, yMax);
}

void AbstractDataGraph::setGraphRange(int xMin, int xMax, float yMin, float yMax)
{
    setGraphRangeX(xMin, xMax);
    setGraphRangeY(yMin, yMax);
}

void AbstractDataGraph::clear()
{
    usedCapacity = 0;
}

void AbstractDataGraph::setGapBeforeIndex(int16_t index)
{
    gapBeforeIndex = index;
}

int16_t AbstractDataGraph::getGapBeforeIndex() const
{
    return gapBeforeIndex;
}

void AbstractDataGraph::addGraphElement(AbstractGraphElement& d)
{
    graphArea.add(d);
    d.setPosition(0, 0, graphArea.getWidth(), graphArea.getHeight());
}

void AbstractDataGraph::addTopElement(AbstractGraphDecoration& d)
{
    topArea.add(d);
    d.setPosition(0, 0, topArea.getWidth(), topArea.getHeight());
}

void AbstractDataGraph::addLeftElement(AbstractGraphDecoration& d)
{
    leftArea.add(d);
    d.setPosition(0, 0, leftArea.getWidth(), leftArea.getHeight());
}

void AbstractDataGraph::addRightElement(AbstractGraphDecoration& d)
{
    rightArea.add(d);
    d.setPosition(0, 0, rightArea.getWidth(), rightArea.getHeight());
}

void AbstractDataGraph::addBottomElement(AbstractGraphDecoration& d)
{
    bottomArea.add(d);
    d.setPosition(0, 0, bottomArea.getWidth(), bottomArea.getHeight());
}

int16_t AbstractDataGraph::getUsedCapacity() const
{
    return usedCapacity;
}

int16_t AbstractDataGraph::getMaxCapacity() const
{
    return maxCapacity;
}

bool AbstractDataGraph::getNearestIndexForScreenXY(int16_t x, int16_t y, int16_t& index)
{
    if (usedCapacity == 0)
    {
        return false;
    }
    int32_t bestDist = 0x7FFFFFFF;
    for (int16_t ix = 0; ix < usedCapacity; ix++)
    {
        const int16_t xDist = abs(indexToScreenX(ix) - x);
        const int16_t yDist = abs(indexToScreenY(ix) - y);
        const int32_t dist = xDist * xDist + yDist * yDist;
        if (dist < bestDist)
        {
            index = ix;
            bestDist = dist;
        }
    }
    return true;
}

bool AbstractDataGraph::getNearestIndexForScreenX(int16_t x, int16_t& index) const
{
    if (usedCapacity == 0)
    {
        return false;
    }
    int16_t indexLow;
    int16_t indexHigh;
    xScreenRangeToIndexRange(x, x, indexLow, indexHigh);
    const int16_t xLow = indexToScreenX(indexLow);
    const int16_t xHigh = indexToScreenX(indexHigh);
    index = abs(xLow - x) <= abs(xHigh - x) ? indexLow : indexHigh;
    return true;
}

int16_t AbstractDataGraph::indexToScreenX(int16_t index) const
{
    return indexToScreenXQ5(index).round();
}

int16_t AbstractDataGraph::indexToScreenY(int16_t index) const
{
    return indexToScreenYQ5(index).round();
}

int AbstractDataGraph::indexToDataPointXAsInt(int16_t index) const
{
    return scaled2int(indexToDataPointXScaled(index));
}

float AbstractDataGraph::indexToDataPointXAsFloat(int16_t index) const
{
    return scaled2float(indexToDataPointXScaled(index));
}

int AbstractDataGraph::indexToDataPointYAsInt(int16_t index) const
{
    return scaled2int(indexToDataPointYScaled(index));
}

float AbstractDataGraph::indexToDataPointYAsFloat(int16_t index) const
{
    return scaled2float(indexToDataPointYScaled(index));
}

int32_t AbstractDataGraph::indexToGlobalIndex(int16_t index) const
{
    return (int32_t)index;
}

void AbstractDataGraph::setClickAction(GenericCallback<const AbstractDataGraph&, const GraphClickEvent&>& callback)
{
    clickAction = &callback;
}

void AbstractDataGraph::setDragAction(GenericCallback<const AbstractDataGraph&, const GraphDragEvent&>& callback)
{
    dragAction = &callback;
}

void AbstractDataGraph::handleClickEvent(const ClickEvent& event)
{
    if (event.getType() == ClickEvent::CANCEL)
    {
        return;
    }
    const int16_t x = event.getX() - graphArea.getX();
    int16_t index;
    if (getNearestIndexForScreenX(x, index))
    {
        if (event.getType() == ClickEvent::PRESSED || event.getType() == ClickEvent::RELEASED)
        {
            if (clickAction && clickAction->isValid())
            {
                GraphClickEvent graphClickEvent(index, event);
                clickAction->execute(*this, graphClickEvent);
            }
        }
    }
}

void AbstractDataGraph::handleDragEvent(const DragEvent& event)
{
    const int16_t x = event.getNewX() - graphArea.getX();
    int16_t index;
    if (getNearestIndexForScreenX(x, index))
    {
        if (event.getType() == DragEvent::DRAGGED)
        {
            if (dragAction && dragAction->isValid())
            {
                GraphDragEvent graphDragEvent(index, event);
                dragAction->execute(*this, graphDragEvent);
            }
        }
    }
}

void AbstractDataGraph::invalidateGraphPointAt(int16_t index)
{
    if (index < usedCapacity)
    {
        AbstractGraphElement* d = (AbstractGraphElement*)graphArea.getFirstChild();
        while (d)
        {
            d->invalidateGraphPointAt(index);
            d = (AbstractGraphElement*)d->getNextSibling();
        }
    }
}

void AbstractDataGraph::invalidateGraphArea()
{
    graphArea.invalidate();
}

void AbstractDataGraph::invalidateXAxisPointAt(int16_t index)
{
    AbstractGraphElement* d = (AbstractGraphElement*)topArea.getFirstChild();
    while (d)
    {
        d->invalidateGraphPointAt(index);
        d = (AbstractGraphElement*)d->getNextSibling();
    }
    d = (AbstractGraphElement*)bottomArea.getFirstChild();
    while (d)
    {
        d->invalidateGraphPointAt(index);
        d = (AbstractGraphElement*)d->getNextSibling();
    }
}

void AbstractDataGraph::invalidateAllXAxisPoints()
{
    int min = getGraphRangeXMin();
    int max = getGraphRangeXMax();
    if (max < min)
    {
        const int16_t tmp = min;
        min = max;
        max = tmp;
    }
    for (int index = min; index <= max; index++)
    {
        invalidateXAxisPointAt(index);
    }
}

void AbstractDataGraph::updateAreasPosition()
{
    Drawable* d = graphArea.getFirstChild();
    while (d)
    {
        d->setPosition(0, 0, graphArea.getWidth(), graphArea.getHeight());
        d = d->getNextSibling();
    }

    d = topArea.getFirstChild();
    while (d)
    {
        d->setPosition(0, 0, topArea.getWidth(), topArea.getHeight());
        d = d->getNextSibling();
    }

    d = leftArea.getFirstChild();
    while (d)
    {
        d->setPosition(0, 0, leftArea.getWidth(), leftArea.getHeight());
        d = d->getNextSibling();
    }

    d = rightArea.getFirstChild();
    while (d)
    {
        d->setPosition(0, 0, rightArea.getWidth(), rightArea.getHeight());
        d = d->getNextSibling();
    }

    d = bottomArea.getFirstChild();
    while (d)
    {
        d->setPosition(0, 0, bottomArea.getWidth(), bottomArea.getHeight());
        d = d->getNextSibling();
    }

    invalidate();
}

void AbstractDataGraph::setGraphRangeScaled(int xMin, int xMax, int yMin, int yMax)
{
    setGraphRangeX(xMin, xMax);
    setGraphRangeYScaled(yMin, yMax);
}

int AbstractDataGraph::convertToGraphScale(int value, int scale) const
{
    if (scale == dataScale)
    {
        return value;
    }
    return muldiv(value, dataScale, scale);
}

int AbstractDataGraph::getXAxisScaleScaled() const
{
    return dataScale;
}

int AbstractDataGraph::getXAxisOffsetScaled() const
{
    return 0;
}

AbstractDataGraphWithY::AbstractDataGraphWithY(int16_t capacity, int* values)
    : AbstractDataGraph(capacity), yValues(values), dataCounter(0), xOffset(0), xScale(1)
{
    assert(capacity > 0);
    setGraphRangeX(0, capacity - 1);
}

int16_t AbstractDataGraphWithY::addDataPoint(int y)
{
    return addDataPointScaled(y * dataScale);
}

int16_t AbstractDataGraphWithY::addDataPoint(float y)
{
    return addDataPointScaled(float2scaled(y));
}

void AbstractDataGraphWithY::setGraphRangeX(int min, int max)
{
    assert(min != max);
    if (max < min)
    {
        const int tmp = min;
        min = max;
        max = tmp;
    }
    if (min != graphRangeMinX || max != graphRangeMaxX)
    {
        graphRangeMinX = min;
        graphRangeMaxX = max;
        topArea.invalidate();
        graphArea.invalidate();
        bottomArea.invalidate();
    }
}

void AbstractDataGraphWithY::setGraphRangeY(int min, int max)
{
    setGraphRangeYScaled(int2scaled(min), int2scaled(max));
}

void AbstractDataGraphWithY::setGraphRangeY(float min, float max)
{
    setGraphRangeYScaled(float2scaled(min), float2scaled(max));
}

int AbstractDataGraphWithY::getGraphRangeXMin() const
{
    return graphRangeMinX;
}

int AbstractDataGraphWithY::getGraphRangeXMax() const
{
    return graphRangeMaxX;
}

int AbstractDataGraphWithY::getGraphRangeYMinAsInt() const
{
    return scaled2int(graphRangeMinY);
}

float AbstractDataGraphWithY::getGraphRangeYMinAsFloat() const
{
    return scaled2float(graphRangeMinY);
}

int AbstractDataGraphWithY::getGraphRangeYMaxAsInt() const
{
    return scaled2int(graphRangeMaxY);
}

float AbstractDataGraphWithY::getGraphRangeYMaxAsFloat() const
{
    return scaled2float(graphRangeMaxY);
}

void AbstractDataGraphWithY::setGraphRangeYAuto(bool showXaxis, int margin)
{
    if (usedCapacity > 0)
    {
        int loX = getGraphRangeXMin();
        int hiX = getGraphRangeXMax();
        if (loX > hiX)
        {
            const int16_t tmp = loX;
            loX = hiX;
            hiX = tmp;
        }
        loX = MAX(loX, 0);
        hiX = MIN(hiX, usedCapacity);
        if (loX < usedCapacity && hiX >= 0)
        {
            int loYnew = showXaxis ? margin : yValues[loX];
            int hiYnew = showXaxis ? -margin : yValues[loX];
            for (int16_t i = loX; i < hiX; i++)
            {
                int y = yValues[i];
                if (loYnew > y)
                {
                    loYnew = y;
                }
                if (hiYnew < y)
                {
                    hiYnew = y;
                }
            }
            loYnew -= margin;
            hiYnew += margin;
            if (loYnew != hiYnew)
            {
                setGraphRangeYScaled(loYnew, hiYnew);
            }
        }
    }
}

void AbstractDataGraphWithY::setScale(int scale)
{
    const int oldScale = getScale();
    AbstractDataGraph::setScale(scale);
    xScale = convertToGraphScale(xScale, oldScale);
    xOffset = convertToGraphScale(xOffset, oldScale);
}

void AbstractDataGraphWithY::setXAxisScale(int scale)
{
    setXAxisScaleScaled(scale * dataScale);
}

void AbstractDataGraphWithY::setXAxisScale(float scale)
{
    setXAxisScaleScaled(float2scaled(scale));
}

int AbstractDataGraphWithY::getXAxisScaleAsInt() const
{
    return scaled2int(getXAxisScaleScaled());
}

float AbstractDataGraphWithY::getXAxisScaleAsFloat() const
{
    return scaled2float(getXAxisScaleScaled());
}

void AbstractDataGraphWithY::setXAxisOffset(int offset)
{
    setXAxisOffsetScaled(offset * dataScale);
}

void AbstractDataGraphWithY::setXAxisOffset(float offset)
{
    setXAxisOffsetScaled(float2scaled(offset));
}

int AbstractDataGraphWithY::getXAxisOffsetAsInt() const
{
    return scaled2int(getXAxisOffsetScaled());
}

float AbstractDataGraphWithY::getXAxisOffsetAsFloat() const
{
    return scaled2float(getXAxisOffsetScaled());
}

int16_t AbstractDataGraphWithY::addDataPointScaled(int y)
{
    beforeAddValue();
    dataCounter++;
    return addValue(y);
}

void AbstractDataGraphWithY::beforeAddValue()
{
}

int16_t AbstractDataGraphWithY::realIndex(int16_t index) const
{
    return index;
}

int AbstractDataGraphWithY::indexToDataPointXScaled(int16_t index) const
{
    return (indexToGlobalIndex(index) * getXAxisScaleScaled()) + getXAxisOffsetScaled();
}

int AbstractDataGraphWithY::indexToDataPointYScaled(int16_t index) const
{
    return yValues[realIndex(index)];
}

bool AbstractDataGraphWithY::xScreenRangeToIndexRange(int16_t xLo, int16_t xHi, int16_t& indexLow, int16_t& indexHigh) const
{
    if (usedCapacity == 0)
    {
        indexLow = indexHigh = -1;
        return false;
    }
    if (getGraphAreaWidth() == 1)
    {
        indexLow = 0;
        indexHigh = usedCapacity - 1;
        return true;
    }
    CWRUtil::Q5 loQ5 = CWRUtil::muldivQ5(CWRUtil::toQ5(xLo - leftPadding), CWRUtil::Q5(graphRangeMaxX - graphRangeMinX), CWRUtil::Q5(getGraphAreaWidth() - 1)) + CWRUtil::toQ5(graphRangeMinX);
    CWRUtil::Q5 hiQ5 = CWRUtil::muldivQ5(CWRUtil::toQ5(xHi - leftPadding), CWRUtil::Q5(graphRangeMaxX - graphRangeMinX), CWRUtil::Q5(getGraphAreaWidth() - 1)) + CWRUtil::toQ5(graphRangeMinX);
    if (loQ5 > hiQ5)
    {
        const CWRUtil::Q5 tmp = loQ5;
        loQ5 = hiQ5;
        hiQ5 = tmp;
    }
    const int low = loQ5.to<int>();
    const int high = hiQ5.ceil();
    const int lowValid = 0;
    const int highValid = usedCapacity - 1;
    if (high < lowValid)
    {
        indexLow = indexHigh = lowValid;
        return false;
    }
    if (low > highValid)
    {
        indexLow = indexHigh = highValid;
        return false;
    }
    indexLow = MAX(lowValid, low);
    indexHigh = MIN(highValid, high);
    return true;
}

void AbstractDataGraphWithY::setXAxisScaleScaled(int scale)
{
    xScale = scale;
}

int AbstractDataGraphWithY::getXAxisScaleScaled() const
{
    return xScale;
}

void AbstractDataGraphWithY::setGraphRangeYScaled(int min, int max)
{
    assert(min != max);
    if (max < min)
    {
        const int tmp = min;
        min = max;
        max = tmp;
    }
    if (min != graphRangeMinY || max != graphRangeMaxY)
    {
        graphRangeMinY = min;
        graphRangeMaxY = max;
        leftArea.invalidate();
        graphArea.invalidate();
        rightArea.invalidate();
    }
}

int AbstractDataGraphWithY::getGraphRangeYMinScaled() const
{
    return graphRangeMinY;
}

int AbstractDataGraphWithY::getGraphRangeYMaxScaled() const
{
    return graphRangeMaxY;
}

void AbstractDataGraphWithY::setXAxisOffsetScaled(int offset)
{
    xOffset = offset;
}

int AbstractDataGraphWithY::getXAxisOffsetScaled() const
{
    return xOffset;
}

CWRUtil::Q5 AbstractDataGraphWithY::valueToScreenXQ5(int x) const
{
    return CWRUtil::muldiv_toQ5(x - graphRangeMinX, getGraphAreaWidth() - 1, graphRangeMaxX - graphRangeMinX) + CWRUtil::toQ5(leftPadding);
}

CWRUtil::Q5 AbstractDataGraphWithY::valueToScreenYQ5(int y) const
{
    return CWRUtil::toQ5(getGraphAreaHeight() + topPadding - 1) - CWRUtil::muldiv_toQ5(y - graphRangeMinY, getGraphAreaHeight() - 1, graphRangeMaxY - graphRangeMinY);
}

CWRUtil::Q5 AbstractDataGraphWithY::indexToScreenXQ5(int16_t index) const
{
    return valueToScreenXQ5(index);
}

CWRUtil::Q5 AbstractDataGraphWithY::indexToScreenYQ5(int16_t index) const
{
    return valueToScreenYQ5(yValues[realIndex(index)]);
}

} // namespace touchgfx
