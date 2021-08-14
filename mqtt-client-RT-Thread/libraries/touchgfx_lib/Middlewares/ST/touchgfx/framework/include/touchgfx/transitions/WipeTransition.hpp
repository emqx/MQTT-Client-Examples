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
 * @file touchgfx/transitions/WipeTransition.hpp
 *
 * Declares the touchgfx::WipeTransition class.
 */
#ifndef WIPETRANSITION_HPP
#define WIPETRANSITION_HPP

#include <touchgfx/EasingEquations.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/Types.hpp>
#include <touchgfx/transitions/Transition.hpp>
#include <touchgfx/widgets/Widget.hpp>

namespace touchgfx
{
class Container;
class Widget;

/**
 * A Transition that expands the new screen over the previous from
 * the given direction.  This transition only draws the pixels in the
 * framebuffer once, and never moves any pixels. It is therefore very
 * usefull on MCUs with limited performance.
 */
template <Direction templateDirection>
class WipeTransition : public Transition
{
public:
    /**
     * A Widget that reports solid and but does not draw anything.
     */
    class FullSolidRect : public Widget
    {
    public:

        virtual Rect getSolidRect() const
        {
            return Rect(0U, 0U, rect.width, rect.height);
        }

        virtual void draw(const Rect& area) const
        {
        }
    };

    /**
     * Initializes a new instance of the WipeTransition class.
     *
     * @param  transitionSteps (Optional) Number of steps in the transition animation.
     */
    WipeTransition(const uint8_t transitionSteps = 20)
        : Transition(),
          animationSteps(transitionSteps),
          animationCounter(0),
          calculatedValue(0),
          solid()
    {
        switch (templateDirection)
        {
        case EAST:
        case WEST:
            targetValue = HAL::DISPLAY_WIDTH;
            break;
        case NORTH:
        case SOUTH:
            targetValue = HAL::DISPLAY_HEIGHT;
            break;
        default:
            done = true;
            // Nothing to do here
            break;
        }

        //Ensure that the solid area covers the entire screen
        solid.setPosition(0, 0, HAL::DISPLAY_WIDTH, HAL::DISPLAY_HEIGHT);
    }

    /**
     * Handles the tick event when transitioning. It uncovers and
     * invalidates increasing parts of the new screen elements.
     */
    virtual void handleTickEvent()
    {
        Transition::handleTickEvent();
        animationCounter++;

        // Calculate new position or stop animation
        if (animationCounter <= (animationSteps))
        {
            // Calculate value in [0;targetValue]
            calculatedValue = EasingEquations::cubicEaseOut(animationCounter, 0, targetValue, animationSteps);

            // Note: Result of "calculatedValue & 1" is compiler dependent for negative values of calculatedValue
            if (calculatedValue % 2)
            {
                // Optimization: calculatedValue is odd, add 1/-1 to move drawables modulo 32 bits in framebuffer
                calculatedValue += (calculatedValue > 0 ? 1 : -1);
            }
        }
        else
        {
            // Final step: stop the animation
            done = true;
            animationCounter = 0;
            return;
        }

        //calculatedValue is the width/height of the visible area

        switch (templateDirection)
        {
        case EAST:
            {
                //cover must have width of remaining part
                const uint16_t prevSolidWidth = solid.getWidth();
                solid.setWidth(HAL::DISPLAY_WIDTH - calculatedValue);

                //invalidate the uncovered part
                const uint16_t delta = prevSolidWidth - solid.getWidth();
                Rect r(solid.getWidth(), 0, delta, HAL::DISPLAY_HEIGHT);
                screenContainer->invalidateRect(r);
                break;
            }
        case WEST:
            {
                //cover must have width of remaining part and start after uncovered
                const uint16_t prevSolidPos = solid.getX();
                solid.setWidth(HAL::DISPLAY_WIDTH - calculatedValue);
                solid.setX(calculatedValue);

                //invalidate the uncovered part
                const uint16_t delta = calculatedValue - prevSolidPos;
                Rect r(prevSolidPos, 0, delta, HAL::DISPLAY_HEIGHT);
                screenContainer->invalidateRect(r);
                break;
            }
        case NORTH:
            {
                //cover must have height of remaining part and start after uncovered
                const uint16_t prevSolidPos = solid.getY();
                solid.setHeight(HAL::DISPLAY_HEIGHT - calculatedValue);
                solid.setY(calculatedValue);

                //invalidate the uncovered part
                const uint16_t delta = calculatedValue - prevSolidPos;
                Rect r(0, prevSolidPos, HAL::DISPLAY_WIDTH, delta);
                screenContainer->invalidateRect(r);
                break;
            }
        case SOUTH:
            {
                //cover must have height of remaining part
                const uint16_t prevSolidHeight = solid.getHeight();
                solid.setHeight(HAL::DISPLAY_HEIGHT - calculatedValue);

                //invalidate the uncovered part
                const uint16_t delta = prevSolidHeight - solid.getHeight();
                Rect r(0, solid.getHeight(), HAL::DISPLAY_WIDTH, delta);
                screenContainer->invalidateRect(r);
                break;
            }
        default:
            // Special case, do not move. Class NoTransition can be used instead.
            done = true;
            break;
        }

        // The WipeTransition only draws to parts of the non-TFT
        // framebuffer. To avoid glitches in Double buffering mode
        // both framebuffers must be made identical.
        //
        // In the first tick WipeTransition cover "calculatedValue"
        // pixels vertically or horizontally depending on the speed of
        // the transition, so there's no need to transfer that. The
        // solid Widget covers the rest, so we copy those pixels.
        //
        if (animationCounter == 1 && HAL::USE_DOUBLE_BUFFERING)
        {
            Rect rect = solid.getRect(); //part to copy between buffers

            // Get the currently displayed framebuffer
            uint16_t* tftFb = HAL::getInstance()->getTFTFrameBuffer();

            Rect source;
            source.x = 0;
            source.y = 0;
            source.width = HAL::DISPLAY_WIDTH;
            source.height = HAL::DISPLAY_HEIGHT;

            //Copy rect from tft to client framebuffer
            HAL::getInstance()->lcd().blitCopy((const uint16_t*)tftFb, source, rect, 255, false);
        }
    }

    virtual void tearDown()
    {
        screenContainer->remove(solid);
    }

    virtual void init()
    {
        Transition::init();
        //add the solid (and not-drawing-anything) widget on top to cover the other widgets
        screenContainer->add(solid);
    }

    /**
     * Wipe transition does not require an invalidation. Invalidation
     * is handled by the class. Do no invalidation initially.
     */
    virtual void invalidate()
    {
        //nop
    }

private:
    const uint8_t animationSteps; ///< Number of steps the transition should move per complete animation.
    uint8_t animationCounter;     ///< Current step in the transition animation.
    int16_t targetValue;          ///< The target value for the transition animation.
    int16_t calculatedValue;      ///< The calculated X or Y value to move the snapshot and the children.
    FullSolidRect solid; ///< A solid rect that covers the entire screen to avoid copying elements outside
};

} // namespace touchgfx

#endif // WIPETRANSITION_HPP
