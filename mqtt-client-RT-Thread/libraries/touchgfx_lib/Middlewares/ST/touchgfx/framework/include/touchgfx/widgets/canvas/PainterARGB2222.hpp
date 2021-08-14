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
 * @file touchgfx/widgets/canvas/PainterARGB2222.hpp
 *
 * Declares the touchgfx::PainterARGB2222 class.
 */
#ifndef PAINTERARGB2222_HPP
#define PAINTERARGB2222_HPP

#include <stdint.h>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterARGB2222.hpp>

namespace touchgfx
{
/**
 * The PainterARGB2222 class allows a shape to be filled with a given color and alpha
 * value. This allows transparent, anti-aliased elements to be drawn.
 *
 * @see AbstractPainter
 */
class PainterARGB2222 : public AbstractPainterARGB2222
{
public:
    /**
     * Initializes a new instance of the PainterARGB2222 class.
     *
     * @param  color (Optional) the color, default is black.
     * @param  alpha (Optional) the alpha, default is 255 i.e. solid.
     */
    PainterARGB2222(colortype color = 0, uint8_t alpha = 255)
        : AbstractPainterARGB2222()
    {
        setColor(color);
        setAlpha(alpha);
    }

    /**
     * Sets color and alpha to use when drawing the CanvasWidget.
     *
     * @param  color The color.
     */
    void setColor(colortype color)
    {
        painterColor = color;
        painterRed = LCD8bpp_ARGB2222::getRedFromColor(color);
        painterGreen = LCD8bpp_ARGB2222::getGreenFromColor(color);
        painterBlue = LCD8bpp_ARGB2222::getBlueFromColor(color);
    }

    /**
     * Gets the current color.
     *
     * @return The color.
     */
    colortype getColor() const
    {
        return painterColor;
    }

    virtual void render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers);

protected:
    virtual bool renderNext(uint8_t& red, uint8_t& green, uint8_t& blue, uint8_t& alpha);

    uint8_t painterColor; ///< The color
    uint8_t painterRed;   ///< The red part of the color, scaled up to [0..255]
    uint8_t painterGreen; ///< The green part of the color, scaled up to [0..255]
    uint8_t painterBlue;  ///< The blue part of the color, scaled up to [0..255]
};

} // namespace touchgfx

#endif // PAINTERARGB2222_HPP
