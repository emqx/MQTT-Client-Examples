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

#include <touchgfx/widgets/canvas/AbstractShape.hpp>
#include <touchgfx/widgets/canvas/Canvas.hpp>

namespace touchgfx
{
AbstractShape::AbstractShape() : CanvasWidget(),
    dx(0), dy(0), shapeAngle(0),
    xScale(CWRUtil::toQ10<int>(1)), yScale(CWRUtil::toQ10<int>(1)),
    minimalRect(Rect())
{
    Drawable::setWidth(0);
    Drawable::setHeight(0);
}

bool AbstractShape::drawCanvasWidget(const Rect& invalidatedArea) const
{
    Canvas canvas(this, invalidatedArea);
    int numPoints = getNumPoints();
    if (numPoints == 0)
    {
        return true;
    }

    canvas.moveTo(getCacheX(0), getCacheY(0));
    for (int i = 1; i < numPoints; i++)
    {
        canvas.lineTo(getCacheX(i), getCacheY(i));
    }
    return canvas.render();
}

void AbstractShape::updateAbstractShapeCache()
{
    int numPoints = getNumPoints();

    int xMin = 0;
    int xMax = 0;
    int yMin = 0;
    int yMax = 0;

    for (int i = 0; i < numPoints; i++)
    {
        CWRUtil::Q5 xCorner = getCornerX(i);
        CWRUtil::Q5 yCorner = getCornerY(i);

        CWRUtil::Q5 xCache = dx + ((CWRUtil::mulQ5(xCorner, xScale) * CWRUtil::cosine(shapeAngle))) - ((CWRUtil::mulQ5(yCorner, yScale) * CWRUtil::sine(shapeAngle)));
        if (i == 0 || xCache.to<int>() > xMax)
        {
            xMax = xCache.to<int>();
        }
        if (i == 0 || xCache.to<int>() < xMin)
        {
            xMin = xCache.to<int>();
        }
        CWRUtil::Q5 yCache = dy + ((CWRUtil::mulQ5(yCorner, yScale) * CWRUtil::cosine(shapeAngle))) + ((CWRUtil::mulQ5(xCorner, xScale) * CWRUtil::sine(shapeAngle)));
        if (i == 0 || yCache.to<int>() > yMax)
        {
            yMax = yCache.to<int>();
        }
        if (i == 0 || yCache.to<int>() < yMin)
        {
            yMin = yCache.to<int>();
        }
        setCache(i, xCache, yCache);
    }
    minimalRect = Rect(xMin, yMin, xMax - xMin + 1, yMax - yMin + 1);
}

Rect AbstractShape::getMinimalRect() const
{
    return minimalRect;
}
} // namespace touchgfx
