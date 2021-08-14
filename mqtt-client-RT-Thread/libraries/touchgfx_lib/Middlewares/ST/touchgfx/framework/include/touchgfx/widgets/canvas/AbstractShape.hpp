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
 * @file touchgfx/widgets/canvas/AbstractShape.hpp
 *
 * Declares the touchgfx::AbstractShape class.
 */
#ifndef ABSTRACTSHAPE_HPP
#define ABSTRACTSHAPE_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/Widget.hpp>

#include <touchgfx/widgets/canvas/CanvasWidget.hpp>

namespace touchgfx
{
/**
 * Simple widget capable of drawing a abstractShape. The abstractShape can be scaled and rotated
 * around 0,0. The shapes points (corners) are calculated with regards to scaling and
 * rotation to allow for faster redrawing. Care must be taken to call
 * updateAbstractShapeCache() after updating the shape, the scale of the shape or the
 * rotation of the shape.
 */
class AbstractShape : public CanvasWidget
{
public:
    /**
     * Defines an alias for a single point. Users of the AbstractShape can chose to store
     * the coordinates as int or float depending on the needs. This will help setting up the
     * abstractShape very easily using setAbstractShape().
     *
     * @tparam T Generic type parameter, either int or float.
     *
     * @see setShape
     */
    template <typename T>
    struct ShapePoint
    {
        T x; ///< The x coordinate of the points.
        T y; ///< The y coordinate of the points.
    };

    AbstractShape();

    /**
     * Gets number of points used to make up the shape.
     *
     * @return The number of points.
     */
    virtual int getNumPoints() const = 0;

    /**
     * Sets one of the points (a corner) of the shape in CWRUtil::Q5 format.
     *
     * @param  i Zero-based index of the corner.
     * @param  x The x coordinate in CWRUtil::Q5 format.
     * @param  y The y coordinate in CWRUtil::Q5 format.
     *
     * @see updateAbstractShapeCache
     *
     * @note Remember to call updateAbstractShapeCache() after calling setCorner one or more times,
     *       to make sure that the cached outline of the shape is correct.
     */
    virtual void setCorner(int i, CWRUtil::Q5 x, CWRUtil::Q5 y) = 0;

    /**
     * Gets the x coordinate of a corner (a point) of the shape.
     *
     * @param  i Zero-based index of the corner.
     *
     * @return The corner x coordinate in CWRUtil::Q5 format.
     */
    virtual CWRUtil::Q5 getCornerX(int i) const = 0;

    /**
     * Gets the y coordinate of a corner (a point) of the shape.
     *
     * @param  i Zero-based index of the corner.
     *
     * @return The corner y coordinate in CWRUtil::Q5 format.
     */
    virtual CWRUtil::Q5 getCornerY(int i) const = 0;

    /**
     * Sets a shape the struct Points. The cached outline of the shape is automatically
     * updated.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param [in] points The points that make up the shape. The pointer must point to an array
     *                    of getNumPoints() elements of type ShapePoint.
     *
     * @note The area containing the shape is not invalidated.
     */
    template <typename T>
    void setShape(ShapePoint<T>* points)
    {
        int numPoints = getNumPoints();
        for (int i = 0; i < numPoints; i++)
        {
            setCorner(i, CWRUtil::toQ5<T>(points[i].x), CWRUtil::toQ5<T>(points[i].y));
        }
        updateAbstractShapeCache();
    }

    /**
     * Sets a shape the struct Points. The cached outline of the shape is automatically
     * updated.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param [in] points The points that make up the shape. The pointer must point to an array
     *                    of getNumPoints() elements of type ShapePoint.
     *
     * @note The area containing the shape is not invalidated.
     */
    template <typename T>
    void setShape(const ShapePoint<T>* points)
    {
        int numPoints = getNumPoints();
        for (int i = 0; i < numPoints; i++)
        {
            setCorner(i, CWRUtil::toQ5<T>(points[i].x), CWRUtil::toQ5<T>(points[i].y));
        }
        updateAbstractShapeCache();
    }

