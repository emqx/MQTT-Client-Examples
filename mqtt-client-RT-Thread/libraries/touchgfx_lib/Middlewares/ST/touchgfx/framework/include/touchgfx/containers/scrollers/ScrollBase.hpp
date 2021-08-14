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
 * @file touchgfx/containers/scrollers/ScrollBase.hpp
 *
 * Declares the touchgfx::ScrollBase class.
 */
#ifndef SCROLLBASE_HPP
#define SCROLLBASE_HPP

#include <touchgfx/Callback.hpp>
#include <touchgfx/EasingEquations.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/containers/scrollers/DrawableList.hpp>

namespace touchgfx
{
/**
 * The ScrollBase class is an abstract class used for Widgets that needs to show (a lot of)
 * elements in a DrawableList that can be scrolled. Due to memory limitations, this is
 * implemented by re-using the Drawables in the DrawableList - once an element is moved
 * off screen, it is filled with new content and moved to the other end and the of the
 * scrolling list.
 *
 * Lists can be horizontal or vertical and the can be circular (infinite scrolling).
 *
 * @see ScrollList, ScrollWheel, ScrollWheelWithSelectionStyle
 * @see ScrollWheelBase, DrawableList
 */
class ScrollBase : public Container
{
public:
    ScrollBase();

    /**
     * @copydoc Container::setWidth
     *
     * @note Also sets the width of the children.
     */
    virtual void setWidth(int16_t width);

    /**
     * @copydoc Container::setHeight
     *
     * @note Also sets the height of the children.
     */
    virtual void setHeight(int16_t height);

    /**
     * Sets a horizontal or vertical layout. If parameter horizontal is set true, all
     * drawables are arranged side by side. If horizontal is set false, the drawables are
     * arranged above and below each other (vertically).
     *
     * @param  horizontal True to align drawables horizontal, false to align drawables
     *                    vertically.
     *
     * @see DrawableList::setHorizontal, getHorizontal
     *
     * @note Default value is false, i.e. vertical layout.
     */
    virtual void setHorizontal(bool horizontal);

    /**
     * Gets the orientation of the drawables, previously set using setHorizontal().
     *
     * @return True if it horizontal, false if it is vertical.
     *
     * @see DrawableList::getHorizontal, setHorizontal
     */
    virtual bool getHorizontal() const;

    /**
     * Sets whether the list is circular (infinite) or not. A circular list is a list where
     * the first drawable re-appears after the last item in the list - and the last item in
     * the list appears before the first item in the list.
     *
     * @param  circular True if the list should be circular, false if the list should not be
     *                  circular.
     *
     * @see DrawableList::setCircular, getCircular
     */
    virtual void setCircular(bool circular);

    /**
     * Gets the circular setting, previously set using setCircular().
     *
     * @return True if the list is circular (infinite), false if the list is not circular
     *         (finite).
     *
     * @see DrawableList::getCircular, setCircular
     */
    virtual bool getCircular() const;

    /**
     * Sets drawables size. The drawable is is the size of each drawable in the list in the
     * set direction of the list (this is enforced by the DrawableList class). The specified
     * margin is added above and below each item for spacing. The entire size of an item is
     * thus size + 2 * spacing.
     *
     * For a horizontal list each element will be \a drawableSize high and have the same
     * width as set using setWidth(). For a vertical list each element will be \a
     * drawableSize wide and have the same height as set using setHeight().
     *
     * @param  drawableSize   The size of the drawable.
     * @param  drawableMargin The margin around drawables (margin before and margin after).
     *
     * @see setWidth, setHeight, setHorizontal
     */
    void setDrawableSize(int16_t drawableSize, int16_t drawableMargin);

    /**
     * Gets drawable size as set through the first parameter in most recent call to
     * setDrawableSize().
     *
     * @return The drawable size.
     *
     * @see setDrawableSize
     */
    virtual int16_t getDrawableSize() const;

    /**
     * Gets drawable margin as set through the second parameter in most recent call to
     * setDrawableSize().
     *
     * @return The drawable margin.
     *
     * @see setDrawableSize
     */
    virtual int16_t getDrawableMargin() const;

    /**
     * Sets number of items in the DrawableList. This forces all drawables to be updated to
     * ensure that the content is correct. For example a date selector might switch number
     * of days between 28, 29, 30, and 31 depending on the month. A circular list might show
     * 27-28-29-30-31 and might need to update this to show 27-28-1-2-3.
     *
     * @param  numberOfItems Number of items.
     *
     * @note The DrawableList is refreshed to reflect the change.
     */
    virtual void setNumberOfItems(int16_t numberOfItems);

    /**
     * Gets number of items in the DrawableList, as previously set using setNumberOfItems().
     *
     * @return The number of items.
     *
     * @see setNumberOfItems, DrawableList::getNumberOfItems
     */
    virtual int16_t getNumberOfItems() const;

