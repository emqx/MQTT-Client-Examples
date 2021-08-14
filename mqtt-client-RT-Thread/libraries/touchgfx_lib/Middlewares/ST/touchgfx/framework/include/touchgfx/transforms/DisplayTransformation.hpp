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
 * @file touchgfx/transforms/DisplayTransformation.hpp
 *
 * Declares the touchgfx::DisplayTransformation class.
 */
#ifndef DISPLAYTRANSFORMATION_HPP
#define DISPLAYTRANSFORMATION_HPP

#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/**
 * Defines transformations from display space to framebuffer space. The display might be
 * (considered) in portrait mode from 0,0 to 272,480, while the actual framebuffer is
 * from 0,0 to 480,272. This class handles the transformations.
 */
class DisplayTransformation
{
public:
    /**
     * Transform x,y from display to framebuffer coordinates.
     *
     * @param [in,out] x the x part to translate.
     * @param [in,out] y the y part to translate.
     */
    static void transformDisplayToFrameBuffer(int16_t& x, int16_t& y);

    /**
     * Transform x,y from display to framebuffer coordinates.
     *
     * @param [in,out] x the x part to translate.
     * @param [in,out] y the y part to translate.
     */
    static void transformDisplayToFrameBuffer(float& x, float& y);

    /**
     * Transform x,y from framebuffer to display coordinates.
     *
     * @param [in,out] x the x part to translate.
     * @param [in,out] y the y part to translate.
     */
    static void transformFrameBufferToDisplay(int16_t& x, int16_t& y);

    /**
     * Transform x,y from coordinates relative to the in rect to framebuffer coordinates.
     *
     * Transform x,y from coordinates relative to the in rect to framebuffer coordinates.
     *
     * @param [in,out] x  the x part to translate.
     * @param [in,out] y  the y part to translate.
     * @param          in the rectangle defining the coordinate space.
     */
    static void transformDisplayToFrameBuffer(int16_t& x, int16_t& y, const Rect& in);

    /**
     * Transform x,y from coordinates relative to the in rect to framebuffer coordinates.
     *
     * Transform x,y from coordinates relative to the in rect to framebuffer coordinates.
     *
     * @param [in,out] x  the x part to translate.
     * @param [in,out] y  the y part to translate.
     * @param          in the rectangle defining the coordinate space.
     */
    static void transformDisplayToFrameBuffer(float& x, float& y, const Rect& in);

    /**
     * Transform rectangle from display to framebuffer coordinates.
     *
     * @param [in,out] r the rectangle to translate.
     */
    static void transformDisplayToFrameBuffer(Rect& r);

    /**
     * Transform rectangle from framebuffer to display coordinates.
     *
     * @param [in,out] r the rectangle to translate.
     */
    static void transformFrameBufferToDisplay(Rect& r);

    /**
     * Transform rectangle r from coordinates relative to the in rect to framebuffer
     * coordinates.
     *
     * Transform rectangle r from coordinates relative to the in rect to framebuffer
     * coordinates.
     *
     * @param [in,out] r  the rectangle to translate.
     * @param          in the rectangle defining the coordinate space.
     */
    static void transformDisplayToFrameBuffer(Rect& r, const Rect& in);
};

} // namespace touchgfx

#endif // DISPLAYTRANSFORMATION_HPP
