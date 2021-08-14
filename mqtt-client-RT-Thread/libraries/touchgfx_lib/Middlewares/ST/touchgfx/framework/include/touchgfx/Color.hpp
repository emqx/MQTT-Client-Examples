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
 * @file touchgfx/Color.hpp
 *
 * Declares the touchgfx::Color class
 */
#ifndef COLOR_HPP
#define COLOR_HPP

#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/lcd/LCD.hpp>

namespace touchgfx
{
/** Contains functionality for color conversion. */
class Color
{
public:
    /**
     * Generates a color representation to be used on the LCD, based on 24 bit RGB values.
     * Depending on your display color bit depth, the color might be interpreted internally
     * as fewer than 24 bits with a loss of color precision.
     *
     * @param  red   Value of the red part (0-255).
     * @param  green Value of the green part (0-255).
     * @param  blue  Value of the blue part (0-255).
     *
     * @return The color representation depending on LCD color format.
     *
     * @see LCD::getColorFrom24BitRGB, LCD16bpp::getColorFromRGB
     *
     * @note This function is not available to call before the LCD has been setup, because the
     *       color depth is required. Consider using the function getColorFromRGB for a
     *       specific class, e.g. LCD16::getColorFromRGB().
     */
    static colortype getColorFrom24BitRGB(uint8_t red, uint8_t green, uint8_t blue)
    {
        assert(HAL::getInstance() && "Cannot set color before HAL is initialized");
        return HAL::lcd().getColorFrom24BitRGB(red, green, blue);
    }

    /**
     * Gets the red color part of a color. As this function must work for all color depths,
     * it can be somewhat slow if used in speed critical sections. Consider finding the
     * color in another way, if possible. If the color depth of the display is known,
     * consider using function getRedFromColor() from the current LCD.
     *
     * @param  color The color value.
     *
     * @return The red part of the color.
     *
     * @see LCD16bpp::getRedFromColor
     */
    FORCE_INLINE_FUNCTION static uint8_t getRedColor(colortype color)
    {
        return HAL::lcd().getRedColor(color);
    }

    /**
     * Gets the green color part of a color. As this function must work for all color depths,
     * it can be somewhat slow if used in speed critical sections. Consider finding the
     * color in another way, if possible. If the color depth of the display is known,
     * consider using function getGreenFromColor() from the current LCD.
     *
     * @param  color The color value.
     *
     * @return The green part of the color.
     *
     * @see LCD16bpp::getGreenFromColor
     */
    FORCE_INLINE_FUNCTION static uint8_t getGreenColor(colortype color)
    {
        return HAL::lcd().getGreenColor(color);
    }

    /**
     * Gets the blue color part of a color. As this function must work for all color depths,
     * it can be somewhat slow if used in speed critical sections. Consider finding the
     * color in another way, if possible. If the color depth of the display is known,
     * consider using function getBlueFromColor() from the current LCD.
     *
     * @param  color The color value.
     *
     * @return The blue part of the color.
     *
     * @see LCD16bpp::getBlueFromColor
     */
    FORCE_INLINE_FUNCTION static uint8_t getBlueColor(colortype color)
    {
        return HAL::lcd().getBlueColor(color);
    }
};

} // namespace touchgfx

#endif // COLOR_HPP
