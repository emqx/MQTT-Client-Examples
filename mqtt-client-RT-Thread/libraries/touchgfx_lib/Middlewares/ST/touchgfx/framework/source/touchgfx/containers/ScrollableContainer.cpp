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

#include <touchgfx/containers/ScrollableContainer.hpp>
#include <touchgfx/EasingEquations.hpp>
#include <touchgfx/Color.hpp>

namespace touchgfx
{
ScrollableContainer::ScrollableContainer()
    : Container(),
      scrollbarPadding(0),
      scrollbarWidth(2),
      scrollbarAlpha(120),
      scrollbarColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF)),
      accelDirection(GestureEvent::SWIPE_HORIZONTAL),
      xSlider(0, 0, scrollbarColor, scrollbarAlpha),
      ySlider(0, 0, scrollbarColor, scrollbarAlpha),
      pressedDrawable(0),
      lastDraggableChild(0),
      scrolledXDistance(0),
      scrolledYDistance(0),
      scrollThreshold(5),
      pressedX(0),
      pressedY(0),
      isPressed(false),
      isScrolling(false),
      scrollableX(true),
      scrollableY(true),
      scrollbarsVisible(true),
      scrollbarsPermanentlyVisible(false),
      scrollDuration(0),
      beginningValue(0),
      targetValue(0),
      animationCounter(0),
      animate(false),
      fingerAdjustmentX(0),
      fingerAdjustmentY(0),
      hasIssuedCancelEvent(false),
      scrollDurationSpeedup(7),
      scrollDurationSlowdown(1)
{
    xSlider.setVisible(false);
    ySlider.setVisible(false);
    maxVelocity = SCROLLBAR_MAX_VELOCITY;
    setTouchable(true);
}

