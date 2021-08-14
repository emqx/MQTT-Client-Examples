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
 * @file touchgfx/containers/progress_indicators/AbstractDirectionProgress.hpp
 *
 * Declares the touchgfx::AbstractDirectionProgress class.
 */
#ifndef ABSTRACTDIRECTIONPROGRESS_HPP
#define ABSTRACTDIRECTIONPROGRESS_HPP

#include <touchgfx/containers/progress_indicators/AbstractProgressIndicator.hpp>

namespace touchgfx
{
/**
 * An abstract class for progress indicators that need a horizontal or vertical direction to be
 * specified.
 */
class AbstractDirectionProgress : public AbstractProgressIndicator
{
public:
    /** Values that represent directions. */
    typedef enum
    {
        RIGHT, ///< Progress should be from left to right
        LEFT,  ///< Progress should be from right to left
        DOWN,  ///< Progress should be down (top to bottom)
        UP     ///< Progress should be up (bottom to top)
    } DirectionType;

    AbstractDirectionProgress();

    /**
     * Sets a direction for the progress indicator. This will re-calculate the current value
     * according to the new direction.
     *
     * @param  direction The direction.
     *
     * @see getDirection
     */
    virtual void setDirection(DirectionType direction);

    /**
     * Gets the current direction for the progress indicator.
     *
     * @return The direction.
     *
     * @see setDirection
     */
    virtual DirectionType getDirection() const;

protected:
    DirectionType progressDirection; ///< The progress direction
};

} // namespace touchgfx

#endif // ABSTRACTDIRECTIONPROGRESS_HPP
