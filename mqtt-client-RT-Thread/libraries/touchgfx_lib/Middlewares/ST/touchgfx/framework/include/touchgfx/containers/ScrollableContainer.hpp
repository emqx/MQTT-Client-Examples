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
 * @file touchgfx/containers/ScrollableContainer.hpp
 *
 * Declares the touchgfx::ScrollableContainer class.
 */
#ifndef SCROLLABLECONTAINER_HPP
#define SCROLLABLECONTAINER_HPP

#include <touchgfx/Application.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Box.hpp>

namespace touchgfx
{
/**
 * A ScrollableContainer is a container that allows its contents to be scrolled. It will
 * intercept drag operations and move child nodes accordingly.
 *
 * A standard Container will simply clip children that are either larger than the
 * container itself, or children that extend beyond the borders of the container or
 * children that are placed outside the borders of the container. A ScrollableContainer
 * behaves much like a Container, except it enables the user to scroll the children and
 * thereby act like a viewport. When the contents of the ScrollableContainer is
 * scrollable, scrollbars can be seen near the edge of the ScrollableContainer.
 *
 * @see Container
 *
 * @note The ScrollableContainer will consume all DragEvents in the area covered by the
 *       container.
 */
class ScrollableContainer : public Container
{
public:
    ScrollableContainer();

    /**
     * Enables horizontal scrolling. By default, scrolling in either direction is enabled,
     * provided that the content is larger than the size of the ScrollableContainer. This
     * function can be used to explicitly (dis)allow horizontal scrolling, even if the
     * content is larger than the container.
     *
     * @param  enable If true (default), horizontal scrolling is enabled. If false, horizontal
     *                scrolling is disabled.
     *
     * @see enableVerticalScroll
     */
    void enableHorizontalScroll(bool enable)
    {
        scrollableX = enable;
    }

    /**
     * Enables vertical scrolling. By default, scrolling in either direction is enabled,
     * provided that the content is larger than the size of the ScrollableContainer. This
     * function can be used to explicitly (dis)allow vertical scrolling, even if the content
     * is larger than the container.
     *
     * @param  enable If true (default), vertical scrolling is enabled. If false, vertical
     *                scrolling is disabled.
     *
     * @see enableHorizontalScroll
     */
    void enableVerticalScroll(bool enable)
    {
        scrollableY = enable;
    }

    /**
     * Is the ClickableContainer scrollable in either direction? Takes the width of the
     * contained elements into account and also checks to see if horizontal or vertical
     * scrolling is allowed.
     *
     * @param [out] scrollX True if the container is able to scroll horizontally.
     * @param [out] scrollY True if the container is able to scroll vertically.
     *
     * @see enableHorizontalScroll, enableVerticalScroll
     */
    virtual void isScrollableXY(bool& scrollX, bool& scrollY)
    {
        Rect contained = getContainedArea();
        scrollX = (scrollableX && (rect.width < contained.width));
        scrollY = (scrollableY && (rect.height < contained.height));
    }

    /**
     * Sets the visibility of the scrollbars, when the scrollable area is pressed. By
     * default the scrollbars are hidden, but shown when the contents of the
     * ScrollableContainer is being dragged around. Using setScrollbarsVisible, it is
     * possble to hide the scrollbars when dragging the contents.
     *
     * @param  newVisible If true (default), the scrollbars are visible when scrollable area is
     *                    pressed. If false, scrollbars are always hidden.
     *
     * @see setScrollbarsPermanentlyVisible
     */
    void setScrollbarsVisible(bool newVisible);

    /**
     * Make scrollbars permanently visible regardless of the size and position of the
     * children of the ScrollableContainer. Normally the scrollbars are hidden and only
     * shown when dragging the contents of the ScrollableContainer (unless prohibited using
     * setScrollbarsVisible()).
     *
     * @see setScrollbarsVisible
     */
    void setScrollbarsPermanentlyVisible();

    virtual void add(Drawable& d);

    virtual void getLastChild(int16_t x, int16_t y, Drawable** last)
    {
        if (isVisible())
        {
            if (isTouchable())
            {
                *last = this;
            }
            else
            {
                Container::getLastChild(x, y, last);
            }
        }
    }

    virtual void handleClickEvent(const ClickEvent& evt);

    virtual void handleDragEvent(const DragEvent& evt);

    virtual void handleGestureEvent(const GestureEvent& evt);

