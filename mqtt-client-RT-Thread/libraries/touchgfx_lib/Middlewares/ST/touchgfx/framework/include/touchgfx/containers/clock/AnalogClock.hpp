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
 * @file touchgfx/containers/clock/AnalogClock.hpp
 *
 * Declares the touchgfx::AnalogClock class.
 */
#ifndef ANALOGCLOCK_HPP
#define ANALOGCLOCK_HPP

#include <touchgfx/EasingEquations.hpp>
#include <touchgfx/containers/clock/AbstractClock.hpp>
#include <touchgfx/widgets/AnimationTextureMapper.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/TextureMapper.hpp>

namespace touchgfx
{
/**
 * An analog clock. Should be supplied with images for the background, hour hand, minute hand
 * and the optional second hand. You setup the AnalogClock by specifying the rotation
 * point of each hand as well as the global rotation point of the clock. You can
 * customize the behavior of the AnalogClock in respect to animations and relations
 * between the hands e.g. if the hour hand should move gradually towards the next hour
 * as the minute hand progresses (setHourHandMinuteCorrection())
 */
class AnalogClock : public AbstractClock
{
public:
    AnalogClock();

    /**
     * Sets the background image of the clock. The clock rotation center is automatically
     * set to the background image center. The clock rotation center is the point that the
     * clock hands rotates around. The size of the AnalocClock widget is set to the size of
     * the bitmap.
     *
     * @param  backgroundBitmapId Identifier for the background bitmap.
     */
    virtual void setBackground(const BitmapId backgroundBitmapId);

    /**
     * Sets the background image of the clock and the rotation center of the clock. The
     * clock rotation center is the point that the clock hands rotates around. The size of
     * the AnalocClock widget is set to the size of the bitmap.
     *
     * @param  backgroundBitmapId Identifier for the background bitmap.
     * @param  rotationCenterX    The rotation center x coordinate.
     * @param  rotationCenterY    The rotation center y coordinate.
     */
    virtual void setBackground(const BitmapId backgroundBitmapId, int16_t rotationCenterX, int16_t rotationCenterY);

    /**
     * Sets the rotation center of the clock. The clock rotation center is the point that
     * the clock hands rotates around.
     *
     * @param  rotationCenterX The rotation center x coordinate.
     * @param  rotationCenterY The rotation center y coordinate.
     */
    virtual void setRotationCenter(int16_t rotationCenterX, int16_t rotationCenterY);

    /**
     * Sets up the hour hand. The specified rotation center is the point of the hand that is
     * to be placed on top of the clock rotation center. That is the point that the hand
     * rotates around. The rotation point is relative to the supplied bitmap and can be
     * placed outside of it.
     *
     * @param  hourHandBitmapId Identifier for the hour hand bitmap.
     * @param  rotationCenterX  The hand rotation center x coordinate.
     * @param  rotationCenterY  The hand rotation center y coordinate.
     *
     * @note If no hour hand is setup it will just be omitted.
     */
    virtual void setupHourHand(const BitmapId hourHandBitmapId, int16_t rotationCenterX, int16_t rotationCenterY);

    /**
     * Sets up the minute hand. The specified rotation center is the point of the hand that
     * is to be placed on top of the clock rotation center. That is the point that the hand
     * rotates around. The rotation point is relative to the supplied bitmap but can be
     * placed outside of it.
     *
     * @param  minuteHandBitmapId Identifier for the minute hand bitmap.
     * @param  rotationCenterX    The hand rotation center x coordinate.
     * @param  rotationCenterY    The hand rotation center y coordinate.
     *
     * @note If no minute hand is setup it will just be omitted.
     */
    virtual void setupMinuteHand(const BitmapId minuteHandBitmapId, int16_t rotationCenterX, int16_t rotationCenterY);

    /**
     * Sets up the second hand. The specified rotation center is the point of the hand that
     * is to be placed on top of the clock rotation center. That is the point that the hand
     * rotates around. The rotation point is relative to the supplied bitmap but can be
     * placed outside of it.
     *
     * @param  secondHandBitmapId Identifier for the second hand bitmap.
     * @param  rotationCenterX    The hand rotation center x coordinate.
     * @param  rotationCenterY    The hand rotation center y coordinate.
     *
     * @note If no second hand is setup it will just be omitted.
     */
    virtual void setupSecondHand(const BitmapId secondHandBitmapId, int16_t rotationCenterX, int16_t rotationCenterY);

    /**
     * Sets whether hour hand minute correction should be active. If set to true the hour
     * hand will be positioned between the current hour and the next depending on the minute
     * hands position.
     *
     * @param  active true to use hour hand correction.
     *
     * @see getHourHandMinuteCorrection
     */
    virtual void setHourHandMinuteCorrection(bool active);

    /**
     * Gets hour hand minute correction.
     *
     * @return true if hour hand minute correction is active.
     *
     * @see setHourHandMinuteCorrection
     */
    virtual bool getHourHandMinuteCorrection() const;

