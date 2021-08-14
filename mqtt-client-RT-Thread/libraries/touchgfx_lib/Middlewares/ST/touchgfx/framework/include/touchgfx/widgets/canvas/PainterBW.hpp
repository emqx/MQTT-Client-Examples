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
 * @file touchgfx/widgets/canvas/PainterBW.hpp
 *
 * Declares the touchgfx::PainterBW class.
 */
#ifndef PAINTERBW_HPP
#define PAINTERBW_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/canvas/AbstractPainterBW.hpp>

namespace touchgfx
{
/**
 * PainterBW is used for drawing one 1bpp displays. The color is either on or off No
 * transparency is supported.
 *
 * @see AbstractPainter
 */
class PainterBW : public AbstractPainterBW
{
public:
    /**
     * Converts the selected color to either white (1) or black (0) depending on the gray
     * representation of the RGB color.
     *
     * @param  red   The red color.
     * @param  green The green color.
     * @param  blue  The blue color.
     *
     * @return 1 (white) if the brightness of the RGB color is above 50% and 0 (black)
     *         otherwise.
     */
    static unsigned bw(unsigned red, unsigned green, unsigned blue)
    {
        return (red * 77 + green * 150 + blue * 29) >> 15;
    }

    /**
     * Sets color to use when drawing the CanvasWidget.
     *
     * @param  color The color, 0=black, otherwise white.
     */
    void setColor(colortype color)
    {
        painterColor = color ? 1 : 0;
    }

    /**
     * Gets the current color.
     *
     * @return The color.
     */
    colortype getColor() const
    {
        return static_cast<colortype>(painterColor);
    }

    virtual void render(uint8_t* ptr, int x, int xAdjust, int y, unsigned count, const uint8_t* covers);

protected:
    virtual bool renderNext(uint8_t& color);

    uint8_t painterColor; ///< The color to use when painting
};

} // namespace touchgfx

#endif // PAINTERBW_HPP
