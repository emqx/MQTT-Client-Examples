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
 * @file touchgfx/containers/Slider.hpp
 *
 * Declares the touchgfx::Slider class.
 */
#ifndef SLIDER_HPP
#define SLIDER_HPP

#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Image.hpp>

namespace touchgfx
{
/**
 * A slider is a graphical element with which the user may set a value by moving an indicator on
 * a slider, or simply by clicking the slider. The slider can operate in horizontal or
 * vertical mode. The slider has two bitmaps. One bitmap is used on one side of the
 * indicator. The other is used on the other side. They can be used in indicating the
 * part of the slider value range that is currently selected.
 *
 * The slider operates on an integer value range that can be set by the user.
 */
class Slider : public Container
{
public:
    Slider();

    /**
     * Sets all the bitmaps for the Slider. The Slider shows the sliderBackgroundSelected
     * bitmap in the region of the Slider that is selected, that is the area to the left of
     * the indicator for a horizontal Slider and below the indicator for a vertical Slider.
     * The sliderBackground is shown on the other side of the Slider. To ignore this effect
     * simply use the same bitmap for both the sliderBackground and the
     * sliderBackgroundSelected.
     *
     * @param  sliderBackground         The slider background with the slider range
     *                                  unselected.
     * @param  sliderBackgroundSelected The slider background with the slider range selected.
     * @param  indicator                The indicator.
     */
    void setBitmaps(const Bitmap& sliderBackground, const Bitmap& sliderBackgroundSelected, const Bitmap& indicator);

    /**
     * Sets all the bitmaps for the Slider. The Slider shows the sliderBackgroundSelected
     * bitmap in the region of the Slider that is selected, that is the area to the left of
     * the indicator for a horizontal Slider and below the indicator for a vertical Slider.
     * The sliderBackground is shown on the other side of the Slider. To ignore this effect
     * simply use the same bitmap for both the sliderBackground and the
     * sliderBackgroundSelected.
     *
     * @param  sliderBackground         The slider background with the slider range
     *                                  unselected.
     * @param  sliderBackgroundSelected The slider background with the slider range selected.
     * @param  indicator                The indicator.
     */
    void setBitmaps(const BitmapId sliderBackground, const BitmapId sliderBackgroundSelected, const BitmapId indicator);

    /**
     * Associates an action to be performed when an interaction with the slider is initiated
     * (click or drag).
     *
     * @param  callback The callback to be executed. The callback will be given a reference
     *                  to the Slider and the current value of the slider at interaction
     *                  start.
     *
     * @see GenericCallback
     */
    void setStartValueCallback(GenericCallback<const Slider&, int>& callback)
    {
        startValueCallback = &callback;
    }

    /**
     * Associates an action to be performed when an interaction with the slider ends (click
     * or drag).
     *
     * @param  callback The callback to be executed. The callback will be given a reference
     *                  to the Slider and the current value of the slider at interaction end.
     *
     * @see GenericCallback
     */
    void setStopValueCallback(GenericCallback<const Slider&, int>& callback)
    {
        stopValueCallback = &callback;
    }

    /**
     * Associates an action to be performed when the slider changes its value.
     *
     * @param  callback The callback to be executed. The callback will be given a reference
     *                  to the Slider and the current value of the slider.
     *
     * @see GenericCallback
     */
    void setNewValueCallback(GenericCallback<const Slider&, int>& callback)
    {
        newValueCallback = &callback;
    }

