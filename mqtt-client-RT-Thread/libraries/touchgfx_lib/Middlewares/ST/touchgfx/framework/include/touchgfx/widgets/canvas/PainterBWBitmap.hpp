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
 * @file touchgfx/widgets/canvas/PainterBWBitmap.hpp
 *
 * Declares the touchgfx::PainterBWBitmap class.
 */
#ifndef PAINTERBWBITMAP_HPP
#define PAINTERBWBITMAP_HPP

#include <stdint.h>
#include <platform/driver/lcd/LCD1bpp.hpp>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/transforms/DisplayTransformation.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterBW.hpp>

namespace touchgfx
{
/**
 * PainterBWBitmap will take the color for a given point in the shape from a bitmap. Please be
 * aware, the the bitmap is used by the CanvasWidgetRenderer (not Shape), so any
 * rotation you might specify for a Canvas Widget (e.g. Shape) is not applied to the
 * bitmap as CWR is not aware of this rotation.
 *
 * @see AbstractPainter
 */
class PainterBWBitmap : public AbstractPainterBW
{
public:
    /**
     * Initializes a new instance of the PainterBWBitmap class.
     *
     * @param  bmp (Optional) The bitmap, default is #BITMAP_INVALID.
     */
    PainterBWBitmap(const Bitmap& bmp = Bitmap(BITMAP_INVALID))
        : AbstractPainterBW(), bitmapBWPointer(0)
    {
        setBitmap(bmp);
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

    virtual bool renderNext(uint8_t& color);

    const uint8_t* bitmapBWPointer; ///< Pointer to the bitmap (BW)
    LCD1bpp::bwRLEdata bw_rle;      ///< Pointer to class for walking through bw_rle image

    Bitmap bitmap;                ///< The bitmap to be used when painting
    Rect bitmapRectToFrameBuffer; ///< Bitmap rectangle translated to framebuffer coordinates
};

} // namespace touchgfx

#endif // PAINTERBWBITMAP_HPP
