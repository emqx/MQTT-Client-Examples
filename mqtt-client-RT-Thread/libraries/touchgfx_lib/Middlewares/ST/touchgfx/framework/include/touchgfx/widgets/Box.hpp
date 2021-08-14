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
 * @file touchgfx/widgets/Box.hpp
 *
 * Declares the touchgfx::Box class.
 */
#ifndef BOX_HPP
#define BOX_HPP

#include <touchgfx/Bitmap.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/widgets/Widget.hpp>

#include <touchgfx/widgets/Button.hpp>

namespace touchgfx
{
/**
 * Simple widget capable of showing a rectangle of a specific color and an optional alpha.
 */
class Box : public Widget
{
public:
    /** Construct a new Box with a default alpha value of 255 (solid) */
    Box()
        : Widget(), alpha(255), color(0), reportAsSolid(false)
    {
    }

    /**
     * Construct a Box with the given size and color (and optionally alpha).
     *
     * @param  width  The width of the box.
     * @param  height The height of the box.
     * @param  color  The color of the box.
     * @param  alpha  (Optional) The alpha of the box. Default is 255 (solid).
     */
    Box(uint16_t width, uint16_t height, colortype color, uint8_t alpha = 255)
        : Widget(),
          alpha(alpha), color(color), reportAsSolid(false)
    {
        rect.width = width;
        rect.height = height;
    }

    virtual Rect getSolidRect() const;

    /**
     * Sets the color of the Box.
     *
     * @param  color The color of the box.
     *
     * @see getColor, Color::getColorFrom24BitRGB
     */
    void setColor(colortype color)
    {
        this->color = color;
    }

    /**
     * Gets the current color of the Box.
     *
     * @return The current color of the box.
     *
     * @see setColor, Color::getRedColor, Color::getGreenColor, Color::getRedColor
     */
    FORCE_INLINE_FUNCTION colortype getColor() const
    {
        return color;
    }

    /**
     * Sets the alpha value for the Box. The value can be from 255 (completely solid) to 0
     * (completely transparent).
     *
     * @param  alpha The alpha value.
     *
     * @see getAlpha
     */
    void setAlpha(uint8_t alpha)
    {
        this->alpha = alpha;
    }

    /**
     * Returns the current alpha value.
     *
     * @return Gets the current alpha value of the Box.
     *
     * @see setAlpha
     */
    FORCE_INLINE_FUNCTION uint8_t getAlpha() const
    {
        return alpha;
    }

    virtual void draw(const Rect& area) const;

    ///@cond
    /**
     * Override solid area for the Box.
     *
     * If this is set, getSolidRect() will report the widget as completely solid even if is
     * (semi-)transparent. The use of forceReportAsSolid is deprecated and the same
     * functionality can be achieved using the following SolidBox instead:
     * @code
     *      template <class T>
     *      class Solid : public T
     *      {
     *      public:
     *        virtual Rect getSolidRect() const
     *        {
     *           return Rect(0, 0, getWidth(), getHeight());
     *        }
     *      };
     * @endcode
     * This will allow the use of Solid<Box>, but this is still not recommended..
     *
     * @param  solid true if this Box should report as solid, even when not.
     *
     * @deprecated Breaks the invariant set in Drawable::getSolidRect().
     */
    TOUCHGFX_DEPRECATED(
        "Breaks the invariant set in Drawable::getSolidRect().",
        void forceReportAsSolid(bool solid))
    {
        reportAsSolid = solid;
    }
    ///@endcond

protected:
    uint8_t alpha;      ///< The alpha value used for this Box.
    colortype color;    ///< The fill color for this Box
    bool reportAsSolid; ///< @deprecated Used by forceReportAsSolid() which breaks TouchGFX invariant.
};

} // namespace touchgfx

#endif // BOX_HPP