    /**
     * Sets easing equation when changing the selected item, for example via swipe or
     * AnimateTo.
     *
     * @param  equation The equation.
     *
     * @see setAnimationSteps, getAnimationSteps
     */
    void setEasingEquation(EasingEquation equation);

    /**
     * Sets animation steps (in ticks) when moving to a new selected item. The default value is 30.
     *
     * @param  steps The animation steps.
     *
     * @see setEasingEquation, getAnimationSteps
     */
    void setAnimationSteps(int16_t steps);

    /**
     * Gets animation steps as set in setAnimationSteps.
     *
     * @return The animation steps.
     *
     * @see setAnimationSteps, setEasingEquation
     */
    uint16_t getAnimationSteps() const;

    /**
     * Sets swipe acceleration (times 10). Default value, if not set, is 10, i.e. 1.0.
     *
     * @param  acceleration The acceleration times 10, so "9" means "0.9" and "75" means "7.5".
     *
     * @see getSwipeAcceleration
     *
     * @note The reason for multiplying the acceleration by 10 is to avoid introducing floating
     *       point arithmetic.
     */
    void setSwipeAcceleration(uint16_t acceleration);

    /**
     * Gets swipe acceleration (times 10).
     *
     * @return The swipe acceleration.
     *
     * @see setSwipeAcceleration
     *
     * @note The reason for multiplying the acceleration by 10 is to avoid introducing floating
     *       point arithmetic.
     */
    uint16_t getSwipeAcceleration() const;

    /**
     * Sets maximum swipe items. Often useful when there are e.g. five visible items on the
     * screen and a swipe action should at most swipe the next/previous five items into view
     * to achieve sort of a paging effect.
     *
     * @param  maxItems The maximum items, 0 means "no limit" (which is also the default).
     *
     * @see getMaxSwipeItems
     */
    void setMaxSwipeItems(uint16_t maxItems);

    /**
     * Gets maximum swipe items as set by setMaxSwipeItems.
     *
     * @return The maximum swipe items, 0 means "no limit".
     *
     * @see setMaxSwipeItems
     */
    uint16_t getMaxSwipeItems() const;

    /**
     * Sets drag acceleration times 10, so "10" means "1", "15" means "1.5". 10 makes the
     * containers follow the finger, higher values makes the containers move faster. This
     * can often be useful if the list is very long.
     *
     * @param  acceleration The drag acceleration.
     *
     * @see getDragAcceleration
     *
     * @note The reason for multiplying the acceleration by 10 is to avoid introducing floating
     *       point arithmetic.
     */
    void setDragAcceleration(uint16_t acceleration);

    /**
     * Gets drag acceleration (times 10).
     *
     * @return The drag acceleration.
     *
     * @see setDragAcceleration
     *
     * @note The reason for multiplying the acceleration by 10 is to avoid introducing floating
     *       point arithmetic.
     */
    uint16_t getDragAcceleration() const;

    /**
     * Enables horizontal scrolling to be passed to the children in the list (in case a child
     * widget is able to handle drag events). By default, scrolling in either direction is
     * disabled. This function can be used to explicitly (dis)allow scrolling in the
     * horizontal direction.
     *
     * @param  enable If true, horizontal scrolling is enabled. If false (default), scrolling is
     *                disabled.
     */
    void allowHorizontalDrag(bool enable);

    /**
     * Enables the vertical scroll.
     *
     * Enables the vertical scroll to be passed to the children in the list (in case a child
     * widget is able to handle drag events). By default, scrolling in either direction is
     * disabled. This function can be used to explicitly (dis)allow scrolling in the
     * vertical direction.
     *
     * @param  enable If true, vertical scrolling is enabled. If false (default), scrolling is
     *                disabled.
     */
    void allowVerticalDrag(bool enable);

    /**
     * Go to a specific item, possibly with animation. The given item index is scrolled into view. If
     * animationSteps is omitted, the default number of animation steps is used. If
     * animationSteps is 0 no animation will be used, otherwise the number of animation
     * steps specified is used.
     *
     * @param  itemIndex      Zero-based index of the item.
     * @param  animationSteps (Optional) The steps to use for the animation. 0 means no animation.
     *                        If omitted, default animation steps are used.
     *
     * @see setAnimationSteps
     */
    virtual void animateToItem(int16_t itemIndex, int16_t animationSteps = -1);

    /**
     * Sets Callback which will be called when the selected item is clicked.
     *
     * @param [in] callback The callback.
     */
    void setItemSelectedCallback(GenericCallback<int16_t>& callback);

    /**
     * Callback, called when the set animation ended.
     *
     * @param [in] callback The ended callback.
     */
    void setAnimationEndedCallback(GenericCallback<>& callback);

