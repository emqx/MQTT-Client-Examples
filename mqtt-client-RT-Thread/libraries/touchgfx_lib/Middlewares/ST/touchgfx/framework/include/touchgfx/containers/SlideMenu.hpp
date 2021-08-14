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
 * @file touchgfx/containers/SlideMenu.hpp
 *
 * Declares the touchgfx::SlideMenu class.
 */
#ifndef SLIDEMENU_HPP
#define SLIDEMENU_HPP

#include <touchgfx/EasingEquations.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/mixins/MoveAnimator.hpp>
#include <touchgfx/widgets/Button.hpp>
#include <touchgfx/widgets/Image.hpp>

namespace touchgfx
{
/**
 * SlideMenu is a menu that can expand and collapse at the touch of a button. The SlideMenu can
 * expand in any of the four directions. Menu items can be added, just like items are
 * added to a normal container.
 *
 * The relative positions of the background and state change button is configurable as
 * is the direction in which the SlideMenu expands and collapses. How much of the
 * SlideMenu that is visible when collapsed can also be set with the. It is, of course,
 * important that the state change button is accessible when collapsed. The SlideMenu
 * will collapse after a given timeout is reached. The timer can be reset, for example
 * when the user interacts with elements in the list.
 *
 * Menu elements are added normally using the add() method and are positioned relative
 * to the SlideMenu.
 */
class SlideMenu : public Container
{
public:
    /** Values that represent the SlideMenu states. */
    enum State
    {
        COLLAPSED, ///< Menu is currently collapsed
        EXPANDED   ///< Menu is currently expanded
    };

    /** Values that represent the expand directions. */
    enum ExpandDirection
    {
        SOUTH, ///< Menu expands downwards (<b>Towards</b> the south)
        NORTH, ///< Menu expands upwards (<b>Towards</b> the north)
        EAST,  ///< Menu expands to the right (<b>Towards</b> the east)
        WEST   ///< Menu expands to the left (<b>Towards</b> the west)
    };

    SlideMenu();

    virtual ~SlideMenu();

    /**
     * Setup the SlideMenu by positioning the stateChangeButton next to background image
     * relative to the expand direction, and center it in the other dimension. The width and
     * height of the SlideMenu will be automatically set to span both elements. Default
     * values are: expandedStateTimeout = 200, visiblePixelsWhenCollapsed = 0,
     * hiddenPixelsWhenExpanded = 0, animationDuration = 10, animationEquation =
     * cubicEaseInOut.
     *
     * @param  newExpandDirection          The new expand direction.
     * @param  backgroundBMP               The background bitmap.
     * @param  stateChangeButtonBMP        The state change button bitmap.
     * @param  stateChangeButtonPressedBMP The state change button pressed bitmap.
     */
    virtual void setup(SlideMenu::ExpandDirection newExpandDirection, const Bitmap& backgroundBMP, const Bitmap& stateChangeButtonBMP, const Bitmap& stateChangeButtonPressedBMP);

    /**
     * Setup method for the SlideMenu. Positioning of the background is done by stating
     * the X and Y coordinates for the element (relative to the SlideMenu).
     * The width and height of the SlideMenu will be automatically set to the size of the background.
     * Default values are: expandedStateTimeout = 200, visiblePixelsWhenCollapsed = 0,
     * hiddenPixelsWhenExpanded = 0, animationDuration * = 10, animationEquation = cubicEaseInOut.
     *
     * @param  newExpandDirection          The new expand direction.
     * @param  backgroundBMP               The background bitmap.
     * @param  backgroundX                 The background x coordinate.
     * @param  backgroundY                 The background y coordinate.
     */
    virtual void setup(SlideMenu::ExpandDirection newExpandDirection, const Bitmap& backgroundBMP, int16_t backgroundX, int16_t backgroundY);