    /**
     * Sets up the slider in horizontal mode with the range going from the left to right.
     *
     * Places the backgrounds and the indicator inside the Slider container. It is possible
     * to place the end points of the indicator outside the background image if it needs to
     * go beyond the boundaries of the background. The width and height of the Slider will
     * be adjusted appropriately so that both the background and the indicator will be fully
     * visible in both the minimum and maximum indicator positions.
     *
     * Calls setValue() with the current value (default 0) and triggers the newSliderValue
     * callback.
     *
     * @param  backgroundX   The background x coordinate inside the slider.
     * @param  backgroundY   The background y coordinate inside the slider.
     * @param  indicatorY    The indicator y coordinate inside the slider.
     * @param  indicatorMinX The indicator minimum x coordinate inside the slider. This is the
     *                       position used when the slider is at its minimum value. Must
     *                       be less than indicatorMaxX.
     * @param  indicatorMaxX The indicator maximum x coordinate inside the slider. This is the
     *                       position used when the slider is at its maximum value. Must
     *                       be greater than indicatorMinX.
     *
     * @note The x and y position of the Slider will either be the left/top of the background or
     *       the left/top of the indicator in its minimum x coordinate.
     */
    virtual void setupHorizontalSlider(uint16_t backgroundX, uint16_t backgroundY, uint16_t indicatorY, uint16_t indicatorMinX, uint16_t indicatorMaxX);

    /**
     * Sets up the slider in vertical mode with the range going from the bottom to top.
     *
     * Places the backgrounds and the indicator inside the Slider container. It is possible
     * to place the end points of the indicator outside the background image if it needs to
     * go beyond the boundaries of the background. The width and height of the Slider will
     * be adjusted appropriately so that both the background and the indicator will be fully
     * visible in both the minimum and maximum indicator positions.
     *
     *
     * Calls setValue with the current value (default 0) and triggers the newSliderValue
     * callback.
     *
     * @param  backgroundX   The background x coordinate inside the slider.
     * @param  backgroundY   The background y coordinate inside the slider.
     * @param  indicatorX    The indicator x coordinate inside the slider.
     * @param  indicatorMinY The indicator minimum y coordinate inside the slider. This is the
     *                       position used when the slider is at its maximum value. Must
     *                       be less than indicatorMaxX.
     * @param  indicatorMaxY The indicator maximum y coordinate inside the slider. This is the
     *                       position used when the slider is at its minimum value. Must
     *                       be greater than indicatorMinX.
     *
     * @note The x and y position of the Slider will either be the left/top of the background or
     *       the left/top of the indicator in its minimum y coordinate.
     */
    virtual void setupVerticalSlider(uint16_t backgroundX, uint16_t backgroundY, uint16_t indicatorX, uint16_t indicatorMinY, uint16_t indicatorMaxY);

    /**
     * Gets indicator minimum previously set using setupHorizontalSlider() or
     * setupVerticalSlider().
     *
     * @return The indicator minimum.
     *
     * @see setupHorizontalSlider, setupVerticalSlider, getIndicatorMax
     */
    virtual uint16_t getIndicatorMin() const
    {
        return indicatorMaxPosition;
    }

    /**
     * Gets indicator maximum previous set using setupHorizontalSlider() or
     * setupVerticalSlider().
     *
     * @return The calculated indicator maximum.
     *
     * @see setupHorizontalSlider, setupVerticalSlider, getIndicatorMin
     */
    virtual uint16_t getIndicatorMax() const
    {
        return indicatorMaxPosition;
    }

    /**
     * Sets the value range of the slider. Values accepted and returned by the slider will
     * be in this range.
     *
     * The slider will set its value to the specified new value.
     *
     * @param  minValue The minimum value. Must be less than maxValue.
     * @param  maxValue The maximum value. Must be greater than minValue.
     * @param  newValue The new value.
     *
     * @note If the range is larger than the number of pixels specified for the indicator min and
     *       max some values will not be represented by the slider.
     */
    virtual void setValueRange(int minValue, int maxValue, int newValue);

    /**
     * Gets the minimum value previously set using setValueRange().
     *
     * @return The minimum value.
     *
     * @see setValueRange, getMaxValue
     */
    virtual uint16_t getMinValue() const
    {
        return valueRangeMin;
    }

    /**
     * Gets the maximum value previously set using setValueRange().
     *
     * @return The maximum value.
     *
     * @see setValueRange, getMinValue
     */
    virtual uint16_t getMaxValue() const
    {
        return valueRangeMax;
    }

