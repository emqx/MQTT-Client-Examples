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
 * @file touchgfx/mixins/PreRenderable.hpp
 *
 * Declares the touchgfx::PreRenderable class.
 */
#ifndef PRERENDERABLE_HPP
#define PRERENDERABLE_HPP

#include <touchgfx/Drawable.hpp>
#include <touchgfx/hal/HAL.hpp>

namespace touchgfx
{
///@cond
/**
 * This mix-in can be used on any Drawable. It provides a preRender function, which will cache
 * the current visual appearance of the Drawable in a cache in a memory region.
 * Subsequent calls to draw() on this Drawable will result in a simple copying of the
 * cached memory to the framebuffer instead of the normal draw operation. This mix-in
 * can therefore be used on Drawables whose visual appearance is static and the normal
 * draw operation takes a long time to perform.
 *
 * @tparam T specifies the type to extend with the FadeAnimator behavior.
 *
 * @note The actual uses of this mix-in are rare, and the class is mainly provided for example
 *       purposes.
 * @note The Drawable is cached in animation storage, which must be available
 *
 * @deprecated Consult CacheableContainer or SnapshotWidget
 */
template <class T>
class
    PreRenderable : public T
{
public:
    /// @cond
    TOUCHGFX_DEPRECATED(
        "Consult CacheableContainer or SnapshotWidget as an alternative to PreRenderable",
        PreRenderable())
        : preRenderedAddress(0)
    {
    }
    ///@endcond

    /**
     * Overrides the draw function. If preRender() has been called, perform a memcpy of the
     * cached version. If not, just call the base class version of draw.
     *
     * @param  invalidatedArea The subregion of this Drawable which needs to be redrawn.
     */
    void draw(const Rect& invalidatedArea) const
    {
        if (isPreRendered())
        {
            Rect dirty = invalidatedArea;
            Rect meAbs = T::getRect();

            uint16_t width = T::rect.width;
            int cols = dirty.width;
            int rows = dirty.height;
            int offsetPos = dirty.x + width * dirty.y;
            uint16_t* src = (uint16_t*)preRenderedAddress;

            HAL::getInstance()->blitCopy(src + offsetPos, meAbs.x + dirty.x, meAbs.y + dirty.y, cols, rows, width, 255, false);
        }
        else
        {
            T::draw(invalidatedArea);
        }
    }

    /// @cond
    /**
     * Add to draw chain.
     *
     * @param          invalidatedArea     Include drawables that intersect with this area only.
     * @param [in,out] nextPreviousElement Modifiable element in linked list.
     *
     * @note For TouchGFX internal use only.
     */
    virtual void setupDrawChain(const Rect& invalidatedArea, Drawable** nextPreviousElement)
    {
        T::resetDrawChainCache();
        if (isPreRendered())
        {
            if (!T::isVisible())
            {
                return;
            }
            T::nextDrawChainElement = *nextPreviousElement;
            *nextPreviousElement = this;
        }
        else
        {
            T::setupDrawChain(invalidatedArea, nextPreviousElement);
        }
    }
    /// @endcond

    /// @cond
    /**
     * Whether or not the snapshot of the widget been taken.
     *
     * @return Is the widget rendered.
     */
    TOUCHGFX_DEPRECATED(
        "Consult CacheableContainer or SnapshotWidget as an alternative to PreRenderable",
        bool isPreRendered() const)
    {
        return preRenderedAddress != 0;
    }
    /// @endcond

    /// @cond
    /**
     * Takes a snapshot of the current visual appearance of this widget. All subsequent
     * calls to draw on this mix-in will result in the snapshot being draw.
     */
    TOUCHGFX_DEPRECATED(
        "Consult CacheableContainer or SnapshotWidget as an alternative to PreRenderable",
        void preRender())
    {
        if (HAL::getInstance()->getBlitCaps() & BLIT_OP_COPY)
        {
            Rect meAbs = T::getRect();
            T::translateRectToAbsolute(meAbs);
            preRenderedAddress = HAL::getInstance()->copyFBRegionToMemory(meAbs);
        }
    }
    /// @endcond

private:
    uint16_t* preRenderedAddress;
};
///@endcond
} // namespace touchgfx

#endif // PRERENDERABLE_HPP
