/*
 * File      : pm_matrix.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-11-05     realthread   the first version
 */
#ifndef __PM_MATRIX_H
#define __PM_MATRIX_H

namespace Persimmon
{

class Matrix
{
public:
    Matrix();
    Matrix(const float *matrix);
    Matrix(const Matrix &other);

    bool isInvertible() const;

    Matrix product(const Matrix &other) const;
    Matrix operator*(const Matrix &other) const { return product(other); }
    void transform(float x, float y, float *tx, float *ty) const;
    float determinant() const;
    Matrix inverse() const;

    float m11() const { return m_matrix[0][0]; }
    float m12() const { return m_matrix[0][1]; }
    float m21() const { return m_matrix[1][0]; }
    float m22() const { return m_matrix[1][1]; }
    float m31() const { return m_matrix[2][0]; }
    float m32() const { return m_matrix[2][1]; }

    // static members
    static Matrix translate(float tx, float ty); // return an transform matrix
    static Matrix rotate(float angle); // return an rotate matrix
    static Matrix scale(float sx, float sy); // return an scale matrix
    static Matrix shear(float sh, float sv); // return an shear matrix

private:
    float m_matrix[3][2];
};

};

#endif
