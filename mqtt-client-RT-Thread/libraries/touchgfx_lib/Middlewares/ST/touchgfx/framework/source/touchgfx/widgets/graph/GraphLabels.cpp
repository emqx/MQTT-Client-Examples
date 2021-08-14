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

#include <touchgfx/widgets/graph/GraphLabels.hpp>

namespace touchgfx
{
GraphLabelsBase::GraphLabelsBase()
    : labelInterval(0), labelTypedText(TypedText(-1)), labelRotation(TEXT_ROTATE_0), labelDecimals(0), labelDecimalPoint('.'),
      majorLabel(0)
{
}

int GraphLabelsBase::getCorrectlyScaledMajorInterval(const AbstractDataGraph* graph) const
{
    return majorLabel == 0 ? 0 : convertToGraphScale(graph, majorLabel->getIntervalScaled(), majorLabel->getScale());
}

void GraphLabelsBase::setIntervalScaled(int interval)
{
    labelInterval = abs(interval);
}

void GraphLabelsBase::setInterval(int interval)
{
    setIntervalScaled(abs(interval) * dataScale);
}

void GraphLabelsBase::setInterval(float interval)
{
    setIntervalScaled(AbstractDataGraph::float2scaled(abs(interval), dataScale));
}

int GraphLabelsBase::getIntervalScaled() const
{
    return labelInterval;
}

int GraphLabelsBase::getIntervalAsInt() const
{
    return AbstractDataGraph::scaled2int(getIntervalScaled(), dataScale);
}

float GraphLabelsBase::getIntervalAsFloat() const
{
    return AbstractDataGraph::scaled2float(getIntervalScaled(), dataScale);
}

void GraphLabelsBase::setMajorLabel(const GraphLabelsBase& major)
{
    majorLabel = &major;
}

void GraphLabelsBase::setLabelTypedText(const TypedText& typedText)
{
    labelTypedText = typedText;
}

TypedText GraphLabelsBase::getLabelTypedText() const
{
    return labelTypedText;
}

void GraphLabelsBase::setLabelRotation(TextRotation rotation)
{
    labelRotation = rotation;
}

TextRotation GraphLabelsBase::getLabelRotation() const
{
    return labelRotation;
}

void GraphLabelsBase::setLabelDecimals(uint16_t decimals)
{
    labelDecimals = decimals;
}

uint16_t GraphLabelsBase::getLabelDecimals() const
{
    return labelDecimals;
}

void GraphLabelsBase::setLabelDecimalPoint(Unicode::UnicodeChar decimalPoint)
{
    labelDecimalPoint = decimalPoint;
}

Unicode::UnicodeChar GraphLabelsBase::getLabelDecimalPoint() const
{
    return labelDecimalPoint;
}

void GraphLabelsBase::invalidateGraphPointAt(int16_t index)
{
}

void GraphLabelsBase::formatLabel(Unicode::UnicodeChar* buffer, int16_t bufferSize, int label, int decimals, Unicode::UnicodeChar decimalPoint, int scale) const
{
    int length = 0;
    if (label < 0 && length < bufferSize - 1)
    {
        buffer[length++] = '-';
        label = -label;
    }
    if (decimals == 0)
    {
        Unicode::snprintf(buffer + length, bufferSize - length, "%d", (label + scale / 2) / scale);
    }
    else if (decimals > 0)
    {
        Unicode::snprintf(buffer + length, bufferSize - length, "%d", label / scale);
        int length = Unicode::strlen(buffer);
        if (length < bufferSize - 1)
        {
            buffer[length++] = decimalPoint;
            int32_t remainder = label % scale;
            for (int i = 0; i < decimals && length < bufferSize - 1; i++)
            {
                remainder *= 10;
                if (i == decimals - 1 || length == bufferSize - 1)
                {
                    remainder += scale / 2; // Rounding on the last (visible) digit
                }
                const int digit = (remainder / scale);
                buffer[length++] = (Unicode::UnicodeChar)('0' + digit);
                remainder %= scale;
            }
            buffer[length] = (Unicode::UnicodeChar)0;
        }
    }
}

void GraphLabelsX::draw(const Rect& invalidatedArea) const
{
    if (!labelTypedText.hasValidId())
    {
        return;
    }
    const Font* fontToDraw = labelTypedText.getFont();
    if (!fontToDraw)
    {
        return;
    }

    const AbstractDataGraph* graph = getGraph();
    const uint8_t alpha = LCD::div255(getAlpha() * graph->getAlpha());
    if (alpha == 0)
    {
        return;
    }

    const int minorInterval = getIntervalAsInt();
    const int majorInterval = (majorLabel == 0) ? 0 : majorLabel->getIntervalAsInt();

    if (minorInterval == 0 && majorInterval == 0)
    {
        drawString(invalidatedArea, fontToDraw, graph, 0, alpha);
    }
    else if (minorInterval > 0)
    {
        int rangeMin = graph->getGraphRangeXMin();
        int rangeMax = graph->getGraphRangeXMax();
        if (rangeMin > rangeMax)
        {
            const int tmp = rangeMin;
            rangeMin = rangeMax;
            rangeMax = tmp;
        }

        const int16_t gapIndex = graph->getGapBeforeIndex();
        if (gapIndex <= 0 || gapIndex <= rangeMin || gapIndex > rangeMax)
        {
            drawIndexRange(invalidatedArea, fontToDraw, graph, rangeMin, rangeMax, alpha);
        }
        else
        {
            drawIndexRange(invalidatedArea, fontToDraw, graph, rangeMin, (int)gapIndex - 1, alpha);
            drawIndexRange(invalidatedArea, fontToDraw, graph, (int)gapIndex, rangeMax, alpha);
        }
    }
}

void GraphLabelsX::invalidateGraphPointAt(int16_t index)
{
    if (!labelTypedText.hasValidId())
    {
        return;
    }
    const Font* fontToDraw = labelTypedText.getFont();
    if (!fontToDraw)
    {
        return;
    }

    const AbstractDataGraph* graph = getGraph();
    const uint8_t alpha = LCD::div255(getAlpha() * graph->getAlpha());
    if (alpha == 0)
    {
        return;
    }

    const int minorInterval = getIntervalAsInt();
    const int majorInterval = (majorLabel == 0) ? 0 : majorLabel->getIntervalAsInt();

    bool isOnMinor = (minorInterval > 0 && index == minorInterval * (int)(index / minorInterval));
    bool isOnMajor = (majorInterval > 0 && index == majorInterval * (int)(index / majorInterval));
    if ((majorInterval == 0 && minorInterval == 0 && index == 0) || (isOnMinor && !isOnMajor))
    {
        Unicode::UnicodeChar wildcard[20];
        formatLabel(wildcard, 20, (graph->indexToGlobalIndex((int)index) * getGraphXAxisScaleScaled(graph)) + getGraphXAxisOffsetScaled(graph), labelDecimals, labelDecimalPoint, graph->getScale());
        // Adjust to make label centered
        uint16_t labelWidth;
        if (labelRotation == TEXT_ROTATE_0 || labelRotation == TEXT_ROTATE_180)
        {
            labelWidth = fontToDraw->getStringWidth(labelTypedText.getText(), wildcard);
        }
        else
        {
            labelWidth = fontToDraw->getMaxTextHeight(labelTypedText.getText(), wildcard) * fontToDraw->getNumberOfLines(labelTypedText.getText(), wildcard) + fontToDraw->getSpacingAbove(labelTypedText.getText(), wildcard);
        }
        Rect dirty((graph->getGraphAreaMarginLeft() + valueToScreenXQ5(graph, index).round()) - labelWidth / 2, 0, labelWidth, getHeight());
        invalidateRect(dirty);
    }
}

void GraphLabelsX::drawIndexRange(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, int indexLow, int indexHigh, const uint8_t alpha) const
{
    if (indexLow > indexHigh)
    {
        const int tmp = indexLow;
        indexLow = indexHigh;
        indexHigh = tmp;
    }
    // Now indexHigh is higher than indexLow

    const int minorInterval = getIntervalAsInt();
    if (minorInterval > 0 && abs(indexHigh - indexLow) / minorInterval > 100)
    {
        return; // Too many labels
    }

    if (minorInterval == 0)
    {
        if ((0 >= indexLow && 0 <= indexHigh) || (0 >= indexHigh && 0 <= indexLow))
        {
            drawString(invalidatedArea, fontToDraw, graph, 0, alpha);
        }
        return;
    }
    const int majorInterval = (majorLabel == 0) ? 0 : majorLabel->getIntervalAsInt();

    const int minorLo = (int)(indexLow / minorInterval) - 1;
    const int minorHi = (int)(indexHigh / minorInterval) + 1;

    if (majorInterval == 0)
    {
        for (int minorIndex = minorLo; minorIndex != minorHi + 1; minorIndex++)
        {
            const int index = (int)(minorInterval * minorIndex);
            if ((index >= indexLow && index <= indexHigh) || (index >= indexHigh && index <= indexLow))
            {
                drawString(invalidatedArea, fontToDraw, graph, index, alpha);
            }
        }
    }
    else
    {
        const int majorLo = (int)(indexLow / majorInterval) - 1;
        const int majorHi = (int)(indexHigh / majorInterval) + 1;
        int majorIndex = majorLo;
        int32_t majorValue = majorInterval * majorLo;
        int32_t minorValue = minorInterval * minorLo;
        for (;;)
        {
            // Draw strings lines up to the major line
            while (minorValue < majorValue)
            {
                if ((minorValue >= indexLow && minorValue <= indexHigh) || (minorValue >= indexHigh && minorValue <= indexLow))
                {
                    drawString(invalidatedArea, fontToDraw, graph, minorValue, alpha);
                }
                minorValue += minorInterval;
            }
            // Advance minor past the major line we are about to draw
            while (minorValue <= majorValue)
            {
                minorValue += minorInterval;
            }
            if (majorValue < minorValue)
            {
                majorIndex++;
                if (majorIndex == majorHi + 1)
                {
                    break;
                }
                majorValue += majorInterval;
            }
        }
    }
}

void GraphLabelsX::drawString(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, int index, const uint8_t alpha) const
{
    const int16_t labelX = valueToScreenXQ5(graph, index).round() - graph->getGraphAreaPaddingLeft();
    if (labelX < 0 || labelX >= graph->getGraphAreaWidth())
    {
        return;
    }

    Unicode::UnicodeChar wildcard[20];
    formatLabel(wildcard, 20, (graph->indexToGlobalIndex(index) * getGraphXAxisScaleScaled(graph)) + getGraphXAxisOffsetScaled(graph), labelDecimals, labelDecimalPoint, graph->getScale());

    // Adjust to make label centered
    uint16_t labelWidth;
    if (labelRotation == TEXT_ROTATE_0 || labelRotation == TEXT_ROTATE_180)
    {
        labelWidth = fontToDraw->getStringWidth(labelTypedText.getText(), wildcard);
    }
    else
    {
        labelWidth = fontToDraw->getMaxTextHeight(labelTypedText.getText(), wildcard) * fontToDraw->getNumberOfLines(labelTypedText.getText(), wildcard) + fontToDraw->getSpacingAbove(labelTypedText.getText(), wildcard);
    }
    Rect labelRect((graph->getGraphAreaMarginLeft() + valueToScreenXQ5(graph, index).round()) - labelWidth / 2, 0, labelWidth, getHeight());

    Rect dirty = labelRect & invalidatedArea;
    if (!dirty.isEmpty())
    {
        dirty.x -= labelRect.x;
        dirty.y -= labelRect.y;
        translateRectToAbsolute(labelRect);
        LCD::StringVisuals visuals(fontToDraw, color, alpha, labelTypedText.getAlignment(), 0, labelRotation, labelTypedText.getTextDirection(), 0, WIDE_TEXT_NONE);
        HAL::lcd().drawString(labelRect, dirty, visuals, labelTypedText.getText(), wildcard, 0);
    }
}

void GraphLabelsY::draw(const Rect& invalidatedArea) const
{
    if (!labelTypedText.hasValidId())
    {
        return;
    }
    const Font* fontToDraw = labelTypedText.getFont();
    if (!fontToDraw)
    {
        return;
    }

    const AbstractDataGraph* graph = getGraph();
    const uint8_t alpha = LCD::div255(getAlpha() * graph->getAlpha());
    if (alpha == 0)
    {
        return;
    }

    const int minorInterval = convertToGraphScale(graph, labelInterval, dataScale);
    int majorInterval = getCorrectlyScaledMajorInterval(graph);

    if (majorInterval == 0 && minorInterval == 0)
    {
        drawString(invalidatedArea, fontToDraw, graph, 0, 0, alpha);
    }
    else if (minorInterval > 0)
    {
        int rangeMin = getGraphRangeYMinScaled(graph);
        int rangeMax = getGraphRangeYMaxScaled(graph);
        if (abs(rangeMax - rangeMin) / minorInterval > 100)
        {
            return; // Too many labels
        }

        if (rangeMin > rangeMax)
        {
            const int tmp = rangeMin;
            rangeMin = rangeMax;
            rangeMax = tmp;
        }
        // Now rangeMax is higher than rangeMin

        const int minorLo = (int)(rangeMin / minorInterval) - 1;
        const int minorHi = (int)(rangeMax / minorInterval) + 1;
        if (majorInterval == 0)
        {
            for (int minorIndex = minorLo; minorIndex != minorHi + 1; minorIndex++)
            {
                drawString(invalidatedArea, fontToDraw, graph, minorInterval * minorIndex, labelInterval * minorIndex, alpha);
            }
        }
        else
        {
            const int majorLo = (int)(rangeMin / majorInterval) - 1;
            const int majorHi = (int)(rangeMax / majorInterval) + 1;
            int majorIndex = majorLo;
            int32_t majorValue = majorInterval * majorIndex;
            int minorIndex = minorLo;
            int32_t minorValue = minorInterval * minorIndex;
            for (;;)
            {
                // Draw strings lines up to the major line
                while (minorValue < majorValue)
                {
                    drawString(invalidatedArea, fontToDraw, graph, minorValue, labelInterval * minorIndex, alpha);
                    minorIndex++;
                    minorValue += minorInterval;
                }
                // Advance minor past the major line we are about to draw
                while (minorValue <= majorValue)
                {
                    minorIndex++;
                    minorValue += minorInterval;
                }
                if (majorValue < minorValue)
                {
                    majorIndex++;
                    if (majorIndex == majorHi + 1)
                    {
                        break;
                    }
                    majorValue += majorInterval;
                }
            }
        }
    }
}

void GraphLabelsY::drawString(const Rect& invalidatedArea, const Font* fontToDraw, const AbstractDataGraph* graph, int valueScaled, int labelScaled, const uint8_t alpha) const
{
    const int16_t labelCoord = valueToScreenYQ5(graph, valueScaled).round() - graph->getGraphAreaPaddingTop();
    if (labelCoord < 0 || labelCoord >= graph->getGraphAreaHeight())
    {
        return;
    }

    Unicode::UnicodeChar wildcard[20];
    formatLabel(wildcard, 20, labelScaled, labelDecimals, labelDecimalPoint, dataScale);

    // Adjust to make label centered
    uint16_t labelHeight;
    if (labelRotation == TEXT_ROTATE_0 || labelRotation == TEXT_ROTATE_180)
    {
        labelHeight = fontToDraw->getMaxTextHeight(labelTypedText.getText(), wildcard) * fontToDraw->getNumberOfLines(labelTypedText.getText(), wildcard) + fontToDraw->getSpacingAbove(labelTypedText.getText(), wildcard);
    }
    else
    {
        labelHeight = fontToDraw->getStringWidth(labelTypedText.getText(), wildcard);
    }
    Rect labelRect(0, (graph->getGraphAreaMarginTop() + valueToScreenYQ5(graph, valueScaled).round()) - labelHeight / 2, getWidth(), labelHeight);

    Rect dirty = labelRect & invalidatedArea;
    if (!dirty.isEmpty())
    {
        dirty.x -= labelRect.x;
        dirty.y -= labelRect.y;
        translateRectToAbsolute(labelRect);
        LCD::StringVisuals visuals(fontToDraw, color, alpha, labelTypedText.getAlignment(), 0, labelRotation, labelTypedText.getTextDirection(), 0, WIDE_TEXT_NONE);
        HAL::lcd().drawString(labelRect, dirty, visuals, labelTypedText.getText(), wildcard, 0);
    }
}

GraphTitle::GraphTitle()
    : titleTypedText(TypedText(-1)), titleRotation(TEXT_ROTATE_0)
{
}

void GraphTitle::setTitleTypedText(const TypedText& typedText)
{
    titleTypedText = typedText;
}

TypedText GraphTitle::getTitleTypedText() const
{
    return titleTypedText;
}

void GraphTitle::setTitleRotation(TextRotation rotation)
{
    titleRotation = rotation;
}

TextRotation GraphTitle::getTitleRotation() const
{
    return titleRotation;
}

void GraphTitle::draw(const Rect& invalidatedArea) const
{
    if (!titleTypedText.hasValidId())
    {
        return;
    }
    const Font* fontToDraw = titleTypedText.getFont();
    if (!fontToDraw)
    {
        return;
    }

    const AbstractDataGraph* graph = getGraph();
    const uint8_t alpha = LCD::div255(getAlpha() * graph->getAlpha());
    if (alpha == 0)
    {
        return;
    }

    const uint16_t lineHeight = fontToDraw->getMaxTextHeight(titleTypedText.getText()) * fontToDraw->getNumberOfLines(titleTypedText.getText()) + fontToDraw->getSpacingAbove(titleTypedText.getText());

    Rect labelRect(rect);
    // Adjust to make label centered
    if (titleRotation == TEXT_ROTATE_0 || titleRotation == TEXT_ROTATE_180)
    {
        labelRect.y += (labelRect.height - lineHeight) / 2;
        labelRect.height = lineHeight;
    }
    else
    {
        labelRect.x += (labelRect.width - lineHeight) / 2;
        labelRect.width = lineHeight;
    }

    Rect dirty = labelRect & invalidatedArea;
    if (!dirty.isEmpty())
    {
        dirty.x -= labelRect.x;
        dirty.y -= labelRect.y;
        translateRectToAbsolute(labelRect);
        LCD::StringVisuals visuals(fontToDraw, getColor(), alpha, titleTypedText.getAlignment(), 0, titleRotation, titleTypedText.getTextDirection(), 0, WIDE_TEXT_NONE);
        HAL::lcd().drawString(labelRect, dirty, visuals, titleTypedText.getText(), 0, 0);
    }
}

bool GraphTitle::drawCanvasWidget(const Rect& /*invalidatedArea*/) const
{
    return true;
}

void GraphTitle::invalidateGraphPointAt(int16_t index)
{
}

} // namespace touchgfx