    /**
     * Sets whether minute hand second correction should be active. If set to true the
     * minute hand will be positioned between the current minute and the next depending on
     * the second hands position.
     *
     * @param  active true to use.
     *
     * @see setMinuteHandSecondCorrection
     */
    virtual void setMinuteHandSecondCorrection(bool active);

    /**
     * Gets minute hand second correction.
     *
     * @return true if minute hand second correction is active.
     *
     * @see setHourHandMinuteCorrection
     */
    virtual bool getMinuteHandSecondCorrection() const;

    /**
     * Setup the clock to use animation for hand movements.
     *
     * @param  duration                     (Optional) The animation duration, default is 10.
     * @param  animationProgressionEquation (Optional) The animation progression equation,
     *                                      default is EasingEquations::backEaseInOut.
     */
    virtual void setAnimation(uint16_t duration = 10, EasingEquation animationProgressionEquation = EasingEquations::backEaseInOut);

    /**
     * Gets the animation duration.
     *
     * @return The animation duration.
     *
     * @see setAnimation
     */
    virtual uint16_t getAnimationDuration()
    {
        return animationDuration;
    }

    /**
     * Sets the time with input format as 24H. No animations are performed regardless of the
     * animation settings. This is often useful when setting up the AnalogClock where you do
     * not want an initial animation.
     *
     * @param  hour   The hours, value should be between 0 and 23.
     * @param  minute The minutes, value should be between 0 and 59.
     * @param  second The seconds, value should be between 0 and 59.
     *
     * @see setTime24Hour
     *
     * @note that this does not affect any selected presentation formats.
     */
    virtual void initializeTime24Hour(uint8_t hour, uint8_t minute, uint8_t second);

    /**
     * Sets the time with input format as 12H. No animations are performed regardless of the
     * animation settings. This is often useful when setting up the AnalogClock where you do
     * not want an initial animation.
     *
     * @param  hour   The hours, value should be between 1 and 12.
     * @param  minute The minutes, value should be between 0 and 59.
     * @param  second The seconds, value should be between 0 and 59.
     * @param  am     AM/PM setting. True = AM, false = PM.
     *
     * @see setTime12Hour
     *
     * @note that this does not affect any selected presentation formats.
     */
    virtual void initializeTime12Hour(uint8_t hour, uint8_t minute, uint8_t second, bool am);

protected:
    Image background; ///< The background image of the AnalogClock

    AnimationTextureMapper hourHand;   ///< TextureMapper used for drawing the hourHand
    AnimationTextureMapper minuteHand; ///< TextureMapper used for drawing the minuteHand
    AnimationTextureMapper secondHand; ///< TextureMapper used for drawing the secondHand

    EasingEquation animationEquation; ///< The easing equation used by hand animations
    uint16_t animationDuration;       ///< The duration of hand animations. If 0 animations are disabled

    int16_t clockRotationCenterX; ///< The x coordinate of the rotation point of the hands
    int16_t clockRotationCenterY; ///< The y coordinate of the rotation point of the hands

    uint8_t lastHour;   ///< The last know hour value
    uint8_t lastMinute; ///< The last know minute value
    uint8_t lastSecond; ///< The last know second value

    bool hourHandMinuteCorrectionActive;   ///< Is hour hand minute correction active
    bool minuteHandSecondCorrectionActive; ///< Is minute hand second correction active

    virtual void updateClock();

    /**
     * Sets up a given the hand.
     *
     * @param [in] hand            Reference to the hand being setup.
     * @param      bitmapId        The bitmap identifier for the given hand.
     * @param      rotationCenterX The hand rotation center x coordinate.
     * @param      rotationCenterY The hand rotation center y coordinate.
     */
    virtual void setupHand(TextureMapper& hand, const BitmapId bitmapId, int16_t rotationCenterX, int16_t rotationCenterY);

    /**
     * Convert hand value to angle.
     *
     * @param  steps           Number of steps the primary hand value is divided into, i.e.
     *                         60 for minutes/seconds and 12 for hour.
     * @param  handValue       The actual value for the hand in question (in the range [0;
     *                         steps]).
     * @param  secondHandValue (Optional) If the angle should be corrected for a secondary
     *                         hand its value should be specified here (in the range [0;
     *                         60]). This is the case when setHourHandMinuteCorrection(true)
     *                         or setMinuteHandSecondCorrection(true) is selected.
     *
     * @return The converted value to angle.
     */
    virtual float convertHandValueToAngle(uint8_t steps, uint8_t handValue, uint8_t secondHandValue = 0) const;

    /**
     * Is animation enabled for the hands?
     *
     * @return true if animation is enabled.
     */
    virtual bool animationEnabled() const;
};

} // namespace touchgfx

#endif // ANALOGCLOCK_HPP
