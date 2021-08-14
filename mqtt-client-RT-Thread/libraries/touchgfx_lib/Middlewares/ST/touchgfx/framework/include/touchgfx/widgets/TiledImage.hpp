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
 * @file touchgfx/widgets/TiledImage.hpp
 *
 * Declares the touchgfx::TiledImage class.
 */
#ifndef TILEDIMAGE_HPP
#define TILEDIMAGE_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/Widget.hpp>

namespace touchgfx
{
/**
 * Simple widget capable of showing a bitmap tiled indefinitely horizontally and vertically.
 * This means that when the TiledImage Widget is larger than the provided Bitmap, the
 * Bitmap is repeated over and over horizontally and vertically. The bitmap can be alpha-
 * blended with the background and have areas of transparency.
 */
class TiledImage : public Image
{
public:
    /**
     * Constructs a new TiledImage with a default alpha value of 255 (solid) and a default
     * Bitmap (undefined) if none is specified. If a Bitmap is passed to the constructor,
     * the width and height of this widget is set to those of the bitmap.
     *
     * @param  bmp (Optional) The bitmap to display.
     *
     * @see setBitmap
     */
    TiledImage(const Bitmap& bmp = Bitmap())
        : Image(bmp), xOffset(0), yOffset(0)
    {
    }

    virtual void setBitmap(const Bitmap& bmp);

    /**
     * Sets an offset into the bitmap where the tile drawing should start. By default the
     * first image is aligned along the top and left, i.e. offset at (0, 0).
     *
     * @param  x The x coordinate offset.
     * @param  y The y coordinate offset.
     *
     * @see setXOffset, setYOffset
     */
    virtual void setOffset(int16_t x, int16_t y);

    /**
     * Sets x offset into the bitmap where the tile drawing should start. Setting the x
     * offset to 1 will push all images one pixel to the left.
     *
     * @param  x The x offset.
     *
     * @see setYOffset, setOffset
     */
    virtual void setXOffset(int16_t x);

    /**
     * Sets y offset into the bitmap where the tile drawing should start. Setting the y
     * offset to 1 will push all images one pixel up.
     *
     * @param  y The y offset.
     *
     * @see setXOffset, setOffset
     */
    virtual void setYOffset(int16_t y);

    /**
     * Gets the offset into the bitmap where the tile drawing should start. Please note that
     * the offsets set using setOffset have been normalized so that x is in the range 0 to
     * bitmap width - 1, and y is in the range 0 to bitmap height - 1.
     *
     * @param [out] x The x offset.
     * @param [out] y The y offset.
     *
     * @see getXOffset, getYOffset
     */
    virtual void getOffset(int16_t& x, int16_t& y);

    /**
     * Get x offset. This is the value set using setXOffset() (or setOffset()) normalized to be
     * in the range 0 to bitmap width - 1.
     *
     * @return The x offset.
     *
     * @see getYOffset, getOffset
     */
    virtual int16_t getXOffset();

    /**
     * Get y coordinate offset. This is the value set using setYOffset() (or setOffset())
     * normalized to be in the range 0 to bitmap height - 1.
     *
     * @return The y offset.
     *
     * @see getXOffset, getOffset
     */
    virtual int16_t getYOffset();

    virtual void draw(const Rect& invalidatedArea) const;

    virtual Rect getSolidRect() const;

protected:
    int16_t xOffset; ///< The X offset into the bitmap to start drawing in range 0..bitmap.width-1
    int16_t yOffset; ///< The Y offset into the bitmap to start drawing in range 0..bitmap.height-1
};

} // namespace touchgfx

#endif // TILEDIMAGE_HPP
