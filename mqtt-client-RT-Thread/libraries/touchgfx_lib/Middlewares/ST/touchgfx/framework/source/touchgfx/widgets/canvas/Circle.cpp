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

#include <touchgfx/widgets/canvas/Circle.hpp>

namespace touchgfx
{
Circle::Circle() : CanvasWidget(),
    circleCenterX(0), circleCenterY(0), circleRadius(0),
    circleArcAngleStart(CWRUtil::toQ5<int>(0)), circleArcAngleEnd(CWRUtil::toQ5<int>(360)),
    circleLineWidth(0), circleArcIncrement(5),
    circleCapArcIncrement(180)
{
    Drawable::setWidth(0);
    Drawable::setHeight(0);
}

void Circle::setPrecision(int precision)
{
    if (precision < 1)
    {
        precision = 1;
    }
    if (precision > 120)
    {
        precision = 120;
    }
    circleArcIncrement = precision;
}

int Circle::getPrecision() const
{
    return circleArcIncrement;
}

void Circle::setCapPrecision(int precision)
{
    if (precision < 1)
    {
        precision = 1;
    }
    if (precision > 180)
    {
        precision = 180;
    }
    circleCapArcIncrement = precision;
}

int Circle::getCapPrecision() const
{
    return circleCapArcIncrement;
}

bool Circle::drawCanvasWidget(const Rect& invalidatedArea) const
{
    CWRUtil::Q5 arcStart = circleArcAngleStart;
    CWRUtil::Q5 arcEnd = circleArcAngleEnd;

    CWRUtil::Q5 _360 = CWRUtil::toQ5<int>(360);

    // Put start before end by swapping
    if (arcStart > arcEnd)
    {
        CWRUtil::Q5 tmp = arcStart;
        arcStart = arcEnd;
        arcEnd = tmp;
    }

    if ((arcEnd - arcStart) >= _360)
    {
        // The entire circle has to be drawn
        arcStart = CWRUtil::toQ5<int>(0);
        arcEnd = _360;
    }

    if (circleLineWidth != 0)
    {
        // Check if invalidated area is completely inside the circle
        int32_t x1 = int(CWRUtil::toQ5(invalidatedArea.x)); // Take the corners of the invalidated area
        int32_t x2 = int(CWRUtil::toQ5(invalidatedArea.right()));
        int32_t y1 = int(CWRUtil::toQ5(invalidatedArea.y));
        int32_t y2 = int(CWRUtil::toQ5(invalidatedArea.bottom()));
        int32_t dx1 = abs(int(circleCenterX) - x1); // Find distances between each corner and circle center
        int32_t dx2 = abs(int(circleCenterX) - x2);
        int32_t dy1 = abs(int(circleCenterY) - y1);
        int32_t dy2 = abs(int(circleCenterY) - y2);
        int32_t dx = CWRUtil::Q5(MAX(dx1, dx2)).to<int>() + 1; // Largest hor/vert distance (round up)
        int32_t dy = CWRUtil::Q5(MAX(dy1, dy2)).to<int>() + 1;
        int32_t dsqr = (dx * dx) + (dy * dy); // Pythagoras

        // From https://www.mathopenref.com/polygonincircle.html
        int32_t rmin = ((circleRadius - (circleLineWidth / 2)) * CWRUtil::cosine((circleArcIncrement + 1) / 2)).to<int>();

        // Check if invalidatedArea is completely inside circle
        if (dsqr < rmin * rmin)
        {
            return true;
        }
    }

    Canvas canvas(this, invalidatedArea);

    CWRUtil::Q5 radius = circleRadius;
    CWRUtil::Q5 lineWidth = circleLineWidth;
    if (circleLineWidth > circleRadius * 2)
    {
        lineWidth = (circleRadius + circleLineWidth / 2);
        radius = lineWidth / 2;
    }

    CWRUtil::Q5 arc = arcStart;
    CWRUtil::Q5 circleArcIncrementQ5 = CWRUtil::toQ5<int>(circleArcIncrement);
    moveToAR2(canvas, arc, (radius * 2) + lineWidth);
    CWRUtil::Q5 nextArc = CWRUtil::Q5(ROUNDUP((int)(arc + CWRUtil::toQ5<int>(1)), (int)circleArcIncrementQ5));
    while (nextArc <= arcEnd)
    {
        arc = nextArc;
        lineToAR2(canvas, arc, (radius * 2) + lineWidth);
        nextArc = nextArc + circleArcIncrementQ5;
    }
    if (arc < arcEnd)
    {
        // "arc" is not updated. It is the last arc in steps of "circleArcIncrement"
        lineToAR2(canvas, arcEnd, (radius * 2) + lineWidth);
    }

    if (lineWidth == CWRUtil::toQ5<int>(0))
    {
        // Draw a filled circle / pie / pacman
        if (arcEnd - arcStart < _360)
        {
            // Not a complete circle, line to center
            canvas.lineTo(circleCenterX, circleCenterY);
        }
    }
    else
    {
        CWRUtil::Q5 circleCapArcIncrementQ5 = CWRUtil::toQ5<int>(circleCapArcIncrement);
        CWRUtil::Q5 _180 = CWRUtil::toQ5<int>(180);
        if (arcEnd - arcStart < _360)
        {
            // Draw the circle cap
            CWRUtil::Q5 capX = circleCenterX + (radius * CWRUtil::sine(arcEnd));
            CWRUtil::Q5 capY = circleCenterY - (radius * CWRUtil::cosine(arcEnd));
            for (CWRUtil::Q5 capAngle = arcEnd + circleCapArcIncrementQ5; capAngle < arcEnd + _180; capAngle = capAngle + circleCapArcIncrementQ5)
            {
                lineToXYAR2(canvas, capX, capY, capAngle, lineWidth);
            }
        }

        // Not a filled circle, draw the path on the inside of the circle
        if (arc < arcEnd)
        {
            lineToAR2(canvas, arcEnd, (radius * 2) - lineWidth);
        }

        nextArc = arc;
        while (nextArc >= arcStart)
        {
            arc = nextArc;
            lineToAR2(canvas, arc, (radius * 2) - lineWidth);
            nextArc = nextArc - circleArcIncrementQ5;
        }

        if (arc > arcStart)
        {
            lineToAR2(canvas, arcStart, (radius * 2) - lineWidth);
        }

        if (arcEnd - arcStart < _360)
        {
            // Draw the circle cap
            CWRUtil::Q5 capX = circleCenterX + (radius * CWRUtil::sine(arcStart));
            CWRUtil::Q5 capY = circleCenterY - (radius * CWRUtil::cosine(arcStart));
            for (CWRUtil::Q5 capAngle = arcStart - _180 + circleCapArcIncrementQ5; capAngle < arcStart; capAngle = capAngle + circleCapArcIncrementQ5)
            {
                lineToXYAR2(canvas, capX, capY, capAngle, lineWidth);
            }
        }
    }

    return canvas.render();
}

Rect Circle::getMinimalRect() const
{
    return getMinimalRect(circleArcAngleStart, circleArcAngleEnd);
}

Rect Circle::getMinimalRect(int16_t arcStart, int16_t arcEnd) const
{
    return getMinimalRect(CWRUtil::toQ5<int>(arcStart), CWRUtil::toQ5<int>(arcEnd));
}

Rect Circle::getMinimalRect(CWRUtil::Q5 arcStart, CWRUtil::Q5 arcEnd) const
{
    CWRUtil::Q5 xMin = CWRUtil::toQ5<int>(getWidth());
    CWRUtil::Q5 xMax = CWRUtil::toQ5<int>(0);
    CWRUtil::Q5 yMin = CWRUtil::toQ5<int>(getHeight());
    CWRUtil::Q5 yMax = CWRUtil::toQ5<int>(0);
    calculateMinimalRect(arcStart, arcEnd, xMin, xMax, yMin, yMax);
    return Rect(xMin.to<int>() - 1, yMin.to<int>() - 1,
                xMax.to<int>() - xMin.to<int>() + 2, yMax.to<int>() - yMin.to<int>() + 2);
}

void Circle::moveToAR2(Canvas& canvas, const CWRUtil::Q5& angle, const CWRUtil::Q5& r2) const
{
    canvas.moveTo(circleCenterX + ((r2 * CWRUtil::sine(angle)) / 2), circleCenterY - ((r2 * CWRUtil::cosine(angle)) / 2));
}

void Circle::lineToAR2(Canvas& canvas, const CWRUtil::Q5& angle, const CWRUtil::Q5& r2) const
{
    lineToXYAR2(canvas, circleCenterX, circleCenterY, angle, r2);
}

void Circle::lineToXYAR2(Canvas& canvas, const CWRUtil::Q5& x, const CWRUtil::Q5& y, const CWRUtil::Q5& angle, const CWRUtil::Q5& r2) const
{
    canvas.lineTo(x + ((r2 * CWRUtil::sine(angle)) / 2), y - ((r2 * CWRUtil::cosine(angle)) / 2));
}

void Circle::updateMinMaxAR(const CWRUtil::Q5& a, const CWRUtil::Q5& r2, CWRUtil::Q5& xMin, CWRUtil::Q5& xMax, CWRUtil::Q5& yMin, CWRUtil::Q5& yMax) const
{
    CWRUtil::Q5 xNew = circleCenterX + ((r2 * CWRUtil::sine(a)) / 2);
    CWRUtil::Q5 yNew = circleCenterY - ((r2 * CWRUtil::cosine(a)) / 2);
    updateMinMaxXY(xNew, yNew, xMin, xMax, yMin, yMax);
}

void Circle::updateMinMaxXY(const CWRUtil::Q5& xNew, const CWRUtil::Q5& yNew, CWRUtil::Q5& xMin, CWRUtil::Q5& xMax, CWRUtil::Q5& yMin, CWRUtil::Q5& yMax) const
{
    if (xNew < xMin)
    {
        xMin = xNew;
    }
    if (xNew > xMax)
    {
        xMax = xNew;
    }
    if (yNew < yMin)
    {
        yMin = yNew;
    }
    if (yNew > yMax)
    {
        yMax = yNew;
    }
}

void Circle::calculateMinimalRect(CWRUtil::Q5 arcStart, CWRUtil::Q5 arcEnd, CWRUtil::Q5& xMin, CWRUtil::Q5& xMax, CWRUtil::Q5& yMin, CWRUtil::Q5& yMax) const
{
    // Put start before end by swapping
    if (arcStart > arcEnd)
    {
        CWRUtil::Q5 tmp = arcStart;
        arcStart = arcEnd;
        arcEnd = tmp;
    }

    CWRUtil::Q5 _90 = CWRUtil::toQ5<int>(90);
    CWRUtil::Q5 _360 = CWRUtil::toQ5<int>(360);

    if ((arcEnd - arcStart) >= _360)
    {
        // The entire circle has to be drawn
        arcStart = CWRUtil::toQ5<int>(0);
        arcEnd = _360;
    }

    // Check start angle
    updateMinMaxAR(arcStart, (circleRadius * 2) + circleLineWidth, xMin, xMax, yMin, yMax);
    // Here we have a up to 4 approximation steps on angles divisible by 90
    CWRUtil::Q5 i;
    for (i = CWRUtil::Q5(ROUNDUP((int)(arcStart + CWRUtil::toQ5<int>(1)), (int)_90)); i <= arcEnd; i = i + _90)
    {
        updateMinMaxAR(i, (circleRadius * 2) + circleLineWidth, xMin, xMax, yMin, yMax);
    }
    // Check end angle
    if ((i - _90) < arcEnd)
    {
        updateMinMaxAR(arcEnd, (circleRadius * 2) + circleLineWidth, xMin, xMax, yMin, yMax);
    }

    if (circleLineWidth == CWRUtil::toQ5<int>(0))
    {
        // A filled circle / pie / pacman
        if ((arcEnd - arcStart) < _360)
        {
            // Not a complete circle, check center
            updateMinMaxAR(CWRUtil::toQ5<int>(0), CWRUtil::toQ5<int>(0), xMin, xMax, yMin, yMax);
        }
    }
    else
    {
        // Not a filled circle, check the inside of the circle. Only start and/or end can cause new min/max values
        updateMinMaxAR(arcStart, (circleRadius * 2) - circleLineWidth, xMin, xMax, yMin, yMax);
        updateMinMaxAR(arcEnd, (circleRadius * 2) - circleLineWidth, xMin, xMax, yMin, yMax);
    }

    // Check if circle cap extends the min/max further
    if ((circleCapArcIncrement < 180) && (arcEnd - arcStart < _360))
    {
        // Round caps
        CWRUtil::Q5 capX = circleCenterX + (circleRadius * CWRUtil::sine(arcStart));
        CWRUtil::Q5 capY = circleCenterY - (circleRadius * CWRUtil::cosine(arcStart));
        updateMinMaxXY(capX - (circleLineWidth / 2), capY - (circleLineWidth / 2), xMin, xMax, yMin, yMax);
        updateMinMaxXY(capX + (circleLineWidth / 2), capY + (circleLineWidth / 2), xMin, xMax, yMin, yMax);
        capX = circleCenterX + (circleRadius * CWRUtil::sine(arcEnd));
        capY = circleCenterY - (circleRadius * CWRUtil::cosine(arcEnd));
        updateMinMaxXY(capX - (circleLineWidth / 2), capY - (circleLineWidth / 2), xMin, xMax, yMin, yMax);
        updateMinMaxXY(capX + (circleLineWidth / 2), capY + (circleLineWidth / 2), xMin, xMax, yMin, yMax);
    }
}

touchgfx::Rect Circle::getMinimalRectForUpdatedStartAngle(const CWRUtil::Q5& startAngleQ5) const
{
    CWRUtil::Q5 minAngle = CWRUtil::Q5(0); // Unused default value
    CWRUtil::Q5 maxAngle = CWRUtil::Q5(0); // Unused default value
    int circleArcIncrementQ5int = (int)CWRUtil::toQ5<int>(circleArcIncrement);
    if (circleArcAngleStart < circleArcAngleEnd)
    {
        // start is smaller than end
        if (startAngleQ5 < circleArcAngleStart)
        {
            // start moved even lower
            minAngle = startAngleQ5;
            maxAngle = CWRUtil::Q5(ROUNDUP((int)circleArcAngleStart, circleArcIncrementQ5int));
            maxAngle = MIN(maxAngle, circleArcAngleEnd); // No need to go higher than end
        }
        else if (startAngleQ5 < circleArcAngleEnd)
        {
            // start moved higher, but not higher than end
            minAngle = circleArcAngleStart;
            maxAngle = CWRUtil::Q5(ROUNDUP((int)startAngleQ5, circleArcIncrementQ5int));
            maxAngle = MIN(maxAngle, circleArcAngleEnd); // No need to go higher than end
        }
        else
        {
            // start moved past end
            minAngle = circleArcAngleStart;
            maxAngle = startAngleQ5;
        }
    }
    else
    {
        // start is higher than end
        if (startAngleQ5 > circleArcAngleStart)
        {
            // start moved even higher
            minAngle = CWRUtil::Q5(ROUNDDOWN((int)circleArcAngleStart, circleArcIncrementQ5int));
            minAngle = MAX(minAngle, circleArcAngleEnd); // No need to go lower then end
            maxAngle = startAngleQ5;
        }
        else if (startAngleQ5 > circleArcAngleEnd)
        {
            // start moved lower, but not lower than end
            minAngle = CWRUtil::Q5(ROUNDDOWN((int)startAngleQ5, circleArcIncrementQ5int));
            minAngle = MAX(minAngle, circleArcAngleEnd); // No need to go lower than end
            maxAngle = circleArcAngleStart;
        }
        else
        {
            // start moved lower past end
            minAngle = startAngleQ5;
            maxAngle = circleArcAngleStart;
        }
    }
    return getMinimalRect(minAngle, maxAngle);
}

touchgfx::Rect Circle::getMinimalRectForUpdatedEndAngle(const CWRUtil::Q5& endAngleQ5) const
{
    CWRUtil::Q5 minAngle = CWRUtil::Q5(0); // Unused default value
    CWRUtil::Q5 maxAngle = CWRUtil::Q5(0); // Unused default value
    int circleArcIncrementQ5int = (int)CWRUtil::toQ5<int>(circleArcIncrement);
    if (circleArcAngleStart < circleArcAngleEnd)
    {
        // start is smaller than end
        if (endAngleQ5 > circleArcAngleEnd)
        {
            // end moved even higher
            minAngle = CWRUtil::Q5(ROUNDDOWN((int)circleArcAngleEnd, circleArcIncrementQ5int));
            minAngle = MAX(minAngle, circleArcAngleStart);
            maxAngle = endAngleQ5;
        }
        else if (endAngleQ5 > circleArcAngleStart)
        {
            // end moved lower, but not past start
            minAngle = CWRUtil::Q5(ROUNDDOWN((int)endAngleQ5, circleArcIncrementQ5int));
            minAngle = MAX(minAngle, circleArcAngleStart); // No need to go lower than start
            maxAngle = circleArcAngleEnd;
        }
        else
        {
            // end move past start
            minAngle = endAngleQ5;
            maxAngle = circleArcAngleEnd;
        }
    }
    else
    {
        // start is higher than end
        if (endAngleQ5 < circleArcAngleEnd)
        {
            // end moved even lower
            minAngle = endAngleQ5;
            maxAngle = CWRUtil::Q5(ROUNDUP((int)circleArcAngleEnd, circleArcIncrementQ5int));
            maxAngle = MIN(maxAngle, circleArcAngleStart); // No need to go higher than start
        }
        else if (endAngleQ5 < circleArcAngleStart)
        {
            // end moved higher, but not higher than start
            minAngle = circleArcAngleEnd;
            maxAngle = CWRUtil::Q5(ROUNDUP((int)endAngleQ5, circleArcIncrementQ5int));
            maxAngle = MIN(maxAngle, circleArcAngleStart);
        }
        else
        {
            // end moved past start
            minAngle = circleArcAngleEnd;
            maxAngle = endAngleQ5;
        }
    }
    return getMinimalRect(minAngle, maxAngle);
}
} // namespace touchgfx