    /**
     * Sets the position of the shape's (0,0) in the widget. This means that all coordinates
     * initially used when created the shape are moved relative to these given offsets.
     * Subsequent calls to setOrigin() or moveOrigin() will replace the old values for
     * origin. The cached outline of the shape is automatically updated.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param  x The absolute x coordinate of the shapes position (0,0).
     * @param  y The absolute y coordinate of the shapes position (0,0).
     *
     * @see moveOrigin
     *
     * @note The area containing the AbstractShape is not invalidated.
     */
    template <typename T>
    void setOrigin(T x, T y)
    {
        CWRUtil::Q5 dxNew = CWRUtil::toQ5<T>(x);
        CWRUtil::Q5 dyNew = CWRUtil::toQ5<T>(y);

        if (dx == dxNew && dy == dyNew)
        {
            return;
        }

        dx = dxNew;
        dy = dyNew;

        updateAbstractShapeCache();
    }

    /**
     * Sets the position of the shape's (0,0) in the widget. This means that all coordinates
     * initially used when created the shape are moved relative to these given offsets.
     * Subsequent calls to moveOrigin() or setOrigin() will replace the old values for
     * origin. The cached outline of the shape is automatically updated.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param  x The absolute x coordinate of the shapes position (0,0).
     * @param  y The absolute y coordinate of the shapes position (0,0).
     *
     * @see setOrigin
     *
     * @note The area containing the AbstractShape is invalidated before and after the change.
     */
    template <typename T>
    void moveOrigin(T x, T y)
    {
        CWRUtil::Q5 xNew = CWRUtil::toQ5<T>(x);
        CWRUtil::Q5 yNew = CWRUtil::toQ5<T>(y);

        if (dx == xNew && dy == yNew)
        {
            return;
        }

        Rect rect = getMinimalRect();

        dx = xNew;
        dy = yNew;

        updateAbstractShapeCache();

        rect.expandToFit(getMinimalRect());
        invalidateRect(rect);
    }

    /**
     * Gets the position of the shapes (0,0).
     *
     * @tparam T Generic type parameter, either int or float.
     * @param [out] dx The x coordinate rounded down to the precision of T.
     * @param [out] dy The y coordinate rounded down to the precision of T.
     */
    template <typename T>
    void getOrigin(T& dx, T& dy) const
    {
        dx = this->dx.to<T>();
        dy = this->dy.to<T>();
    }

    /**
     * Sets the absolute angle to turn the AbstractShape. 0 degrees means no rotation and 90
     * degrees is rotate the shape clockwise. Repeated calls to setAngle(10) will therefore
     * not rotate the shape by an additional 10 degrees. The cached outline of the shape is
     * automatically updated.
     *
     * @tparam T Generic type parameter.
     * @param  angle The absolute angle to turn the abstractShape to relative to 0 (straight up).
     *
     * @see updateAngle
     *
     * @note The area containing the AbstractShape is not invalidated.
     */
    template <typename T>
    void setAngle(T angle)
    {
        CWRUtil::Q5 angleQ5 = CWRUtil::toQ5<T>(angle);
        if (shapeAngle != angleQ5)
        {
            shapeAngle = angleQ5;
            updateAbstractShapeCache();
        }
    }

    /**
     * Gets the abstractShape's angle.
     *
     * @tparam T Generic type parameter.
     * @param [out] angle The current AbstractShape rotation angle rounded down to the precision of T.
     */
    template <typename T>
    void getAngle(T& angle)
    {
        angle = this->shapeAngle.to<T>();
    }

    /**
     * Sets the absolute angle to turn the AbstractShape. 0 degrees means no rotation and 90
     * degrees is rotate the shape clockwise. Repeated calls to setAngle(10) will therefore
     * not rotate the shape by an additional 10 degrees. The cached outline of the shape is
     * automatically updated.
     *
     * @tparam T Generic type parameter.
     * @param  angle The angle to turn the abstractShape.
     *
     * @see setAngle
     *
     * @note The area containing the AbstractShape is invalidated before and after the change.
     */
    template <typename T>
    void updateAngle(T angle)
    {
        CWRUtil::Q5 angleQ5 = CWRUtil::toQ5<T>(angle);
        if (shapeAngle != angleQ5)
        {
            Rect rectBefore = getMinimalRect();

            shapeAngle = angleQ5;
            updateAbstractShapeCache();

            Rect rectAfter = getMinimalRect();
            rectBefore.expandToFit(rectAfter);
            invalidateRect(rectBefore);
        }
    }

    /**
     * Gets the current angle of the abstractShape.
     *
     * @return The angle of the AbstractShaperounded down to the precision of int.
     */
    int getAngle() const
    {
        return shapeAngle.to<int>();
    }

