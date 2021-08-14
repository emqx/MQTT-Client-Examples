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
 * @file touchgfx/widgets/canvas/PainterRGB888Bitmap.hpp
 *
 * Declares the touchgfx::PainterRGB888Bitmap class.
 */
#ifndef PAINTERRGB888BITMAP_HPP
#define PAINTERRGB888BITMAP_HPP

#include <stdint.h>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterRGB888.hpp>

namespace touchgfx
{
/**
 * PainterRGB888Bitmap will take the color for a given point in the shape from a bitmap.
 * Please be aware, the the bitmap is used by the CanvasWidgetRenderer (not Shape), so
 * any rotation you might specify for a Canvas Widget (e.g. Shape) is not applied to the
 * bitmap as CWR is not aware of this rotation.
 *
 * @see AbstractPainter
 */
class PainterRGB888Bitmap : public AbstractPainterRGB888
{
public:
    /**
     * Initializes a new instance of the PainterRGB888Bitmap class.
     *
     * @param  bmp   (Optional) The bitmap, default is #BITMAP_INVALID.
     * @param  alpha (Optional) the alpha, default is 255 i.e. solid.
     */
    PainterRGB888Bitmap(const Bitmap& bmp = Bitmap(BITMAP_INVALID), uint8_t alpha = 255)
        : AbstractPainterRGB888(), bitmapARGB8888Pointer(0), bitmapRGB888Pointer(0)
    {
        setBitmap(bmp);
        setAlpha(alpha);
    }

    /**
     * Sets a bitmap to be used when drawing the CanvasWidget.
     *
     * @param  bmp The bitmap.
     */
    void setBitmap(const Bitmap& bmp);

    virtual void render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers);

protected:
    virtual bool renderInit();

    virtual bool renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha);

    const uint32_t* bitmapARGB8888Pointer; ///< Pointer to the bitmap (ARGB8888)
    const uint8_t* bitmapRGB888Pointer;    ///< Pointer to the bitmap (RGB888)

    Bitmap bitmap;                ///< The bitmap to be used when painting
    Rect bitmapRectToFrameBuffer; ///< Bitmap rectangle translated to framebuffer coordinates
};

} // namespace touchgfx

#endif // PAINTERRGB888BITMAP_HPP
