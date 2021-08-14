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
 * @file touchgfx/containers/buttons/TiledImageButtonStyle.hpp
 *
 * Declares the touchgfx::TiledImageButtonStyle class.
 */
#ifndef TILEDIMAGEBUTTONSTYLE_HPP
#define TILEDIMAGEBUTTONSTYLE_HPP

#include <touchgfx/widgets/TiledImage.hpp>

namespace touchgfx
{
/**
 * A tiled image button style.
 *
 * An tiled image button style. This class is supposed to be used with one of the
 * ButtonTrigger classes to create a functional button. This class will show one of two
 * tiled images depending on the state of the button (pressed or released).
 *
 * The TiledImageButtonStyle does not set the size of the enclosing container (normally
 * AbstractButtonContainer) to the size of the pressed Bitmap. This can be overridden by
 * calling setWidth/setHeight after setting the bitmaps.
 *
 * @tparam T Generic type parameter. Typically a AbstractButtonContainer subclass.
 *
 * @see AbstractButtonContainer
 */
template <class T>
class TiledImageButtonStyle : public T
{
public:
    TiledImageButtonStyle()
        : T()
    {
        tiledImage.setXY(0, 0);
        T::add(tiledImage);
    }

    /**
     * Sets width.
     *
     * @param  width The width.
     */
    virtual void setWidth(int16_t width)
    {
        tiledImage.setWidth(width);
        T::setWidth(width);
    }

    /**
     * Sets height.
     *
     * @param  height The height.
     */
    virtual void setHeight(int16_t height)
    {
        tiledImage.setHeight(height);
        T::setHeight(height);
    }

    /**
     * Sets tile bitmaps.
     *
     * @param  bmpReleased The bitmap released.
     * @param  bmpPressed  The bitmap pressed.
     */
    virtual void setTileBitmaps(const Bitmap& bmpReleased, const Bitmap& bmpPressed)
    {
        upTile = bmpReleased;
        downTile = bmpPressed;
        AbstractButtonContainer::setWidth(downTile.getWidth());
        AbstractButtonContainer::setHeight(downTile.getHeight());

        handlePressedUpdated();
    }

    /**
     * Sets an offset into the bitmap where the tile drawing should start.
     *
     * @param  x The x coordinate offset.
     * @param  y The y coordinate offset.
     * @see TiledImage::setOffset
     */
    virtual void setTileOffset(int16_t x, int16_t y)
    {
        tiledImage.setOffset(x, y);
    }

protected:
    TiledImage tiledImage; ///< The tiled image
    Bitmap upTile;         ///< The image to display when button is released.
    Bitmap downTile;       ///< The image to display when button is pressed.

    /** @copydoc AbstractButtonContainer::handlePressedUpdated() */
    virtual void handlePressedUpdated()
    {
        int16_t buttonWidth = AbstractButtonContainer::getWidth();
        int16_t buttonHeight = AbstractButtonContainer::getHeight();

        tiledImage.setBitmap(T::getPressed() ? downTile : upTile);
        tiledImage.setWidth(buttonWidth);
        tiledImage.setHeight(buttonHeight);
        T::handlePressedUpdated();
    }

    /** @copydoc AbstractButtonContainer::handleAlphaUpdated() */
    virtual void handleAlphaUpdated()
    {
        tiledImage.setAlpha(T::getAlpha());
        T::handleAlphaUpdated();
    }
};

} // namespace touchgfx

#endif // TILEDIMAGEBUTTONSTYLE_HPP
