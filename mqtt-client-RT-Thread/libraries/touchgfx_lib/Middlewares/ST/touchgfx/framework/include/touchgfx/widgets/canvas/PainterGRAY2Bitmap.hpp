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
 * @file touchgfx/widgets/canvas/PainterGRAY2Bitmap.hpp
 *
 * Declares the touchgfx::PainterGRAY2Bitmap class.
 */
#ifndef PAINTERGRAY2BITMAP_HPP
#define PAINTERGRAY2BITMAP_HPP

#include <stdint.h>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterGRAY2.hpp>

namespace touchgfx
{
/**
 * PainterGRAY2Bitmap will take the color for a given point in the shape from a bitmap. Please
 * be aware, the the bitmap is used by the CanvasWidgetRenderer (not Shape), so any
 * rotation you might specify for a Canvas Widget (e.g. Shape) is not applied to the
 * bitmap as CWR is not aware of this rotation.
 *
 * @see AbstractPainter
 */
class PainterGRAY2Bitmap : public AbstractPainterGRAY2
{
public:
    /**
     * Initializes a new instance of the PainterGRAY2Bitmap class.
     *
     * @param  bmp   (Optional) The bitmap, default is #BITMAP_INVALID.
     * @param  alpha (Optional) the alpha, default is 255 i.e. solid.
     */

    PainterGRAY2Bitmap(const Bitmap& bmp = Bitmap(BITMAP_INVALID), uint8_t alpha = 255)
        : AbstractPainterGRAY2(), bitmapGRAY2Pointer(0), bitmapAlphaPointer(0)
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

    virtual bool renderNext(uint8_t& gray, uint8_t& alpha);

    const uint8_t* bitmapGRAY2Pointer; ///< Pointer to the bitmap (GRAY2)
    const uint8_t* bitmapAlphaPointer; ///< Pointer to the bitmap alpha data for GRAY2

    Bitmap bitmap;                ///< The bitmap to be used when painting
    Rect bitmapRectToFrameBuffer; ///< Bitmap rectangle translated to framebuffer coordinates
};

} // namespace touchgfx

#endif // PAINTERGRAY2BITMAP_HPP
