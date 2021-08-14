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
 * @file touchgfx/widgets/canvas/PainterRGBA2222Bitmap.hpp
 *
 * Declares the touchgfx::PainterRGBA2222Bitmap class.
 */
#ifndef PAINTERRGBA2222BITMAP_HPP
#define PAINTERRGBA2222BITMAP_HPP

#include <stdint.h>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterRGBA2222.hpp>

namespace touchgfx
{
/**
 * PainterRGBA2222Bitmap will take the color for a given point in the shape from a
 * bitmap. Please be aware, the the bitmap is used by the CanvasWidgetRenderer (not
 * Shape), so any rotation you might specify for a Canvas Widget (e.g. Shape) is not
 * applied to the bitmap as CWR is not aware of this rotation.
 *
 * @see AbstractPainter
 */
class PainterRGBA2222Bitmap : public AbstractPainterRGBA2222
{
public:
    /**
     * Initializes a new instance of the PainterRGBA2222Bitmap class.
     *
     * @param  bmp   (Optional) The bitmap, default is #BITMAP_INVALID.
     * @param  alpha (Optional) the alpha, default is 255 i.e. solid.
     */
    PainterRGBA2222Bitmap(const Bitmap& bmp = Bitmap(BITMAP_INVALID), uint8_t alpha = 255)
        : AbstractPainterRGBA2222(), bitmapRGBA2222Pointer(0)
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

    const uint8_t* bitmapRGBA2222Pointer; ///< Pointer to the bitmap (RGBA2222)

    Bitmap bitmap;                ///< The bitmap to be used when painting
    Rect bitmapRectToFrameBuffer; ///< Bitmap rectangle translated to framebuffer coordinates
};

} // namespace touchgfx

#endif // PAINTERRGBA2222BITMAP_HPP
