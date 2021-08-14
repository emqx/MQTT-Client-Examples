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
 * @file touchgfx/Math3D.hpp
 *
 * Declares the touchgfx::Quadruple, touchgfx::Point4, touchgfx::Vector4
 * and touchgfx::Matrix4x4 classes.
 */
#ifndef MATH3D_HPP
#define MATH3D_HPP

#include <touchgfx/hal/Config.hpp>

namespace touchgfx
{
class Point4;
class Vector4;
class Matrix4x4;

/**
 * Multiplication operator.
 *
 * @param  multiplicand The first value to multiply.
 * @param  multiplier   The second value to multiply.
 *
 * @return The result of the operation.
 */
Matrix4x4 operator*(const Matrix4x4& multiplicand, const Matrix4x4& multiplier);

/**
 * Multiplication operator.
 *
 * @param  multiplicand The first value to multiply.
 * @param  multiplier   The second value to multiply.
 *
 * @return The result of the operation.
 */
Point4 operator*(const Matrix4x4& multiplicand, const Point4& multiplier);

/** Base class for homogeneous vectors and points. */
class Quadruple
{
public:
    /**
     * Gets an element.
     *
     * @param  row The row (0-3).
     *
     * @return The element.
     */
    FORCE_INLINE_FUNCTION float getElement(int row) const
    {
        return elements[row];
    }

    /**
     * Get x coordinate.
     *
     * @return The x coordinate.
     */
    FORCE_INLINE_FUNCTION float getX() const
    {
        return elements[0];
    }

    /**
     * Get y coordinate.
     *
     * @return The y coordinate.
     */
    FORCE_INLINE_FUNCTION float getY() const
    {
        return elements[1];
    }

    /**
     * Get z coordinate.
     *
     * @return The z coordinate.
     */
    FORCE_INLINE_FUNCTION float getZ() const
    {
        return elements[2];
    }

    /**
     * Get w coordinate.
     *
     * @return The w coordinate.
     */
    FORCE_INLINE_FUNCTION float getW() const
    {
        return elements[3];
    }

    /**
     * Sets an element.
     *
     * @param  row   The row (0-3).
     * @param  value The new value.
     */
    FORCE_INLINE_FUNCTION void setElement(int row, float value)
    {
        elements[row] = value;
    }

    /**
     * Sets an x coordinate.
     *
     * @param  value The new value.
     */
    FORCE_INLINE_FUNCTION void setX(float value)
    {
        elements[0] = value;
    }

    /**
     * Sets a y coordinate.
     *
     * @param  value The new value.
     */
    FORCE_INLINE_FUNCTION void setY(float value)
    {
        elements[1] = value;
    }

    /**
     * Sets a z coordinate.
     *
     * @param  value The new value.
     */
    FORCE_INLINE_FUNCTION void setZ(float value)
    {
        elements[2] = value;
    }

    /**
     * Sets a w coordinate.
     *
     * @param  value The new value.
     */
    FORCE_INLINE_FUNCTION void setW(float value)
    {
        elements[3] = value;
    }

protected:
    /** Initializes a new instance of the Quadruple class. */
    FORCE_INLINE_FUNCTION Quadruple()
    {
        elements[0] = elements[1] = elements[2] = elements[3] = 0; ///< The elements[ 3]
    }

    /**
     * Initializes a new instance of the Quadruple class.
     *
     * @param  x The x coordinate.
     * @param  y The y coordinate.
     * @param  z The z coordinate.
     * @param  w The w coordinate.
     */
    FORCE_INLINE_FUNCTION Quadruple(float x, float y, float z, float w)
    {
        elements[0] = x;
        elements[1] = y;
        elements[2] = z;
        elements[3] = w;
    }

    float elements[4]; ///< The elements[4]
};

/**
 * This class represents a homogeneous 3D point.
 *
 * @see Quadruple
 */
class Point4 : public Quadruple
{
public:
    /** Initializes a new instance of the Point4 class. */
    FORCE_INLINE_FUNCTION Point4()
        : Quadruple(0, 0, 0, 1)
    {
    }

    /**
     * Initializes a new instance of the Point4 class.
     *
     * @param  x The x coordinate.
     * @param  y The y coordinate.
     * @param  z The z coordinate.
     */
    FORCE_INLINE_FUNCTION Point4(float x, float y, float z)
        : Quadruple(x, y, z, 1)
    {
    }
};

/**
 * This class represents a homogeneous 3D vector.
 *
 * @see Quadruple
 */
class Vector4 : public Quadruple
{
public:
    /** Initializes a new instance of the Vector4 class. */
    FORCE_INLINE_FUNCTION Vector4()
        : Quadruple(0, 0, 0, 0)
    {
    }