    /**
     * Sets the value range of the slider. Values accepted and returned by the slider will
     * be in this range.
     *
     * The slider will set its value to the current value or round to minValue or maxValue
     * if the current value is outside the new range.
     *
     * @param  minValue The minimum value. Must be less than maxValue.
     * @param  maxValue The maximum value. Must be greater than minValue.
     *
     * @note If the range is larger than the number of pixels specified for the indicator min and
     *       indicator max, some values will not be represented by the slider.
     */
    virtual void setValueRange(int minValue, int maxValue);

    /**
     * Places the indicator at the specified value relative to the specified value range.
     * Values beyond the value range will be rounded to the min/max value in the value range.
     *
     * @param  value The value.
     *
     * @see setValueRange
     *
     * @note The value update triggers a newSliderValue callback just as a drag or click does.
     * @note If the value range is larger than the number of pixels specified for the indicator
     *       min and indicator max, some values will not be represented by the slider and
     *       thus is not possible to set with this method. In this case the value will be
     *       rounded to the nearest value that is represented in the current setting.
     */
    virtual void setValue(int value);

    /**
     * Gets the current value represented by the indicator.
     *
     * @return The current value.
     */
    int getValue()
    {
        return currentValue;
    }

protected:
    /** Values that represent slider orientations. */
    enum SliderOrientation
    {
        HORIZONTAL, ///< The Slider can be moved horizontally between left and right
        VERTICAL    ///< The Slider can be moved vertically between top and bottom
    };

    SliderOrientation sliderOrientation; ///< The selected slider orientation

    int currentValue; ///< The current value represented by the slider

    int valueRangeMin; ///< The value range min
    int valueRangeMax; ///< The value range max

    Image background;                     ///< The background image
    Image backgroundSelected;             ///< The backgroundSelected image
    Image indicator;                      ///< The indicator image
    Container backgroundSelectedViewPort; ///< The backgroundSelected view port. Controls the visible part of the backgroundSelected image.

    int16_t indicatorMinPosition; ///< The minimum position of the indicator (either x coordinate in horizontal mode or y coordinate in vertical mode)
    int16_t indicatorMaxPosition; ///< The maximum position of the indicator (either x coordinate in horizontal mode or y coordinate in vertical mode)

    GenericCallback<const Slider&, int>* startValueCallback; ///< The start value callback (called  when an interaction with the indicator is initiated)
    GenericCallback<const Slider&, int>* stopValueCallback;  ///< The stop value callback (called when an interaction with the indicator ends)
    GenericCallback<const Slider&, int>* newValueCallback;   ///< The new value callback (called when the indicator is moved)

    virtual void handleClickEvent(const ClickEvent& evt);

    virtual void handleDragEvent(const DragEvent& evt);

    /**
     * Updates the indicator position described by position. Calls the
     * newSliderValueCallback with the new value.
     *
     * @param  position The position (x coordinate in horizontal mode and y coordinate in
     *                  vertical mode).
     */
    virtual void updateIndicatorPosition(int16_t position);

    /**
     * Translate a value in the value range to the corresponding position in the indicator
     * position range (x coordinate in horizontal mode and y in vertical mode).
     *
     * @param  value The value.
     *
     * @return The coordinate that corresponds to the value.
     */
    virtual int16_t valueToPosition(int value) const;

    /**
     * Translate a position (x coordinate in horizontal mode and y in vertical mode) in the
     * indicator position range to the corresponding value in the value range.
     *
     * @param  position The position.
     *
     * @return The value that corresponds to the coordinate.
     */
    virtual int positionToValue(int16_t position) const;

    /**
     * Gets the indicator radius, which is half the size of the indicator.
     *
     * @return The the indicator radius.
     */
    virtual uint16_t getIndicatorRadius() const;

    /**
     * Gets the indicator position range, i.e. the difference between max and min for the
     * position of the indicator.
     *
     * @return The indicator position range.
     */
    virtual int getIndicatorPositionRangeSize() const;

    /**
     * Gets the value range, i.e. the difference between max and min for the value range.
     *
     * @return The value range.
     */
    virtual int getValueRangeSize() const;
};

} // namespace touchgfx

#endif // SLIDER_HPP
