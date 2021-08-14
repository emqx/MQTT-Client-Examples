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
 * @file touchgfx/containers/CacheableContainer.hpp
 *
 * Declares the touchgfx::CacheableContainer class.
 */
#ifndef CACHEABLECONTAINER_HPP
#define CACHEABLECONTAINER_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Image.hpp>

namespace touchgfx
{
/**
 * A CacheableContainer can be seen as a regular Container, i.e. a Drawable that can have child
 * nodes. The z-order of children is determined by the order in which Drawables are
 * added to the container - the Drawable added last will be front-most on the screen.
 *
 * The important difference is that a CacheableContainer can also render its content to
 * a dynamic bitmap which can then be used as a texture in subsequent drawing operations,
 * either as a simple Image or in a TextureMapper. If the bitmap format of the dynamic
 * bitmap differs from the format of the current LCD, the LCD from drawing the bitmap
 * must be setup using HAL::setAuxiliaryLCD().
 *
 * @see Container, Bitmap, Image, TextureMapper
 */
class CacheableContainer : public Container
{
public:
    /**
     * Set the dynamic bitmap into which the container content will be rendered. The format
     * of the bitmap must be the same as the current LCD or the same as the auxiliary LCD
     * setup using HAL::setAuxiliaryLCD.
     *
     * @param  bitmapId Id of the dynamic bitmap to serve as a render target.
     *
     * @see updateCache, HAL::setAuxiliaryLCD
     */
    void setCacheBitmap(BitmapId bitmapId);

    /**
     * Render the container into the attached dynamic bitmap.
     *
     * @see setCacheBitmap
     */
    void updateCache();

    /**
     * Render the container into the attached dynamic bitmap. Only the specified Rect region
     * is updated.
     *
     * @param  rect Region to update.
     *
     * @see setCacheBitmap
     */
    void updateCache(const Rect& rect);

    /**
     * Toggle cached mode on and off. The CacheableContainer behaves just like a regular
     * Container when cached mode is turned off.
     *
     * @param  enable Enable or disable cached mode.
     */
    void enableCachedMode(bool enable);

    /**
     * Request that a subregion of this drawable is redrawn. Will recursively traverse the
     * children. When this function returns, the specified invalidated area has been redrawn
     * for all appropriate Drawables covering the region.
     *
     * @param [in] invalidatedArea The area of this drawable to redraw expressed in coordinates
     *                             relative to its parent (e.g. to request a complete
     *                             redraw, invalidatedArea will be (0, 0, width, height).
     */
    virtual void invalidateRect(Rect& invalidatedArea) const;

    /**
     * Queries the CacheableContainer whether any child widget has been invalidated.
     *
     * @return True if a child widget has been invalidated and false otherwise.
     */
    bool isChildInvalidated() const;

protected:
    /// @cond
    virtual void setupDrawChain(const Rect& invalidatedArea, Drawable** nextPreviousElement);

    /**
     * A CachedImage is a specialized Image object that exposes the setupDrawChain() method.
     *
     * @see CacheableContainer, Image
     */
    class CachedImage : public Image
    {
    public:
        virtual void setupDrawChain(const Rect& invalidatedArea, Drawable** nextPreviousElement)
        {
            Drawable::setupDrawChain(invalidatedArea, nextPreviousElement);
        }
    };
    /// @endcond

private:
    BitmapId cachedBitmapId;  ///< The BitmapId of the dynamic bitmap attached to the CacheableContainer
    CachedImage cachedImage;  ///< The CachedImage object used as a wrapper widget around the attached dynamic bitmap
    bool isCachedMode;        ///< Cached mode whether enabled or disabled
    bool childWasInvalidated; ///< A child widget has been invalidated. The flag is meaningful when isCachedMode is true.
};

} // namespace touchgfx

#endif // CACHEABLECONTAINER_HPP