    /**
     * Initializes a new instance of the Vector4 class.
     *
     * @param  x The x coordinate.
     * @param  y The y coordinate.
     * @param  z The z coordinate.
     */
    FORCE_INLINE_FUNCTION Vector4(float x, float y, float z)
        : Quadruple(x, y, z, 0)
    {
    }

    /**
     * Cross product.
     *
     * @param  operand The second operand.
     *
     * @return The result of the operation.
     */
    FORCE_INLINE_FUNCTION Vector4 crossProduct(const Vector4& operand)
    {
        float X = getY() * operand.getZ() - getZ() * operand.getY();
        float Y = getZ() * operand.getX() - getX() * operand.getZ();
        float Z = getX() * operand.getY() - getY() * operand.getX();

        return Vector4(X, Y, Z);
    }
};

/** This class represents row major 4x4 homogeneous matrices. */
class Matrix4x4
{
public:
    /** Initializes a new instance of the Point4 class. */
    Matrix4x4();

    /**
     * Gets an element.
     *
     * @param  row    The row (0-3).
     * @param  column The column (0-3).
     *
     * @return The element.
     */
    FORCE_INLINE_FUNCTION float getElement(int row, int column) const
    {
        return elements[row][column];
    }

    /**
     * Sets view distance.
     *
     * @param  distance The distance.
     */
    void setViewDistance(float distance);

    /**
     * Sets an element.
     *
     * @param  row    The row.
     * @param  column The column.
     * @param  value  The value.
     *
     * @return A matrix_4x4&amp;
     */
    FORCE_INLINE_FUNCTION Matrix4x4 setElement(int row, int column, float value)
    {
        elements[row][column] = value;

        return *this;
    }

    ///@cond
    /**
     * Gets the identity. Instead of using "Matrix4x4::identity()" consider using
     * "Matrix4x4()" instead.
     *
     * @return A matrix_4x4.
     *
     * @deprecated Use Matrix4x4() instead.
     */
    TOUCHGFX_DEPRECATED(
        "Use Matrix4x4() instead.",
        static Matrix4x4 identity())
    {
        return Matrix4x4();
    }
    ///@endcond

    /**
     * Concatenate x coordinate rotation.
     *
     * @param  radians The radians.
     *
     * @return A matrix_4x4&amp;
     */
    Matrix4x4& concatenateXRotation(float radians);

    /**
     * Concatenate y coordinate rotation.
     *
     * @param  radians The radians.
     *
     * @return A matrix_4x4&amp;
     */
    Matrix4x4& concatenateYRotation(float radians);

    /**
     * Concatenate z coordinate rotation.
     *
     * @param  radians The radians.
     *
     * @return A matrix_4x4&amp;
     */
    Matrix4x4& concatenateZRotation(float radians);

    /**
     * Concatenate x coordinate translation.
     *
     * @param  distance The distance.
     *
     * @return A matrix_4x4&amp;
     */
    Matrix4x4& concatenateXTranslation(float distance);

    /**
     * Concatenate y coordinate translation.
     *
     * @param  distance The distance.
     *
     * @return A matrix_4x4&amp;
     */
    Matrix4x4& concatenateYTranslation(float distance);

    /**
     * Concatenate z coordinate translation.
     *
     * @param  distance The distance.
     *
     * @return A matrix_4x4&amp;
     */
    Matrix4x4& concatenateZTranslation(float distance);

    /**
     * Concatenate x coordinate scale.
     *
     * @param  distance The distance.
     *
     * @return A matrix_4x4&amp;
     */
    Matrix4x4& concatenateXScale(float distance);

    /**
     * Concatenate y coordinate scale.
     *
     * @param  distance The distance.
     *
     * @return A matrix_4x4&amp;
     */
    Matrix4x4& concatenateYScale(float distance);

    /**
     * Concatenate z coordinate scale.
     *
     * @param  distance The distance.
     *
     * @return A matrix_4x4&amp;
     */
    Matrix4x4& concatenateZScale(float distance);

protected:
    float elements[4][4]; ///< The elements[4][4]
};

} // namespace touchgfx

#endif // MATH3D_HPP
