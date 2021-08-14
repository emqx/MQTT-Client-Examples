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
 * @file touchgfx/containers/progress_indicators/ImageProgress.hpp
 *
 * Declares the touchgfx::ImageProgress class.
 */
#ifndef IMAGEPROGRESS_HPP
#define IMAGEPROGRESS_HPP

#include <touchgfx/containers/progress_indicators/AbstractDirectionProgress.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/TiledImage.hpp>

namespace touchgfx
{
/**
 * An image progress will show parts of an image as a progress indicator. The image can progress
 * from the left, the right, the bottom or the top of the given area, and can visually
 * be fixed with a larger and larger portion of the image showing, or it can be moved
 * into view.
 */
class ImageProgress : public AbstractDirectionProgress
{
public:
    ImageProgress();

    virtual void setProgressIndicatorPosition(int16_t x, int16_t y, int16_t width, int16_t height);

    /**
     * Sets anchor at zero.
     *
     * Setting anchor at zero will force the image will be placed so that it is not moved
     * during progress, only more and more of the image will become visible. If the image is
     * not anchored at zero, it will be anchored at the current progress and will appear to
     * slide into view.
     *
     * @param  anchorAtZero true to anchor at zero, false to anchor at current progress.
     *
     * @see getAnchorAtZero
     */
    virtual void setAnchorAtZero(bool anchorAtZero);

    /**
     * Gets the current anchor at zero setting.
     *
     * @return true if the image is anchored at zero, false if it is anchored at current
     *         progress.
     *
     * @see setAnchorAtZero
     */
    virtual bool getAnchorAtZero() const;

    /**
     * Sets the bitmap id to use for progress. Please note that the bitmap is tiled which
     * will allow smaller bitmaps to repeat on the display and save memory.
     *
     * @param  bitmapId The bitmap id.
     *
     * @see getBitmap, TiledImage
     */
    virtual void setBitmap(BitmapId bitmapId);

    /**
     * Gets the bitmap id of the current image.
     *
     * @return The image.
     *
     * @see setBitmap
     */
    virtual BitmapId getBitmap() const;

    /**
     * @copydoc Image::setAlpha
     */
    virtual void setAlpha(uint8_t newAlpha);

    /**
     * @copydoc Image::getAlpha
     */
    virtual uint8_t getAlpha() const;

    virtual void setValue(int value);

protected:
    Container container; ///< The container for the image to allow for anchored images
    TiledImage image;    ///< The image
    bool fixedPosition;  ///< true if the image should not move during progress
};

} // namespace touchgfx

#endif // IMAGEPROGRESS_HPP
