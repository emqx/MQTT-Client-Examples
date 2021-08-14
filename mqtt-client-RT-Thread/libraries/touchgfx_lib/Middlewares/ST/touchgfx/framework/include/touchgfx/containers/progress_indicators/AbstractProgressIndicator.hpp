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
 * @file touchgfx/containers/progress_indicators/AbstractProgressIndicator.hpp
 *
 * Declares the touchgfx::AbstractProgressIndicator class.
 */
#ifndef ABSTRACTPROGRESSINDICATOR_HPP
#define ABSTRACTPROGRESSINDICATOR_HPP

#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Image.hpp>

namespace touchgfx
{
/**
 * The AbstractProgressIndicator declares methods that provides the basic mechanisms and tools
 * to implement a progress indicator. For more specific implementations see classes that
 * inherit from AbstractProgressIndicator.
 *
 * @see BoxProgress, CircleProgress, ImageProgress, LineProgress, TextProgress
 */
class AbstractProgressIndicator : public Container
{
public:
    /** Initializes a new instance of the AbstractProgressIndicator class with a default range 0-100. */
    AbstractProgressIndicator();

    /**
     * Sets the background image. The width and height of the progress indicator widget is
     * updated according to the dimensions of the bitmap.
     *
     * @param  bmpBackground The background bitmap.
     */
    virtual void setBackground(const Bitmap& bmpBackground);

    /**
     * Sets the position and dimensions of the actual progress indicator relative to the
     * background image.
     *
     * @param  x      The x coordinate.
     * @param  y      The y coordinate.
     * @param  width  The width of the box progress indicator.
     * @param  height The height of the box progress indicator.
     *
     * @see getProgressIndicatorX, getProgressIndicatorY, getProgressIndicatorWidth,
     *      getProgressIndicatorHeight
     */
    virtual void setProgressIndicatorPosition(int16_t x, int16_t y, int16_t width, int16_t height);

    /**
     * Gets progress indicator x coordinate.
     *
     * @return The progress indicator x coordinate.
     *
     * @see setProgressIndicatorPosition
     */
    virtual int16_t getProgressIndicatorX() const;

    /**
     * Gets progress indicator y coordinate.
     *
     * @return The progress indicator y coordinate.
     *
     * @see setProgressIndicatorPosition
     */
    virtual int16_t getProgressIndicatorY() const;

    /**
     * Gets progress indicator width.
     *
     * @return The progress indicator width.
     *
     * @see setProgressIndicatorPosition
     */
    virtual int16_t getProgressIndicatorWidth() const;

    /**
     * Gets progress indicator height.
     *
     * @return The progress indicator height.
     *
     * @see setProgressIndicatorPosition
     */
    virtual int16_t getProgressIndicatorHeight() const;

    /**
     * Sets the range for the progress indicator. The range is the values that are given to
     * the progress indicator while progressing through the task at hand. If an app needs to
     * work through 237 items to finish a task, the range should be set to (0, 237) assuming
     * that 0 items is the minimum. Though the minimum is often 0, it is possible to
     * customize this.
     *
     * The steps parameter is used to specify at what granularity you want the progress
     * indicator to report a new progress value. If the 237 items to be reported as 0%,
     * 10%, 20%, ... 100%, the steps should be set to 10 as there are ten steps from 0%
     * to 100%. If you want to update a widget which is 150 pixels wide, you might want to
     * set steps to 150 to get a new progress value for every pixel. If you are updating a
     * clock and want this to resemble an analog clock, you might want to use
     * 12 or perhaps 60 as number of steps.
     *
     * The minStep parameter is used when the minimum input value (min) should give a
     * progress different from 0. For example, if progress is a clock face, you want to
     * count from 0..1000 and you want progress per minute, but want to make sure that 0 is
     * not a blank clock face, but instead you want 1 minute to show, use
     * @code
     *     setRange(0, 1000, 60, 1)
     * @endcode
     * to make sure that as values progress from 0 to 1000, getProgress() start from 1 and
     * goes up to 60. Another example could be a BoxProgress with a TextProgress on top and
     * you want to make sure that "0%" will always show in the box, use something like
     * @code
     *      setRange(0, 1000, 200, 40)
     * @endcode
     * if your box is 200 pixels wide and "0%" is 40 pixels wide.
     *
     * @param  min     The minimum input value.
     * @param  max     The maximum input value.
     * @param  steps   (Optional) The steps in which to report progress.
     * @param  minStep (Optional) The step which the minimum input value is mapped to.
     *
     * @see setValue, getProgress
     */
    virtual void setRange(int16_t min, int16_t max, uint16_t steps = 0, uint16_t minStep = 0);

    /**
     * Gets the range set by setRange().
     *
     * @param [out] min     The minimum input value.
     * @param [out] max     The maximum input value.
     * @param [out] steps   The steps in which to report progress.
     * @param [out] minStep The step which the minimum input value is mapped to.
     *
     * @see setRange
     */
    virtual void getRange(int16_t& min, int16_t& max, uint16_t& steps, uint16_t& minStep) const;

    /**
     * Gets the range set by setRange().
     *
     * @param [out] min   The minimum input value.
     * @param [out] max   The maximum input value.
     * @param [out] steps The steps in which to report progress.
     *
     * @see setRange
     */
    virtual void getRange(int16_t& min, int16_t& max, uint16_t& steps) const;

    /**
     * Gets the range set by setRange().
     *
     * @param [out] min The minimum input value.
     * @param [out] max The maximum input value.
     *
     * @see setRange
     */
    virtual void getRange(int16_t& min, int16_t& max) const;

    /**
     * Sets the current value in the range (min..max) set by setRange(). Values lower than
     * min are mapped to min, values higher than max are mapped to max.
     *
     * @param  value The value.
     * @see getValue
     */
    virtual void setValue(int value);

    /**
     * Gets the current value set by setValue().
     *
     * @return The value.
     *
     * @see setValue
     */
    virtual int getValue() const;

    /**
     * Gets the current progress based on the range set by setRange() and the value set by
     * setValue().
     *
     * @param  range (Optional) The range, default is 100.
     *
     * @return The progress.
     *
     * @see setRange, setValue, getValue
     */
    virtual uint16_t getProgress(uint16_t range = 100) const;

protected:
    Image background;                     ///< The background image
    Container progressIndicatorContainer; ///< The container that holds the actual progress indicator
    int16_t rangeMin;                     ///< The range minimum
    int16_t rangeMax;                     ///< The range maximum
    uint16_t currentValue;                ///< The current value
    uint16_t rangeSteps;                  ///< The range steps
    uint16_t rangeStepsMin;               ///< The range steps minimum
};

} // namespace touchgfx

#endif // ABSTRACTPROGRESSINDICATOR_HPP