    virtual void handleTickEvent();

    /**
     * Gets the area that contains all children added to the ScrollableContainer. The
     * scrollbars are not considered in this operation.
     *
     * @return The contained area.
     */
    virtual Rect getContainedArea() const;

    /**
     * Used to signal that the size or position of one or more children have changed. This
     * function can be called on parent nodes to signal that the size of one or more of its
     * children have changed.
     */
    virtual void childGeometryChanged();

    /**
     * Resets the ScrollableContainer to its original state, before the user started
     * dragging the contents. This reset the x/y coordinates of children to the position
     * they were in before the first drag event was received.
     */
    void reset();

    /**
     * @copydoc Container::moveChildrenRelative
     *
     * @note Takes care not to move the scrollbars, which are also children.
     */
    virtual void moveChildrenRelative(int16_t deltaX, int16_t deltaY);

    /**
     * Sets the maximum velocity of a scroll due to a swipe. This can be used to force
     * smooth scrolling by limiting the speed of any swipe gesture.
     *
     * @param  max The maximum velocity of the scroll.
     *
     * @see GestureEvent::getVelocity
     */
    void setMaxVelocity(uint16_t max)
    {
        maxVelocity = max;
    }

    /**
     * Change the threshold which the first drag event received must exceed before
     * initiating a scroll. This can be used to avoid touching the screen and moving the
     * finger only a few pixels resulting in the contents being scrolled.
     *
     * @param  t The new threshold value.
     *
     * @note All subsequent scrolls will be processed regardless of threshold value until a
     *       ClickEvent::RELEASED is received.
     */
    void setScrollThreshold(int16_t t)
    {
        scrollThreshold = t;
    }

    /**
     * Sets the color of the scrollbars.
     *
     * @param  color The color of the box.
     */
    void setScrollbarsColor(colortype color);

    /**
     * Sets the alpha value (transparency) of the scrollbars.
     *
     * @param  alpha The alpha value. 255 being completely solid, 0 being completely invisible.
     */
    void setScrollbarsAlpha(uint8_t alpha);

    /**
     * Sets the amount of space between the scrollbar and the edge of the ScrollableContainer.
     *
     * @param  padding The padding.
     */
    void setScrollbarPadding(uint8_t padding);

    /**
     * Sets the width of the scrollbar measured in pixels.
     *
     * @param  width The width of the scrollbar.
     */
    void setScrollbarWidth(uint8_t width);

    /**
     * Gets the distance scrolled for the x-axis.
     *
     * @return the distance scrolled for the x-axis.
     */
    int16_t getScrolledX() const;

    /**
     * Gets the distance scrolled for the y-axis.
     *
     * @return the distance scrolled for the y-axis.
     */
    int16_t getScrolledY() const;

    /**
     * Sets scroll duration speedup multiplier. Default value is 7 which gives a nice speedup on gestures.
     *
     * @param  speedup The scroll duration speedup multiplier.
     *
     * @see getScrollDurationSpeedup, setScrollDurationSlowdown
     */
    void setScrollDurationSpeedup(uint16_t speedup);

    /**
     * Gets scroll duration speedup multiplier.
     *
     * @return The swipe acceleration.
     *
     * @see setScrollDurationSpeedup, getScrollDurationSlowdown
     */
    uint16_t getScrollDurationSpeedup() const;

    /**
     * Sets scroll duration speedup divisor. Default value is 1.
     *
     * @param  slowdown The scroll duration speedup divisor.
     *
     * @see setScrollDurationSpeedup, getScrollDurationSlowdown
     */
    void setScrollDurationSlowdown(uint16_t slowdown);

    /**
     * Gets scroll duration speedup divisor.
     *
     * @return The scroll duration speedup divisor.
     *
     * @see setScrollDurationSlowdown
     */
    uint16_t getScrollDurationSlowdown() const;

protected:
    uint8_t scrollbarPadding;                          ///< The amount of padding. The scrollbar will have a bit of space to the borders of the container.
    uint8_t scrollbarWidth;                            ///< The width of the scrollbar.
    uint8_t scrollbarAlpha;                            ///< The scrollbar is semitransparent
    static const uint8_t SCROLLBAR_LINE = 0;           ///< The scrollbar line.
    colortype scrollbarColor;                          ///< The color of the scrollbar
    static const uint16_t SCROLLBAR_MIN_VELOCITY = 5;  ///< The minimum velocity of a scroll due to a swipe
    static const uint16_t SCROLLBAR_MAX_VELOCITY = 17; ///< The (default) maximum velocity of a scroll due to a swipe
    uint16_t maxVelocity;                              ///< The maximum velocity of a scroll (due to a swipe)