    /**
     * Set Callback which will be called when a item is pressed.
     *
     * @param [in] callback The callback.
     */
    void setItemPressedCallback(GenericCallback<int16_t>& callback);

    /**
     * Query if an animation is ongoing. This can be good to know if getSelectedItem()
     * is called, as the result might not be as expected if isAnimating() returns true,
     * since the display is not showing the selected item in the right place yet.
     *
     * @return true if animating, false if not.
     */
    bool isAnimating() const;

    /**
     * Stops an animation if one is ongoing. Immediately moves to the item which is being
     * animated to.
     */
    void stopAnimation();

    virtual void handleDragEvent(const DragEvent& evt);

    virtual void handleGestureEvent(const GestureEvent& evt);

    virtual void handleTickEvent();

    /**
     * Inform the scroll list that the contents of an item has changed and force all
     * drawables with the given item index to be updated via the callback provided. This is
     * important as a circular list with very few items might display the same item more
     * than once and all these items should be updated.
     *
     * @param  itemIndex Zero-based index of the changed item.
     */
    virtual void itemChanged(int itemIndex);

    /** Removed all drawables and initializes the content of these items. */
    virtual void initialize()
    {
        list.refreshDrawables();
    }

protected:
    /**
     * Sets display offset of first item.
     *
     * @param  offset The offset.
     */
    virtual void setOffset(int32_t offset);

    /**
     * Gets display offset of first item.
     *
     * @return The offset.
     */
    virtual int32_t getOffset() const;

    /**
     * Get the position for an item. The position should ensure that the item is in view as
     * defined by the semantics of the actual scroll class. If the item is already in view,
     * the current offset is returned and not the offset of the given item.
     *
     * @param  itemIndex Zero-based index of the item.
     *
     * @return The position for item.
     */
    virtual int32_t getPositionForItem(int16_t itemIndex) = 0;

    /**
     * Gets normalized offset from a given offset from 0 down to -numItems*itemSize.
     *
     * @param  offset The offset.
     *
     * @return The normalized offset.
     */
    int getNormalizedOffset(int offset) const;

    /**
     * Keep offset inside limits. Return the new offset that is inside the limits of the
     * scroll list, with the overShoot value added at both ends of the list.
     *
     * @param  newOffset The new offset.
     * @param  overShoot The over shoot.
     *
     * @return The new offset inside the limits.
     */
    virtual int32_t keepOffsetInsideLimits(int32_t newOffset, int16_t overShoot) const = 0;

    /**
     * Gets nearest offset aligned to a multiple of itemSize.
     *
     * @param  offset The offset.
     *
     * @return The nearest aligned offset.
     */
    virtual int32_t getNearestAlignedOffset(int32_t offset) const;

    /**
     * Animate to a new position/offset using the given number of steps.
     *
     * @param  position The new position.
     * @param  steps    (Optional) The steps.
     */
    virtual void animateToPosition(int32_t position, int16_t steps = -1);

    /** Values that represent animation states. */
    enum AnimationState
    {
        NO_ANIMATION,      ///< No animation
        ANIMATING_GESTURE, ///< Animating a gesture
        ANIMATING_DRAG     ///< Animating a drag
    };

    DrawableList list;                 ///< The list
    int16_t numberOfDrawables;         ///< Number of drawables
    int16_t distanceBeforeAlignedItem; ///< The distance before aligned item

    int16_t itemSize;               ///< Size of the item (including margin)
    uint16_t swipeAcceleration;     ///< The swipe acceleration x10
    uint16_t dragAcceleration;      ///< The drag acceleration x10
    uint16_t maxSwipeItems;         ///< The maximum swipe items
    EasingEquation easingEquation;  ///< The easing equation used for animation
    uint16_t defaultAnimationSteps; ///< The animation steps

    GenericCallback<int16_t>* itemSelectedCallback; ///< The item selected callback
    GenericCallback<>* itemLockedInCallback;        ///< The item locked in callback
    GenericCallback<>* animationEndedCallback;      ///< The animation ended callback
    GenericCallback<int16_t>* itemPressedCallback;  ///< The item pressed callback

    AnimationState currentAnimationState; ///< The current animation state
    int gestureStep;                      ///< The current gesture step
    int gestureStepsTotal;                ///< The total gesture steps
    int gestureStart;                     ///< The gesture start
    int gestureEnd;                       ///< The gesture end

    int16_t xClick;             ///< The x coordinate of a click
    int16_t yClick;             ///< The y coordinate of a click
    int32_t initialSwipeOffset; ///< The initial swipe offset

    bool draggableX; ///< Is the container draggable in the horizontal direction.
    bool draggableY; ///< Is the container draggable in the vertical direction.
};

} // namespace touchgfx

#endif // SCROLLBASE_HPP
