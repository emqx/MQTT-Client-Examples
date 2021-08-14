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
 * @file touchgfx/EasingEquations.hpp
 *
 * Declares the touchgfx::EasingEquations class
 */
#ifndef EASINGEQUATIONS_HPP
#define EASINGEQUATIONS_HPP

#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/**
 * This function pointer typedef matches the signature for all easing equations. Thereby
 * EasingEquation is a convenient shorthand for a pointer to any easing equation.
 */
typedef int16_t (*EasingEquation)(uint16_t, int16_t, int16_t, uint16_t);

/**
 * Defines the "Penner easing functions", which are a de facto standard for computing
 * aesthetically pleasing motion animations. See http://easings.net/ for visual
 * illustrations of the easing equations.
 */
class EasingEquations
{
public:
    /**
     * Back easing in: Overshooting cubic easing: (s+1)*t^3 - s*t^2. Backtracking slightly,
     * then reversing direction and moving to target.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t backEaseIn(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Back easing out: Overshooting cubic easing: (s+1)*t^3 - s*t^2. Moving towards target,
     * overshooting it slightly, then reversing and coming back to target.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t backEaseOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Back easing in/out: Overshooting cubic easing: (s+1)*t^3 - s*t^2. Backtracking
     * slightly, then reversing direction and moving to target, then overshooting target,
     * reversing, and finally coming back to target.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t backEaseInOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Bounce easing in - exponentially decaying parabolic bounce.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t bounceEaseIn(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Bounce easing out - exponentially decaying parabolic bounce.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t bounceEaseOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Bounce easing in/out - exponentially decaying parabolic bounce.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t bounceEaseInOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Circular easing in: sqrt(1-t^2). Accelerating from zero velocity.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t circEaseIn(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Circular easing out: sqrt(1-t^2). Decelerating to zero velocity.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t circEaseOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Circular easing in/out: sqrt(1-t^2). Acceleration until halfway, then deceleration.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t circEaseInOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Cubic easing in: t^3. Accelerating from zero velocity.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t cubicEaseIn(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Cubic easing out: t^3. Decelerating to zero velocity.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t cubicEaseOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Cubic easing in/out: t^3. Acceleration until halfway, then deceleration.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t cubicEaseInOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Elastic easing in - exponentially decaying sine wave.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t elasticEaseIn(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Elastic easing out - exponentially decaying sine wave.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t elasticEaseOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Elastic easing in/out - exponentially decaying sine wave.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t elasticEaseInOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Exponential easing in: 2^t. Accelerating from zero velocity.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t expoEaseIn(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Exponential easing out: 2^t. Deceleration to zero velocity.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t expoEaseOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Exponential easing in/out: 2^t. Accelerating until halfway, then decelerating.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t expoEaseInOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Simple linear tweening - no easing.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t linearEaseNone(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Simple linear tweening - no easing.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t linearEaseIn(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Simple linear tweening - no easing.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t linearEaseOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Simple linear tweening - no easing.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t linearEaseInOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Quadratic easing in: t^2. Accelerating from zero velocity.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t quadEaseIn(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Quadratic easing out: t^2. Decelerating to zero velocity.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t quadEaseOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Quadratic easing in/out: t^2. Acceleration until halfway, then deceleration.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t quadEaseInOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Quartic easing in: t^4. Accelerating from zero velocity.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t quartEaseIn(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Quartic easing out: t^4. Decelerating to zero velocity.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t quartEaseOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Quartic easing in/out: t^4. Acceleration until halfway, then deceleration.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t quartEaseInOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Quintic/strong easing in: t^5. Accelerating from zero velocity.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t quintEaseIn(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Quintic/strong easing out: t^5. Decelerating to zero velocity.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t quintEaseOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Quintic/strong easing in/out: t^5. Acceleration until halfway, then deceleration.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t quintEaseInOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Sinusoidal easing in: sin(t). Accelerating from zero velocity.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t sineEaseIn(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Sinusoidal easing out: sin(t). Decelerating to zero velocity.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t sineEaseOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

    /**
     * Sinusoidal easing in/out: sin(t). Acceleration until halfway, then deceleration.
     *
     * @param  t Time. The current time or step.
     * @param  b Beginning. The beginning value.
     * @param  c Change. The change between the beginning value and the destination value.
     * @param  d Duration. The total time or total number of steps.
     *
     * @return The current value as a function of the current time or step.
     */
    static int16_t sineEaseInOut(uint16_t t, int16_t b, int16_t c, uint16_t d);

private:
    /**
     * Round respect sign.
     *
     * @param  resultFloating The result floating.
     *
     * @return An int16_t.
     */
    FORCE_INLINE_FUNCTION static int16_t roundRespectSign(float resultFloating)
    {
        return static_cast<int16_t>((resultFloating > 0.0f) ? (resultFloating + 0.5f) : (resultFloating - 0.5f));
    }
};

} // namespace touchgfx

#endif // EASINGEQUATIONS_HPP