    /**
     * Gets x coordinate of the scrollbar.
     *
     * @return The horizontal scrollbar area.
     */
    Rect getXScrollbar() const;

    /**
     * Gets y coordinate of the scrollbar.
     *
     * @return The vertical scrollbar area.
     */
    Rect getYScrollbar() const;

    /**
     * Gets the area where the horizontal scrollbar can move.
     *
     * @param  xBar The current horizontal scrollbar, supplied for caching reasons.
     * @param  yBar The current vertical scrollbar, supplied for caching reasons.
     *
     * @return The area.
     */
    Rect getXBorder(const Rect& xBar, const Rect& yBar) const;

    /**
     * Gets the area where the vertical scrollbar can move.
     *
     * @param  xBar The current horizontal scrollbar, supplied for caching reasons.
     * @param  yBar The current vertical scrollbar, supplied for caching reasons.
     *
     * @return The area.
     */
    Rect getYBorder(const Rect& xBar, const Rect& yBar) const;

    /** Invalidate the scrollbars. */
    void invalidateScrollbars();

    /**
     * Method to actually scroll the container. Passing negative values will scroll the
     * items in the ScrollableContainer up / left, whereas positive values will scroll items
     * down / right.
     *
     * If the distance is larger than allowed, the deltas are adjusted down to make sure the
     * contained items stay inside view.
     *
     * @param  deltaX The horizontal amount to scroll.
     * @param  deltaY The vertical amount to scroll.
     *
     * @return did the container actually scroll. The call doScroll(0,0) will always return
     *         false.
     */
    virtual bool doScroll(int16_t deltaX, int16_t deltaY);

    GestureEvent::GestureEventType accelDirection; ///< The current direction (horizontal or vertical) of scroll

    Box xSlider; ///< The horizontal scrollbar drawable
    Box ySlider; ///< The vertical scrollbar drawable

    Drawable* pressedDrawable;    ///< The drawable child of this container which received the last ClickEvent::PRESSED notification. When scrolling, send this drawable a CANCEL event if the new x/y coords no longer matches this drawable.
    Drawable* lastDraggableChild; ///< The drawable child of this container which should receive drag events. Note that only drag events in directions which cannot be scrolled by this ScrollableContainer will be forwarded to children.

    int16_t scrolledXDistance; ///< The scrolled horizontal distance
    int16_t scrolledYDistance; ///< The scrolled vertical distance
    int16_t scrollThreshold;   ///< The threshold which the first drag event received must exceed before scrolling. Default is 5.

    int16_t pressedX; ///< The x coordinate where the last ClickEvent::PRESSED was received.
    int16_t pressedY; ///< The y coordinate where the last ClickEvent::PRESSED was received.

    bool isPressed;   ///< Is the container currently pressed (maybe show scrollbars)
    bool isScrolling; ///< Is the container scrolling (i.e. has overcome the initial larger drag that is required to initiate a scroll).

    bool scrollableX; ///< Is the container scrollable in the horizontal direction.
    bool scrollableY; ///< Is the container scrollable in the vertical direction.

    bool scrollbarsVisible;            ///< Are scrollbars visible.
    bool scrollbarsPermanentlyVisible; ///< Are scrollbars always visible.

    uint16_t scrollDuration; ///< Number of ticks the scroll animation should use.

    int16_t beginningValue; ///< Initial X or Y for calculated values in scroll animation.
    int16_t targetValue;    ///< Target X or Y value for scroll animation

    uint16_t animationCounter; ///< Current step/tick in scroll animation.
    bool animate;              ///< Is scroll animation currently active

    int16_t fingerAdjustmentX; ///< How much should the finger be adjusted horizontally
    int16_t fingerAdjustmentY; ///< and how much vertically

    bool hasIssuedCancelEvent; ///< true if the pressed drawable has received cancel event

    uint16_t scrollDurationSpeedup; ///< The scroll durations is multipled by this number
    uint16_t scrollDurationSlowdown; ///< The scroll durations is divided by this number
};

} // namespace touchgfx

#endif // SCROLLABLECONTAINER_HPP