    /**
     * Setup method for the SlideMenu. Positioning of the background image and the
     * stateChangeButton is done by stating the X and Y coordinates for the elements
     * (relative to the SlideMenu). The width and height of the SlideMenu will be
     * automatically set to span both elements. Default values are: expandedStateTimeout =
     * 200, visiblePixelsWhenCollapsed = 0, hiddenPixelsWhenExpanded = 0, animationDuration
     * = 10, animationEquation = cubicEaseInOut.
     *
     * @param  newExpandDirection          The new expand direction.
     * @param  backgroundBMP               The background bitmap.
     * @param  stateChangeButtonBMP        The state change button bitmap.
     * @param  stateChangeButtonPressedBMP The state change button pressed bitmap.
     * @param  backgroundX                 The background x coordinate.
     * @param  backgroundY                 The background y coordinate.
     * @param  stateChangeButtonX          The state change button x coordinate.
     * @param  stateChangeButtonY          The state change button y coordinate.
     */
    virtual void setup(SlideMenu::ExpandDirection newExpandDirection, const Bitmap& backgroundBMP, const Bitmap& stateChangeButtonBMP, const Bitmap& stateChangeButtonPressedBMP, int16_t backgroundX, int16_t backgroundY, int16_t stateChangeButtonX, int16_t stateChangeButtonY);

    /**
     * Sets the expand direction.
     *
     * @param  newExpandDirection The new expand direction.
     */
    virtual void setExpandDirection(SlideMenu::ExpandDirection newExpandDirection);

    /**
     * Gets the expand direction.
     *
     * @return The expand direction.
     */
    virtual SlideMenu::ExpandDirection getExpandDirection() const;

    /**
     * Sets the amount of visible pixels when collapsed.
     *
     * @param  visiblePixels The visible pixels.
     */
    virtual void setVisiblePixelsWhenCollapsed(int16_t visiblePixels);

    /**
     * Gets the visible pixels when collapsed.
     *
     * @return The visible pixels when collapsed.
     */
    virtual int16_t getVisiblePixelsWhenCollapsed() const;

    /**
     * Sets the amount of hidden pixels when expanded.
     *
     * @param  hiddenPixels The hidden pixels.
     */
    virtual void setHiddenPixelsWhenExpanded(int16_t hiddenPixels);

    /**
     * Gets the hidden pixels when expanded.
     *
     * @return The hidden pixels when expanded.
     */
    virtual int16_t getHiddenPixelsWhenExpanded() const;

    /**
     * Sets the expanded state timeout in ticks. The SlideMenu will animate to the COLLAPSED
     * state when this number of ticks has been executed while the SlideMenu is in the
     * EXPANDED state. The timer can be reset with the resetExpandedStateTimer method.
     *
     * @param  timeout The timeout in ticks.
     */
    virtual void setExpandedStateTimeout(uint16_t timeout);

    /**
     * Gets expanded state timeout.
     *
     * @return The expanded state timeout.
     */
    virtual uint16_t getExpandedStateTimeout() const;

    /**
     * Sets the animation duration.
     *
     * @param  duration The animation duration.
     */
    virtual void setAnimationDuration(uint16_t duration);

    /**
     * Gets the animation duration.
     *
     * @return The animation duration.
     */
    virtual uint16_t getAnimationDuration() const;

    /**
     * Sets the animation easing equation.
     *
     * @param  animationEasingEquation The animation easing equation.
     */
    virtual void setAnimationEasingEquation(EasingEquation animationEasingEquation);

    /**
     * Gets the animation easing equation.
     *
     * @return The animation easing equation.
     */
    virtual EasingEquation getAnimationEasingEquation() const;

    /**
     * Sets the state of the SlideMenu. No animation is performed.
     *
     * @param  newState The new state of the SlideMenu.
     *
     * @see animateToState, getState
     */
    virtual void setState(SlideMenu::State newState);

    /**
     * Animate to the given expanded or collapsed state.
     *
     * @param  newState The new state of the SlideMenu.
     *
     * @see setState, getState
     */
    virtual void animateToState(SlideMenu::State newState);

    /**
     * Gets the current expanded or collapsed state.
     *
     * @return The current state.
     *
     * @see setState, animateToState
     */
    virtual SlideMenu::State getState();

    /**
     * Resets the expanded state timer. The SlideMenu will automatically animate to the
     * COLLAPSED state after a number of ticks, as set with setExpandedStateTimeout(). This
     * method resets this timer.
     *
     * @see getExpandedStateTimer
     */
    virtual void resetExpandedStateTimer();

