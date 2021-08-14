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

#include <touchgfx/containers/ZoomAnimationImage.hpp>

namespace touchgfx
{
ZoomAnimationImage::ZoomAnimationImage() :
    Container(),
    currentState(NO_ANIMATION),
    animationCounter(0),
    zoomAnimationDelay(0),
    currentZoomMode(FIXED_LEFT_AND_TOP),
    zoomAnimationStartWidth(0),
    zoomAnimationStartHeight(0),
    zoomAnimationEndWidth(0),
    zoomAnimationEndHeight(0),
    zoomAnimationStartX(0),
    zoomAnimationStartY(0),
    zoomAnimationDeltaX(0),
    zoomAnimationDeltaY(0),
    moveAnimationEndX(0),
    moveAnimationEndY(0),
    animationDuration(0),
    zoomAnimationWidthEquation(EasingEquations::linearEaseNone),
    zoomAnimationHeightEquation(EasingEquations::linearEaseNone),
    moveAnimationXEquation(EasingEquations::linearEaseNone),
    moveAnimationYEquation(EasingEquations::linearEaseNone),
    animationEndedAction(0)
{
    image.setXY(0, 0);
    image.setVisible(false);

    scalableImage.setScalingAlgorithm(ScalableImage::NEAREST_NEIGHBOR);
    scalableImage.setXY(0, 0);
    scalableImage.setVisible(false);

    Container::add(image);
    Container::add(scalableImage);
}


void ZoomAnimationImage::startZoomAnimation(int16_t endWidth, int16_t endHeight, uint16_t duration, ZoomMode zoomMode, EasingEquation widthProgressionEquation, EasingEquation heightProgressionEquation)
{
    setCurrentState(ANIMATE_ZOOM);

    startTimerAndSetParameters(endWidth, endHeight, duration, zoomMode, widthProgressionEquation, heightProgressionEquation);
}

void ZoomAnimationImage::startZoomAndMoveAnimation(int16_t endX, int16_t endY, int16_t endWidth, int16_t endHeight, uint16_t duration, ZoomMode zoomMode, EasingEquation xProgressionEquation, EasingEquation yProgressionEquation, EasingEquation widthProgressionEquation, EasingEquation heightProgressionEquation)
{
    moveAnimationEndX = endX;
    moveAnimationEndY = endY;

    moveAnimationXEquation = xProgressionEquation;
    moveAnimationYEquation = yProgressionEquation;

    setCurrentState(ANIMATE_ZOOM_AND_MOVE);

    startTimerAndSetParameters(endWidth, endHeight, duration, zoomMode, widthProgressionEquation, heightProgressionEquation);
}

void ZoomAnimationImage::cancelZoomAnimation()
{
    touchgfx::Application::getInstance()->unregisterTimerWidget(this);
    setCurrentState(NO_ANIMATION);
}

void ZoomAnimationImage::handleTickEvent()
{
    if ((currentState == ANIMATE_ZOOM) || (currentState == ANIMATE_ZOOM_AND_MOVE))
    {
        if (animationCounter < zoomAnimationDelay)
        {
            // Just wait for the delay time to pass
            animationCounter++;
        }
        else if (animationCounter <= (uint32_t)(zoomAnimationDelay + animationDuration))
        {
            // Adjust the used animationCounter for the startup delay
            uint32_t actualAnimationCounter = animationCounter - zoomAnimationDelay;

            int16_t deltaWidth = zoomAnimationWidthEquation(actualAnimationCounter, 0, zoomAnimationEndWidth - zoomAnimationStartWidth, animationDuration);
            int16_t deltaHeight = zoomAnimationHeightEquation(actualAnimationCounter, 0, zoomAnimationEndHeight - zoomAnimationStartHeight, animationDuration);

            setDimension(zoomAnimationStartWidth + deltaWidth, zoomAnimationStartHeight + deltaHeight);

            int16_t deltaX;
            int16_t deltaY;

            if (currentState == ANIMATE_ZOOM_AND_MOVE)
            {
                deltaX = moveAnimationXEquation(actualAnimationCounter, 0, (moveAnimationEndX - zoomAnimationStartX) + zoomAnimationDeltaX, animationDuration);
                deltaY = moveAnimationYEquation(actualAnimationCounter, 0, (moveAnimationEndY - zoomAnimationStartY) + zoomAnimationDeltaY, animationDuration);
            }
            else
            {
                deltaX = zoomAnimationWidthEquation(actualAnimationCounter, 0, zoomAnimationDeltaX, animationDuration);
                deltaY = zoomAnimationHeightEquation(actualAnimationCounter, 0, zoomAnimationDeltaY, animationDuration);
            }
            moveTo(zoomAnimationStartX + deltaX, zoomAnimationStartY + deltaY);
            animationCounter++;
        }
        else
        {
            cancelZoomAnimation();

            if (animationEndedAction && animationEndedAction->isValid())
            {
                animationEndedAction->execute(*this);
            }
        }
    }
}

void ZoomAnimationImage::setBitmaps(const Bitmap& smallBitmap, const Bitmap& largeBitmap)
{
    smallBmp = smallBitmap;
    largeBmp = largeBitmap;

    scalableImage.setBitmap(largeBmp);

    touchgfx::Container::setWidth(largeBmp.getWidth());
    touchgfx::Container::setHeight(largeBmp.getHeight());
    updateRenderingMethod();
}

void ZoomAnimationImage::setPosition(int16_t x, int16_t y, int16_t width, int16_t height)
{
    invalidate();
    touchgfx::Container::setPosition(x, y, width, height);
    updateRenderingMethod();
}

void ZoomAnimationImage::setWidth(int16_t width)
{
    setDimension(width, getHeight());
}

void ZoomAnimationImage::setHeight(int16_t height)
{
    setDimension(getWidth(), height);
}

void ZoomAnimationImage::setDimension(int16_t width, int16_t height)
{
    invalidate();
    touchgfx::Container::setWidth(width);
    touchgfx::Container::setHeight(height);
    updateRenderingMethod();
}

void ZoomAnimationImage::setScalingMode(ScalableImage::ScalingAlgorithm mode)
{
    scalableImage.setScalingAlgorithm(mode);
}

ScalableImage::ScalingAlgorithm ZoomAnimationImage::getScalingMode()
{
    return scalableImage.getScalingAlgorithm();
}

void ZoomAnimationImage::setAlpha(const uint8_t alpha)
{
    image.setAlpha(alpha);
    scalableImage.setAlpha(alpha);
}

uint8_t ZoomAnimationImage::getAlpha() const
{
    return image.getAlpha();
}

void ZoomAnimationImage::setAnimationDelay(uint16_t delay)
{
    zoomAnimationDelay = delay;
}

uint16_t ZoomAnimationImage::getAnimationDelay() const
{
    return zoomAnimationDelay;
}

bool ZoomAnimationImage::isRunning()
{
    return isZoomAnimationRunning();
}

bool ZoomAnimationImage::isZoomAnimationRunning() const
{
    return currentState != NO_ANIMATION;
}

void ZoomAnimationImage::updateRenderingMethod()
{
    if ((smallBmp.getWidth() == getWidth()) && (smallBmp.getHeight() == getHeight()))
    {
        image.setVisible(true);
        scalableImage.setVisible(false);
        image.setBitmap(smallBmp);
        image.invalidate();
        scalableImage.invalidate();
    }
    else if ((largeBmp.getWidth() == getWidth()) && (largeBmp.getHeight() == getHeight()))
    {
        image.setVisible(true);
        scalableImage.setVisible(false);
        image.setBitmap(largeBmp);
        image.invalidate();
        scalableImage.invalidate();
    }
    else
    {
        image.setVisible(false);
        image.invalidate();
        scalableImage.setVisible(true);
        scalableImage.setWidth(getWidth());
        scalableImage.setHeight(getHeight());
        scalableImage.invalidate();
    }
}

void ZoomAnimationImage::setCurrentState(States state)
{
    currentState = state;
    animationCounter = 0;
}

void ZoomAnimationImage::startTimerAndSetParameters(int16_t endWidth, int16_t endHeight, uint16_t duration, ZoomMode zoomMode, EasingEquation widthProgressionEquation, EasingEquation heightProgressionEquation)
{
    touchgfx::Application::getInstance()->registerTimerWidget(this);

    currentZoomMode = zoomMode;

    zoomAnimationStartX = getX();
    zoomAnimationStartY = getY();
    zoomAnimationStartWidth = getWidth();
    zoomAnimationStartHeight = getHeight();
    zoomAnimationEndWidth = endWidth;
    zoomAnimationEndHeight = endHeight;
    animationDuration = duration;

    zoomAnimationWidthEquation = widthProgressionEquation;
    zoomAnimationHeightEquation = heightProgressionEquation;

    updateZoomAnimationDeltaXY();

    if (zoomAnimationDelay == 0 && animationDuration == 0)
    {
        handleTickEvent(); // Finish the zoom and move operation immediately
    }
}

void ZoomAnimationImage::updateZoomAnimationDeltaXY()
{
    zoomAnimationDeltaX = zoomAnimationStartWidth - zoomAnimationEndWidth;
    zoomAnimationDeltaY = zoomAnimationStartHeight - zoomAnimationEndHeight;

    switch (currentZoomMode)
    {
    case ZoomAnimationImage::FIXED_CENTER:
        zoomAnimationDeltaX /= 2;
        zoomAnimationDeltaY /= 2;
        break;
    case ZoomAnimationImage::FIXED_LEFT:
        zoomAnimationDeltaX = 0;
        zoomAnimationDeltaY /= 2;
        break;
    case ZoomAnimationImage::FIXED_RIGHT:
        zoomAnimationDeltaY /= 2;
        break;
    case ZoomAnimationImage::FIXED_TOP:
        zoomAnimationDeltaX /= 2;
        zoomAnimationDeltaY = 0;
        break;
    case ZoomAnimationImage::FIXED_BOTTOM:
        zoomAnimationDeltaX /= 2;
        break;
    case ZoomAnimationImage::FIXED_LEFT_AND_TOP:
        zoomAnimationDeltaX = 0;
        zoomAnimationDeltaY = 0;
        break;
    case ZoomAnimationImage::FIXED_RIGHT_AND_TOP:
        zoomAnimationDeltaY = 0;
        break;
    case ZoomAnimationImage::FIXED_LEFT_AND_BOTTOM:
        zoomAnimationDeltaX = 0;
        break;
    case ZoomAnimationImage::FIXED_RIGHT_AND_BOTTOM:
        break;
    default:
        break;
    }
}
} // namespace touchgfx
