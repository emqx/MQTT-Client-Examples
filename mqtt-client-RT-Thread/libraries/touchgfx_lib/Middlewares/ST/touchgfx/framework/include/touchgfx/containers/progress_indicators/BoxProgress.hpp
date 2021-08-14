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
 * @file touchgfx/containers/progress_indicators/BoxProgress.hpp
 *
 * Declares the touchgfx::BoxProgress class.
 */
#ifndef BOXPROGRESS_HPP
#define BOXPROGRESS_HPP

#include <touchgfx/containers/progress_indicators/AbstractDirectionProgress.hpp>
#include <touchgfx/widgets/Box.hpp>

namespace touchgfx
{
/**
 * A BoxProgress which shows the current progress using a simple Box. It is possible to set the
 * color and the alpha of the box. It is also possible to control in what direction the
 * box will progress (up, down, to the left or to the right).
 */
class BoxProgress : public AbstractDirectionProgress
{
public:
    BoxProgress();

    virtual void setProgressIndicatorPosition(int16_t x, int16_t y, int16_t width, int16_t height);

    /**
     * Sets the color of the Box.
     *
     * @param  color The color.
     * @see getColor
     */
    virtual void setColor(colortype color);

    /**
     * Gets the color of the Box.
     *
     * @return The color.
     *
     * @see setColor
     */
    virtual colortype getColor() const;

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
    Box box; ///< The box
};

} // namespace touchgfx

#endif // BOXPROGRESS_HPP