    /**
     * Gets the expanded state timer.
     *
     * @return The expanded state timer.
     *
     * @see resetExpandedStateTimer
     */
    virtual uint16_t getExpandedStateTimer() const;

    /**
     * Gets the background Image x coordinate.
     *
     * @return The background Image x coordinate.
     */
    virtual int16_t getBackgroundX() const;

    /**
     * Gets the background Image y coordinate.
     *
     * @return The background Image y coordinate.
     */
    virtual int16_t getBackgroundY() const;

    /**
     * Gets the state change button x coordinate.
     *
     * @return The state change button x coordinate.
     */
    virtual int16_t getStateChangeButtonX() const;

    /**
     * Gets the state change button y coordinate.
     *
     * @return The state change button y coordinate.
     */
    virtual int16_t getStateChangeButtonY() const;

    /**
     * Set the state changed callback. This callback is called when the state change button
     * is clicked.
     *
     * @param  callback The callback.
     */
    virtual void setStateChangedCallback(GenericCallback<const SlideMenu&>& callback);

    /**
     * Set the state change animation ended callback. This callback is called when a state
     * change animation has ended.
     *
     * @param  callback The callback.
     */
    virtual void setStateChangedAnimationEndedCallback(GenericCallback<const SlideMenu&>& callback);

    /**
     * Adds a drawable to the container. Make sure the x and y coordinates of the Drawable
     * is correct relative to the SlideMenu.
     *
     * @param [in] d The drawable to add.
     */
    virtual void add(Drawable& d);

    /**
     * Removes the drawable from the container.
     *
     * @param [in] d The drawable to remove.
     */
    virtual void remove(Drawable& d);

protected:
    MoveAnimator<Container> menuContainer; ///< The container holding the actual menu items. This is the container that performs the state change animation
    Button stateChangeButton;              ///< The state change button that toggles the SlideMenu state
    Image background;                      ///< The background of the SlideMenu

    Callback<SlideMenu, const AbstractButton&> onStateChangeButtonClicked;      ///< The local state changed button clicked callback
    Callback<SlideMenu, const MoveAnimator<Container>&> animationEndedCallback; ///< The local state changed animation ended callback

    GenericCallback<const SlideMenu&>* stateChangedCallback;               ///< The public state changed button clicked callback
    GenericCallback<const SlideMenu&>* stateChangedAnimationEndedCallback; ///< The public state changed animation ended callback

    SlideMenu::State currentState;              ///< The current state of the SlideMenu
    SlideMenu::ExpandDirection expandDirection; ///< The expand direction of the SlideMenu

    EasingEquation animationEquation; ///< The easing equation used for the state change animation

    int16_t visiblePixelsWhenCollapsed; ///< The number of visible pixels when collapsed
    int16_t hiddenPixelsWhenExpanded;   ///< The number of hidden pixels when expanded

    uint16_t expandedStateTimeout; ///< The expanded state timeout.
    uint16_t expandedStateTimer;   ///< The timer that counts towards the expandedStateTimeout. If reached the SlideMenu will animate to COLLAPSED.

    uint16_t animationDuration; ///< The animation duration of the state change animation

    /**
     * Handler for the state change button clicked event.
     *
     * @param  button The state change button.
     */
    void stateChangeButtonClickedHandler(const AbstractButton& button);

    /**
     * Handler for the state change animation ended event.
     *
     * @param  container The menuContainer.
     */
    void animationEndedHandler(const MoveAnimator<Container>& container);

    virtual void handleTickEvent();

    /**
     * Gets the x coordinate for the collapsed state.
     *
     * @return The collapsed x coordinate.
     */
    virtual int16_t getCollapsedXCoordinate();

    /**
     * Gets the y coordinate for the collapsed state.
     *
     * @return The collapsed y coordinate.
     */
    virtual int16_t getCollapsedYCoordinate();

    /**
     * Gets the x coordinate for the expanded state.
     *
     * @return The expanded x coordinate.
     */
    virtual int16_t getExpandedXCoordinate();

    /**
     * Gets the y coordinate for the expanded state.
     *
     * @return The expanded y coordinate.
     */
    virtual int16_t getExpandedYCoordinate();
};

} // namespace touchgfx

#endif // SLIDEMENU_HPP
