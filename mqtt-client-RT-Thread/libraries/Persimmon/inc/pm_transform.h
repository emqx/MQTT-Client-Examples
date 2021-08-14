/*
 * File      : pm_transform.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */
#ifndef __PM_TRANSFORM_H
#define __PM_TRANSFORM_H

#include "pm_matrix.h"

namespace Persimmon
{

struct PointF
{
    PointF();
    PointF(float x, float y);
    float x, y;
};

class Transform
{
public:
    Transform();
    Transform(const Matrix &matrix);

    bool isInvertible() const;

    void translate(float tx, float ty);
    void rotate(float angle);
    void scale(float sx, float sy);
    void shear(float sh, float sv);
    PointF transform(float x, float y) const;
    PointF transform(const PointF &point) const { return transform(point.x, point.y); }
    Transform inverse() const;

    float m11() const { return m_matrix.m11(); }
    float m12() const { return m_matrix.m12(); }
    float m21() const { return m_matrix.m21(); }
    float m22() const { return m_matrix.m22(); }
    float m31() const { return m_matrix.m31(); }
    float m32() const { return m_matrix.m32(); }

private:
    Matrix m_matrix;
};

};

#endif