    /**
     * Scale the AbstractShape the given amounts in the x direction and the y direction. The
     * new scaling factors do not multiply to previously set scaling factors, so scaling by
     * 2 and later scaling by 2 again will not scale by 4, only by 2. The cached outline of
     * the shape is automatically updated.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param  newXScale The new scale in the x direction.
     * @param  newYScale The new scale in the y direction.
     *
     * @see getScale, updateScale
     *
     * @note The area containing the AbstractShape is not invalidated.
     */
    template <typename T>
    void setScale(T newXScale, T newYScale)
    {
        xScale = CWRUtil::toQ10<T>(newXScale);
        yScale = CWRUtil::toQ10<T>(newYScale);
        updateAbstractShapeCache();
    }

    /**
     * Scale the AbstractShape the given amount in the x direction and the y direction. The
     * new scaling factors do not multiply to previously set scaling factors, so scaling by
     * 2 and later scaling by 2 again will not scale by 4, only by 2. The cached outline of
     * the shape is automatically updated.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param  scale The scale in the x direction.
     *
     * @see getScale
     *
     * @note The area containing the AbstractShape is not invalidated.
     */
    template <typename T>
    void setScale(T scale)
    {
        setScale(scale, scale);
    }

    /**
     * Scale the AbstractShape the given amount in the x direction and the y direction. The
     * new scaling factors do not multiply to previously set scaling factors, so scaling by
     * 2 and later scaling by 2 again will not scale by 4, only by 2. The cached outline of
     * the shape is automatically updated.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param  newXScale The new scale in the x direction.
     * @param  newYScale The new scale in the y direction.
     *
     * @see setScale
     *
     * @note The area containing the AbstractShape is invalidated before and after the change.
     */
    template <typename T>
    void updateScale(T newXScale, T newYScale)
    {
        CWRUtil::Q10 xScaleQ10 = CWRUtil::toQ10<T>(newXScale);
        CWRUtil::Q10 yScaleQ10 = CWRUtil::toQ10<T>(newYScale);

        if (xScale != xScaleQ10 || yScale != yScaleQ10)
        {
            Rect rectBefore = getMinimalRect();

            xScale = xScaleQ10;
            yScale = yScaleQ10;
            updateAbstractShapeCache();

            Rect rectAfter = getMinimalRect();
            rectBefore.expandToFit(rectAfter);
            invalidateRect(rectBefore);
        }
    }

    /**
     * Gets the x scale and y scale of the shape as previously set using setScale. Default
     * is 1 for both x scale and y scale.
     *
     * @tparam T Generic type parameter, either int or float.
     * @param [out] x Scaling of x coordinates rounded down to the precision of T.
     * @param [out] y Scaling of y coordinates rounded down to the precision of T.
     *
     * @see setScale
     */
    template <typename T>
    void getScale(T& x, T& y) const
    {
        x = xScale.to<T>();
        y = yScale.to<T>();
    }

    virtual bool drawCanvasWidget(const Rect& invalidatedArea) const;

    /**
     * Updates the AbstractShape cache. The cache is used to be able to quickly redraw the
     * AbstractShape without calculating the points that make up the abstractShape (with
     * regards to scaling and rotation).
     */
    void updateAbstractShapeCache();

protected:
    /**
     * Sets the cached coordinates of a given point/corner. The coordinates in the cache are
     * the coordinates from the corners after rotation and scaling has been applied to the
     * coordinate.
     *
     * @param  i Zero-based index of the corner.
     * @param  x The x coordinate.
     * @param  y The y coordinate.
     */
    virtual void setCache(int i, CWRUtil::Q5 x, CWRUtil::Q5 y) = 0;

    /**
     * Gets cached x coordinate of a point/corner.
     *
     * @param  i Zero-based index of the point/corner.
     *
     * @return The cached x coordinate, or zero if nothing is cached for the given i.
     */
    virtual CWRUtil::Q5 getCacheX(int i) const = 0;

    /**
     * Gets cached y coordinate of a point/corner.
     *
     * @param  i Zero-based index of the point/corner.
     *
     * @return The cached y coordinate, or zero if nothing is cached for the given i.
     */
    virtual CWRUtil::Q5 getCacheY(int i) const = 0;

    virtual Rect getMinimalRect() const;

private:
    CWRUtil::Q5 dx, dy;
    CWRUtil::Q5 shapeAngle;
    CWRUtil::Q10 xScale, yScale;
    Rect minimalRect;
};

} // namespace touchgfx

#endif // ABSTRACTSHAPE_HPP
