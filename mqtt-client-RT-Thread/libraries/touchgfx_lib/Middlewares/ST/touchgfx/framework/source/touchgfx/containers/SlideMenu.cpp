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

#include <touchgfx/containers/SlideMenu.hpp>

namespace touchgfx
{
SlideMenu::SlideMenu() :
    Container(),
    onStateChangeButtonClicked(this, &SlideMenu::stateChangeButtonClickedHandler),
    animationEndedCallback(this, &SlideMenu::animationEndedHandler),
    stateChangedCallback(0),
    stateChangedAnimationEndedCallback(0),
    currentState(COLLAPSED),
    expandDirection(EAST),
    animationEquation(EasingEquations::cubicEaseInOut),
    visiblePixelsWhenCollapsed(0),
    hiddenPixelsWhenExpanded(0),
    expandedStateTimeout(200),
    expandedStateTimer(0),
    animationDuration(10)
{
    Application::getInstance()->registerTimerWidget(this);

    stateChangeButton.setAction(onStateChangeButtonClicked);
    menuContainer.setMoveAnimationEndedAction(animationEndedCallback);

    Container::add(menuContainer);
    menuContainer.add(background);
    menuContainer.add(stateChangeButton);
}

SlideMenu::~SlideMenu()
{
    Application::getInstance()->unregisterTimerWidget(this); //lint !e1551
}

void SlideMenu::setup(ExpandDirection newExpandDirection, const Bitmap& backgroundBMP, const Bitmap& stateChangeButtonBMP, const Bitmap& stateChangeButtonPressedBMP)
{
    int16_t backgroundX = 0;
    int16_t backgroundY = 0;
    int16_t buttonX = 0;
    int16_t buttonY = 0;

    switch (newExpandDirection)
    {
    case touchgfx::SlideMenu::SOUTH:
        backgroundX = 0;
        backgroundY = 0;
        buttonX = (backgroundBMP.getWidth() - stateChangeButtonBMP.getWidth()) / 2;
        buttonY = backgroundBMP.getHeight();

        setVisiblePixelsWhenCollapsed(stateChangeButtonBMP.getHeight());
        break;
    case touchgfx::SlideMenu::NORTH:
        backgroundX = 0;
        backgroundY = stateChangeButtonBMP.getHeight();
        buttonX = (backgroundBMP.getWidth() - stateChangeButtonBMP.getWidth()) / 2;
        buttonY = 0;

        setVisiblePixelsWhenCollapsed(stateChangeButtonBMP.getHeight());
        break;
    case touchgfx::SlideMenu::EAST:
        backgroundX = 0;
        backgroundY = 0;
        buttonX = backgroundBMP.getWidth();
        buttonY = (backgroundBMP.getHeight() - stateChangeButtonBMP.getHeight()) / 2;

        setVisiblePixelsWhenCollapsed(stateChangeButtonBMP.getWidth());
        break;
    case touchgfx::SlideMenu::WEST:
        backgroundX = stateChangeButtonBMP.getWidth();
        backgroundY = 0;
        buttonX = 0;
        buttonY = (backgroundBMP.getHeight() - stateChangeButtonBMP.getHeight()) / 2;

        setVisiblePixelsWhenCollapsed(stateChangeButtonBMP.getWidth());
        break;
    default:
        break;
    }

    setup(newExpandDirection, backgroundBMP, stateChangeButtonBMP, stateChangeButtonPressedBMP, backgroundX, backgroundY, buttonX, buttonY);
}

void SlideMenu::setup(ExpandDirection newExpandDirection, const Bitmap& backgroundBMP, int16_t backgroundX, int16_t backgroundY)
{
    setExpandDirection(newExpandDirection);

    background.setBitmap(backgroundBMP);
    background.setXY(backgroundX, backgroundY);

    Rect boundingRect = background.getRect();
    // boundingRect.expandToFit(background.getRect());

    menuContainer.setWidth(boundingRect.right());
    menuContainer.setHeight(boundingRect.bottom());

    setWidth(menuContainer.getWidth());
    setHeight(menuContainer.getHeight());

    setExpandDirection(expandDirection);
    setState(currentState);

    invalidate();
}

void SlideMenu::setup(ExpandDirection newExpandDirection, const Bitmap& backgroundBMP, const Bitmap& stateChangeButtonBMP, const Bitmap& stateChangeButtonPressedBMP, int16_t backgroundX, int16_t backgroundY, int16_t stateChangeButtonX, int16_t stateChangeButtonY)
{
    setExpandDirection(newExpandDirection);

    background.setBitmap(backgroundBMP);
    background.setXY(backgroundX, backgroundY);

    stateChangeButton.setBitmaps(stateChangeButtonBMP, stateChangeButtonPressedBMP);
    stateChangeButton.setXY(stateChangeButtonX, stateChangeButtonY);

    Rect boundingRect(0, 0, 0, 0);
    boundingRect.expandToFit(background.getRect());
    boundingRect.expandToFit(stateChangeButton.getRect());

    menuContainer.setWidth(boundingRect.right());
    menuContainer.setHeight(boundingRect.bottom());

    setWidth(menuContainer.getWidth());
    setHeight(menuContainer.getHeight());

    setExpandDirection(expandDirection);
    setState(currentState);

    invalidate();
}

void SlideMenu::setExpandDirection(ExpandDirection newExpandDirection)
{
    expandDirection = newExpandDirection;

    setState(currentState);
}

touchgfx::SlideMenu::ExpandDirection SlideMenu::getExpandDirection() const
{
    return expandDirection;
}

void SlideMenu::setVisiblePixelsWhenCollapsed(int16_t visiblePixels)
{
    visiblePixelsWhenCollapsed = visiblePixels;

    setState(currentState);
}

int16_t SlideMenu::getVisiblePixelsWhenCollapsed() const
{
    return visiblePixelsWhenCollapsed;
}

void SlideMenu::setHiddenPixelsWhenExpanded(int16_t hiddenPixels)
{
    hiddenPixelsWhenExpanded = hiddenPixels;

    setState(currentState);
}

int16_t SlideMenu::getHiddenPixelsWhenExpanded() const
{
    return hiddenPixelsWhenExpanded;
}

void SlideMenu::setExpandedStateTimeout(uint16_t timeout)
{
    expandedStateTimeout = timeout;
}

uint16_t SlideMenu::getExpandedStateTimeout() const
{
    return expandedStateTimeout;
}

void SlideMenu::setAnimationDuration(uint16_t duration)
{
    animationDuration = duration;
}

uint16_t SlideMenu::getAnimationDuration() const
{
    return animationDuration;
}

void SlideMenu::setAnimationEasingEquation(EasingEquation animationEasingEquation)
{
    animationEquation = animationEasingEquation;
}

touchgfx::EasingEquation SlideMenu::getAnimationEasingEquation() const
{
    return animationEquation;
}

void SlideMenu::setState(State newState)
{
    if (newState == COLLAPSED)
    {
        menuContainer.moveTo(getCollapsedXCoordinate(), getCollapsedYCoordinate());
    }
    else
    {
        menuContainer.moveTo(getExpandedXCoordinate(), getExpandedYCoordinate());
    }
    currentState = newState;
}

void SlideMenu::animateToState(State newState)
{
    if (animationDuration == 0)
    {
        setState(newState);
    }
    else if (newState != currentState)
    {
        if (currentState == COLLAPSED)
        {
            menuContainer.startMoveAnimation(getExpandedXCoordinate(), getExpandedYCoordinate(), animationDuration, animationEquation, animationEquation);
            currentState = EXPANDED;
        }
        else
        {
            menuContainer.startMoveAnimation(getCollapsedXCoordinate(), getCollapsedYCoordinate(), animationDuration, animationEquation, animationEquation);
            currentState = COLLAPSED;
        }

        // Disable stateChangeButton while animating
        stateChangeButton.setTouchable(false);
    }
}

touchgfx::SlideMenu::State SlideMenu::getState()
{
    return currentState;
}

void SlideMenu::resetExpandedStateTimer()
{
    expandedStateTimer = 0;
}

uint16_t SlideMenu::getExpandedStateTimer() const
{
    return expandedStateTimer;
}

int16_t SlideMenu::getBackgroundX() const
{
    return background.getX();
}

int16_t SlideMenu::getBackgroundY() const
{
    return background.getY();
}

int16_t SlideMenu::getStateChangeButtonX() const
{
    return stateChangeButton.getX();
}

int16_t SlideMenu::getStateChangeButtonY() const
{
    return stateChangeButton.getY();
}

void SlideMenu::setStateChangedCallback(GenericCallback< const SlideMenu& >& callback)
{
    stateChangedCallback = &callback;
}

void SlideMenu::setStateChangedAnimationEndedCallback(GenericCallback< const SlideMenu& >& callback)
{
    stateChangedAnimationEndedCallback = &callback;
}

void SlideMenu::handleTickEvent()
{
    if ((expandedStateTimeout != 0) && (currentState == EXPANDED) && !menuContainer.isMoveAnimationRunning())
    {
        expandedStateTimer++;

        if (expandedStateTimer > expandedStateTimeout)
        {
            animateToState(COLLAPSED);
        }
    }
}

void SlideMenu::add(Drawable& d)
{
    menuContainer.add(d);
}

void SlideMenu::remove(Drawable& d)
{
    menuContainer.remove(d);
}

void SlideMenu::stateChangeButtonClickedHandler(const AbstractButton& /*button*/)
{
    if (currentState == COLLAPSED)
    {
        animateToState(EXPANDED);
    }
    else
    {
        animateToState(COLLAPSED);
    }

    if ((stateChangedCallback != 0) && stateChangedCallback->isValid())
    {
        stateChangedCallback->execute(*this);
    }
}

void SlideMenu::animationEndedHandler(const MoveAnimator<Container>& /*container*/)
{
    resetExpandedStateTimer();
    stateChangeButton.setTouchable(true);

    if ((stateChangedAnimationEndedCallback != 0) && stateChangedAnimationEndedCallback->isValid())
    {
        stateChangedAnimationEndedCallback->execute(*this);
    }
}

int16_t SlideMenu::getCollapsedXCoordinate()
{
    switch (expandDirection)
    {
    case touchgfx::SlideMenu::EAST:
        return -menuContainer.getWidth() + visiblePixelsWhenCollapsed;
    case touchgfx::SlideMenu::WEST:
        return getWidth() - visiblePixelsWhenCollapsed;
    case touchgfx::SlideMenu::SOUTH:
    case touchgfx::SlideMenu::NORTH:
    default:
        return 0;
    }
}

int16_t SlideMenu::getCollapsedYCoordinate()
{
    switch (expandDirection)
    {
    case touchgfx::SlideMenu::SOUTH:
        return -menuContainer.getHeight() + visiblePixelsWhenCollapsed;
    case touchgfx::SlideMenu::NORTH:
        return getHeight() - visiblePixelsWhenCollapsed;
    case touchgfx::SlideMenu::EAST:
    case touchgfx::SlideMenu::WEST:
    default:
        return 0;
    }
}

int16_t SlideMenu::getExpandedXCoordinate()
{
    switch (expandDirection)
    {
    case touchgfx::SlideMenu::EAST:
        return -hiddenPixelsWhenExpanded;
    case touchgfx::SlideMenu::WEST:
        return hiddenPixelsWhenExpanded;
    case touchgfx::SlideMenu::SOUTH:
    case touchgfx::SlideMenu::NORTH:
    default:
        return 0;
    }
}

int16_t SlideMenu::getExpandedYCoordinate()
{
    switch (expandDirection)
    {
    case touchgfx::SlideMenu::SOUTH:
        return -hiddenPixelsWhenExpanded;
    case touchgfx::SlideMenu::NORTH:
        return hiddenPixelsWhenExpanded;
    case touchgfx::SlideMenu::EAST:
    case touchgfx::SlideMenu::WEST:
    default:
        return 0;
    }
}
} // namespace touchgfx
