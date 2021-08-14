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
 * @file touchgfx/transforms/TouchCalibration.hpp
 *
 * Declares the touchgfx::TouchCalibration class.
 */
#ifndef TOUCHCALIBRATION_HPP
#define TOUCHCALIBRATION_HPP

#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/**
 * Calibrates a touch coordinate.
 *
 * Class TouchCalibraiton is responsible for translating coordinates (Point) based on
 * matrix of calibration values.
 */
class TouchCalibration
{
public:
    TouchCalibration()
    {
        matrix.Divider = 0;
    }

    /**
     * Initializes the calibration matrix based on reference and measured values.
     *
     * @param  ref Pointer to array of three reference points.
     * @param  scr Pointer to array of three measured points.
     */
    static void setCalibrationMatrix(const Point* ref, const Point* scr);

    /**
     * Translates the specified point using the matrix. If matrix has not been initialized,
     * p is not modified.
     *
     * @param [in,out] p The point to translate.
     */
    static void translatePoint(Point& p);

private:
    static int32_t muldiv(int32_t factor1, int32_t mls, int32_t mls_result, int32_t mls_remainder, int32_t factor2, int32_t divisor, int32_t& remainder);

    static int32_t clz(int32_t x);

    /**
     * A matrix. See http://www.embedded.com/design/system-integration/4023968/How-To-Calibrate-Touch-Screens
     * for calibration technique by Carlos E. Vidales.
     */
    struct Matrix
    {
        int32_t An, Bn, Cn, Dn, En, Fn, Divider;
        int32_t mlsAn, mlsBn, mlsDn, mlsEn;
        int32_t resAn, resBn, resDn, resEn;
        int32_t remAn, remBn, remDn, remEn;
    };

    typedef struct Matrix Matrix;

    static Matrix matrix;
};

} // namespace touchgfx

#endif // TOUCHCALIBRATION_HPP