void ScrollableContainer::handleClickEvent(const ClickEvent& evt)
{
    if (evt.getType() == ClickEvent::PRESSED)
    {
        isPressed = true;

        if (animate)
        {
            // Stop scroll animation
            animate = false;
            animationCounter = 0;
            Application::getInstance()->unregisterTimerWidget(this);
        }

        const int fingerSize = HAL::getInstance()->getFingerSize();
        fingerAdjustmentX = 0;
        fingerAdjustmentY = 0;

        const int minimumDistance = 3;
        if ((fingerSize - 1) >= minimumDistance)
        {
            pressedDrawable = 0;

            const int maximumSquares = 3;
            const int numberOfSquares = MIN(maximumSquares, (fingerSize - 1) / minimumDistance);
            uint32_t bestDistance = 0xFFFFFFFF;
            Drawable* last = 0;

            Rect me(0, 0, getWidth(), getHeight());
            Rect meAbs = me;
            translateRectToAbsolute(meAbs);

            for (int squareNumber = 1; squareNumber <= numberOfSquares; squareNumber++)
            {
                int distance = ((squareNumber * fingerSize) / numberOfSquares);
                const int samplePoints[10][2] = { { 0, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 }, { -1, 0 }, { 0, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } };

                for (int sampleIndex = squareNumber - 1; sampleIndex < 10; sampleIndex += 2)
                {
                    Drawable* d = 0;
                    int16_t deltaX = samplePoints[sampleIndex][0] * distance;
                    int16_t deltaY = samplePoints[sampleIndex][1] * distance;
                    if (me.intersect(evt.getX() + deltaX, evt.getY() + deltaY))
                    {
                        Container::getLastChild(evt.getX() + deltaX, evt.getY() + deltaY, &d);
                        if (d && d != last && d != this)
                        {
                            Rect absRect = d->getAbsoluteRect();
                            int x = meAbs.x + evt.getX() - (absRect.x + (absRect.width / 2));
                            int y = meAbs.y + evt.getY() - (absRect.y + (absRect.height / 2));
                            uint32_t dist = x * x + y * y;
                            if (dist < bestDistance)
                            {
                                last = d;
                                bestDistance = dist;
                                pressedDrawable = d;
                                fingerAdjustmentX = deltaX;
                                fingerAdjustmentY = deltaY;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            Container::getLastChild(evt.getX(), evt.getY(), &pressedDrawable);
        }

        if (pressedDrawable == this)
        {
            pressedDrawable = 0;
        }

        if (pressedDrawable)
        {
            hasIssuedCancelEvent = false;
            pressedX = evt.getX();
            pressedY = evt.getY();
            Rect r = pressedDrawable->getAbsoluteRect();
            ClickEvent relative(evt.getType(), evt.getX() + rect.x + fingerAdjustmentX - r.x, evt.getY() + rect.y + fingerAdjustmentY - r.y);
            pressedDrawable->handleClickEvent(relative);
            lastDraggableChild = pressedDrawable;
        }
    }
    else if (evt.getType() == ClickEvent::CANCEL)
    {
        return;
    }
    else // if (evt.getType() == ClickEvent::RELEASED)
    {
        if (pressedDrawable)
        {
            Rect r = pressedDrawable->getAbsoluteRect();
            ClickEvent relative(evt.getType(), evt.getX() + rect.x + fingerAdjustmentX - r.x, evt.getY() + rect.y + fingerAdjustmentY - r.y);
            pressedDrawable->handleClickEvent(relative);
        }

        pressedDrawable = 0;
        lastDraggableChild = 0;
        isPressed = false;
    }

    isScrolling = false;

    // Redraw scrollbars.
    xSlider.setVisible((isPressed && scrollbarsVisible) || scrollbarsPermanentlyVisible);
    ySlider.setVisible((isPressed && scrollbarsVisible) || scrollbarsPermanentlyVisible);
    invalidateScrollbars();
}

void ScrollableContainer::handleDragEvent(const DragEvent& evt)
{
    DragEvent actualDrag = evt;
    bool acceptInitialScroll = false;

    bool canScrollX = false;
    bool canScrollY = false;
    isScrollableXY(canScrollX, canScrollY);

    if ((pressedDrawable != 0) && (pressedDrawable != this))
    {
        // Also send this drag event to the appropriate child widget
        Rect r = pressedDrawable->getAbsoluteRect();
        int16_t oldX = evt.getOldX() + rect.x + fingerAdjustmentX - r.x;
        int16_t oldY = evt.getOldY() + rect.y + fingerAdjustmentY - r.y;
        int16_t newX = canScrollX ? oldX : evt.getNewX() + rect.x + fingerAdjustmentX - r.x;
        int16_t newY = canScrollY ? oldY : evt.getNewY() + rect.y + fingerAdjustmentY - r.y;

        // but only in the direction(s) where the scrollable container itself
        // cannot scroll.
        if ((!canScrollX && newX != oldX) || (!canScrollY && newY != oldY))
        {
            DragEvent relative(DragEvent::DRAGGED, oldX, oldY, newX, newY);
            pressedDrawable->handleDragEvent(relative);
        }
    }

    // If we are not currently scrolling, the drag event delta must
    // be larger than the threshold value, otherwise the event is ignored.
    if (!isScrolling)
    {
        // Only consider the delta in directions that are actually scrollable.
        // Note: Do not use the delta from received evt since that only reflects
        // change since last drag. What we want to check here is if the total
        // delta from the point of click has now exceeded the threshold.
        actualDrag = DragEvent(DragEvent::DRAGGED, pressedX + fingerAdjustmentX, pressedY + fingerAdjustmentY, evt.getNewX() + fingerAdjustmentX, evt.getNewY() + fingerAdjustmentY);
        if (canScrollX)
        {
            // Can scroll in X.
            if (abs(actualDrag.getDeltaX()) > scrollThreshold)
            {
                acceptInitialScroll = true;
            }
        }

        if (canScrollY)
        {
            // Can scroll in Y.
            if (abs(actualDrag.getDeltaY()) > scrollThreshold)
            {
                acceptInitialScroll = true;
            }
        }

        if (acceptInitialScroll)
        {
            isScrolling = true;
        }
        else
        {
            // Discard this drag event. However, if the new coordinates no longer matches the drawable which received the PRESSED click event
            // issue a CANCEL event to that drawable.
            if (pressedDrawable && !hasIssuedCancelEvent)
            {
                Drawable* child = 0;
                Container::getLastChild(evt.getNewX() + fingerAdjustmentX, evt.getNewY() + fingerAdjustmentY, &child);
                if (pressedDrawable != child)
                {
                    ClickEvent ce(ClickEvent::CANCEL, 0, 0);
                    pressedDrawable->handleClickEvent(ce);
                    hasIssuedCancelEvent = true;
                }
            }
            return;
        }
    }

    // Send cancel events to child in focus
    if (pressedDrawable && !hasIssuedCancelEvent)
    {
        ClickEvent ce(ClickEvent::CANCEL, 0, 0);
        pressedDrawable->handleClickEvent(ce);
        hasIssuedCancelEvent = true;
    }

    int16_t deltaX = 0;
    int16_t deltaY = 0;

    if (scrollableX)
    {
        if (acceptInitialScroll)
        {
            // Initial drag which is past threshold, only scroll one pixel in initial event.
            if (actualDrag.getDeltaX() > 0)
            {
                deltaX = 1;
            }
            else if (actualDrag.getDeltaX() < 0)
            {
                deltaX = -1;
            }
        }
        else
        {
            // Scroll entire delta
            deltaX = actualDrag.getDeltaX();
        }
    }
    else
    {
        // Not scrollable
        deltaX = 0;
    }

    if (scrollableY)
    {
        if (acceptInitialScroll)
        {
            // Initial drag which is past threshold, only scroll one pixel in initial event.
            if (actualDrag.getDeltaY() > 0)
            {
                deltaY = 1;
            }
            else if (actualDrag.getDeltaY() < 0)
            {
                deltaY = -1;
            }
        }
        else
        {
            // Scroll entire delta
            deltaY = actualDrag.getDeltaY();
        }
    }
    else
    {
        // Not scrollable
        deltaY = 0;
    }

    doScroll(deltaX, deltaY);
}

void ScrollableContainer::handleGestureEvent(const GestureEvent& evt)
{
    bool canScrollX = false;
    bool canScrollY = false;
    isScrollableXY(canScrollX, canScrollY);

    if ((canScrollX && (evt.getType() == GestureEvent::SWIPE_HORIZONTAL)) ||
            (canScrollY && (evt.getType() == GestureEvent::SWIPE_VERTICAL)))
    {
        int16_t velocityAbsolute = abs(evt.getVelocity());

        // Ignore gestures with velocity lower than threshold
        if (velocityAbsolute < scrollThreshold)
        {
            return;
        }

        // Force velocity within limits
        velocityAbsolute = MAX(MIN(velocityAbsolute, maxVelocity), SCROLLBAR_MIN_VELOCITY);

        // Try to set some reasonable values for how long the resulting scroll should be, and how many ticks is should take
        scrollDuration = velocityAbsolute * scrollDurationSpeedup / scrollDurationSlowdown;
        targetValue = ((evt.getVelocity() > 0) ? 1 : -1) * (velocityAbsolute - 4) * 72;
        scrollDuration = MIN(scrollDuration, abs(targetValue));

        // Get ready to animate scroll: Initialize values
        beginningValue = (evt.getType() == GestureEvent::SWIPE_VERTICAL) ? getContainedArea().y : getContainedArea().x;
        animate = true;
        Application::getInstance()->registerTimerWidget(this);
        accelDirection = evt.getType();

        if (pressedDrawable && !hasIssuedCancelEvent)
        {
            ClickEvent ce(ClickEvent::CANCEL, 0, 0);
            pressedDrawable->handleClickEvent(ce);
            hasIssuedCancelEvent = true;
        }
    }
}

Rect ScrollableContainer::getXScrollbar() const
{
    Rect res(0, 0, 0, 0);
    if (scrollableX)
    {
        Rect contained = getContainedArea();
        const int scrollSpace = (scrollableY && (contained.height > rect.height)) ? (2 * scrollbarPadding + scrollbarWidth + SCROLLBAR_LINE) : 0;

        if (contained.width > rect.width)
        {
            int leftPadding = (-1 * contained.x * rect.width) / contained.width;
            int rightPadding = ((contained.right() - rect.width) * rect.width) / contained.width;
            const int startWidth = rect.width - 2 * scrollbarPadding - 2 * SCROLLBAR_LINE - scrollSpace;
            int width = startWidth;
            width -= (leftPadding + rightPadding);
            if (width < scrollbarWidth * 2)
            {
                // If the contained area is very large, the scrollbar width may become zero or even negative.
                int diff = scrollbarWidth * 2 - width;
                width = scrollbarWidth * 2; // Force scrollbar width to a minimum
                // Distribute the deviation error based on current scrollbar X position (the amount subtracted from scrollbar xpos increases gradually).
                leftPadding -= (diff * leftPadding) / startWidth;
            }
            res = Rect(leftPadding + scrollbarPadding + SCROLLBAR_LINE, rect.height - scrollbarWidth - scrollbarPadding - SCROLLBAR_LINE, width, scrollbarWidth);
        }
    }
    return res;
}

Rect ScrollableContainer::getYScrollbar() const
{
    Rect res(0, 0, 0, 0);
    if (scrollableY)
    {
        Rect contained = getContainedArea();
        const int scrollSpace = (scrollableX && (contained.width > rect.width)) ? (2 * scrollbarPadding + scrollbarWidth + SCROLLBAR_LINE) : 0;

        if (contained.height > rect.height)
        {
            int topPadding = (-1 * contained.y * rect.height) / contained.height;
            int bottomPadding = ((contained.bottom() - rect.height) * rect.height) / contained.height;
            const int startHeight = rect.height - 2 * scrollbarPadding - 2 * SCROLLBAR_LINE - scrollSpace;
            int height = startHeight;
            height -= (topPadding + bottomPadding);
            if (height < scrollbarWidth * 2)
            {
                // If the contained area is very large, the scrollbar height may become zero or even negative.
                int diff = scrollbarWidth * 2 - height;
                height = scrollbarWidth * 2; // Force scrollbar height to a minimum
                // Distribute the deviation error based on current scrollbar Y position (the amount subtracted from scrollbar ypos increases gradually).
                topPadding -= (diff * topPadding) / startHeight;
            }
            res = Rect(rect.width - scrollbarWidth - scrollbarPadding - 2 * SCROLLBAR_LINE, topPadding + scrollbarPadding + SCROLLBAR_LINE, scrollbarWidth, height);
        }
    }
    return res;
}

Rect ScrollableContainer::getXBorder(const Rect& xBar, const Rect& yBar) const
{
    Rect border(0, 0, 0, 0);
    if (!xBar.isEmpty())
    {
        const int scrollSpace = (!yBar.isEmpty()) ? (2 * scrollbarPadding + scrollbarWidth + SCROLLBAR_LINE) : 0;
        border = Rect(scrollbarPadding, xBar.y - SCROLLBAR_LINE, rect.width - 2 * scrollbarPadding - scrollSpace, scrollbarWidth + 2 * SCROLLBAR_LINE);
    }
    return border;
}

Rect ScrollableContainer::getYBorder(const Rect& xBar, const Rect& yBar) const
{
    Rect border(0, 0, 0, 0);
    if (!yBar.isEmpty())
    {
        const int scrollSpace = (!xBar.isEmpty()) ? (2 * scrollbarPadding + scrollbarWidth + SCROLLBAR_LINE) : 0;
        border = Rect(yBar.x - SCROLLBAR_LINE, scrollbarPadding, scrollbarWidth + 2 * SCROLLBAR_LINE, rect.height - 2 * scrollbarPadding - scrollSpace);
    }
    return border;
}

void ScrollableContainer::invalidateScrollbars()
{
    Rect xBar = getXScrollbar();
    Rect yBar = getYScrollbar();

    Rect xBorder = getXBorder(xBar, yBar);
    Rect yBorder = getYBorder(xBar, yBar);

    // The two if statements ensure that the two sliders is invalidates thereby hides them, before they are set to size zero.
    if (xSlider.getY() > xBorder.y)
    {
        xSlider.invalidate();
    }
    if (ySlider.getX() > yBorder.x)
    {
        ySlider.invalidate();
    }

    xSlider.setPosition(xBar.x, xBar.y, xBar.width, xBar.height);
    ySlider.setPosition(yBar.x, yBar.y, yBar.width, yBar.height);

    // x-/yBorder is given the coordinates zero and the witdh of the visiable area for the scrollable container,
    // to ensure that the entire area where for the scrollable bars is and have been is invalidated correct.
    xBorder.x = 0;
    xBorder.width = rect.width;
    yBorder.height = rect.height;
    yBorder.y = 0;

    if (!xBorder.isEmpty())
    {
        invalidateRect(xBorder);
    }

    if (!yBorder.isEmpty())
    {
        invalidateRect(yBorder);
    }
}

bool ScrollableContainer::doScroll(int16_t deltaX, int16_t deltaY)
{
    if (!deltaX && !deltaY)
    {
        return false;
    }
    bool couldScroll = false;
    Rect contained = getContainedArea();
    if (contained.width > rect.width)
    {
        if (deltaX > 0)
        {
            if (contained.x + deltaX > 0)
            {
                deltaX = -contained.x;
            }
        }
        else if (deltaX < 0)
        {
            if (contained.right() + deltaX < rect.width)
            {
                deltaX = rect.width - contained.right();
            }
        }
    }
    else
    {
        deltaX = 0;
    }

    if (contained.height > rect.height)
    {
        if (deltaY > 0)
        {
            if (contained.y + deltaY > 0)
            {
                deltaY = -contained.y;
            }
        }
        else if (deltaY < 0)
        {
            if (contained.bottom() + deltaY < rect.height)
            {
                deltaY = rect.height - contained.bottom();
            }
        }
    }
    else
    {
        deltaY = 0;
    }

    if (deltaX || deltaY)
    {
        scrolledXDistance += deltaX;
        scrolledYDistance += deltaY;
        moveChildrenRelative(deltaX, deltaY);

        invalidateScrollbars();
        couldScroll = true;
    }
    return couldScroll;
}

void ScrollableContainer::childGeometryChanged()
{
    Rect contained = getContainedArea();
    // If children are not aligned top left, make sure they are
    if (contained.y > 0)
    {
        moveChildrenRelative(0, -contained.y);
    }
    if (contained.x > 0)
    {
        moveChildrenRelative(-contained.x, 0);
    }
    // Make sure we haven't scrolled below the bottom
    if (contained.bottom() < rect.height)
    {
        int16_t deltaY = contained.bottom() - rect.height;
        if (contained.y > deltaY)
        {
            deltaY = contained.y;
        }
        scrolledYDistance -= deltaY;
        moveChildrenRelative(0, -deltaY);
    }
    // Make sure we haven't scrolled too far to the right
    if (contained.right() < rect.width)
    {
        int deltaX = contained.right() - rect.width;
        if (contained.x > deltaX)
        {
            deltaX = contained.x;
        }
        scrolledXDistance -= deltaX;
        moveChildrenRelative(-deltaX, 0);
    }
    invalidateScrollbars();
}

void ScrollableContainer::add(Drawable& d)
{
    remove(xSlider);
    remove(ySlider);

    Container::add(d);
    Container::add(xSlider);
    Container::add(ySlider);
}

Rect ScrollableContainer::getContainedArea() const
{
    Drawable* d = firstChild;
    Rect contained(0, 0, 0, 0);
    Rect r(0, 0, rect.width, rect.height);
    contained.expandToFit(r);
    while (d)
    {
        if ((d != &xSlider) && (d != &ySlider) && (d->isVisible()))
        {
            contained.expandToFit(d->getRect());
        }
        d = d->getNextSibling();
    }
    return contained;
}

void ScrollableContainer::reset()
{
    moveChildrenRelative(-scrolledXDistance, -scrolledYDistance);
    scrolledXDistance = 0;
    scrolledYDistance = 0;
    invalidateScrollbars();
}

void ScrollableContainer::moveChildrenRelative(int16_t deltaX, int16_t deltaY)
{
    Drawable* d = firstChild;
    while (d)
    {
        if ((d != &xSlider) && (d != &ySlider))
        {
            d->moveRelative(deltaX, deltaY);
        }
        d = d->getNextSibling();
    }
}

void ScrollableContainer::handleTickEvent()
{
    if (animate)
    {
        // Calculate new position or stop animation
        animationCounter++;
        if (animationCounter <= scrollDuration)
        {
            // Calculate value in [beginningValue ; (beginningValue+targetValue)]
            int16_t calculatedValue = EasingEquations::cubicEaseOut(animationCounter, beginningValue, targetValue, scrollDuration);

            // Note: Result of "calculatedValue & 1" is compiler dependent for negative values of calculatedValue
            if (calculatedValue % 2)
            {
                // Optimization: calculatedValue is odd, add 1/-1 to move drawables modulo 32 bits in framebuffer
                calculatedValue += (calculatedValue > 0) ? 1 : -1;
            }

            // Convert to delta value relative to current X or Y
            int16_t scrollX = (accelDirection == GestureEvent::SWIPE_VERTICAL) ? 0 : (calculatedValue - getContainedArea().x);
            int16_t scrollY = (accelDirection == GestureEvent::SWIPE_HORIZONTAL) ? 0 : (calculatedValue - getContainedArea().y);

            // Perform the actual animation step, stop animation if
            // scrolling was not possible (doScroll invalidates children)
            animate = doScroll(scrollX, scrollY);
        }
        else
        {
            animate = false;
        }

        if (!animate)
        {
            Application::getInstance()->unregisterTimerWidget(this);
            animationCounter = 0;
        }
    }
}

void ScrollableContainer::setScrollbarsColor(colortype color)
{
    scrollbarColor = color;
    xSlider.setColor(scrollbarColor);
    ySlider.setColor(scrollbarColor);
}

void ScrollableContainer::setScrollbarsAlpha(uint8_t alpha)
{
    scrollbarAlpha = alpha;
    xSlider.setAlpha(scrollbarAlpha);
    ySlider.setAlpha(scrollbarAlpha);
}

void ScrollableContainer::setScrollbarPadding(uint8_t padding)
{
    scrollbarPadding = padding;
}

void ScrollableContainer::setScrollbarWidth(uint8_t width)
{
    scrollbarWidth = width;
}

void ScrollableContainer::setScrollbarsVisible(bool newVisible)
{
    scrollbarsVisible = newVisible;
}

void ScrollableContainer::setScrollbarsPermanentlyVisible()
{
    scrollbarsPermanentlyVisible = true;
    xSlider.setVisible(true);
    ySlider.setVisible(true);
    invalidateScrollbars();
}

int16_t ScrollableContainer::getScrolledX() const
{
    return scrolledXDistance;
}

int16_t ScrollableContainer::getScrolledY() const
{
    return scrolledYDistance;
}

void ScrollableContainer::setScrollDurationSpeedup(uint16_t speedup)
{
    scrollDurationSpeedup = MAX(1, speedup);
}

uint16_t ScrollableContainer::getScrollDurationSpeedup() const
{
    return scrollDurationSpeedup;
}

void ScrollableContainer::setScrollDurationSlowdown(uint16_t slowdown)
{
    scrollDurationSlowdown = MAX(1, slowdown);
}

uint16_t ScrollableContainer::getScrollDurationSlowdown() const
{
    return scrollDurationSlowdown;
}

} // namespace touchgfx
