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
 * @file touchgfx/containers/ZoomAnimationImage.hpp
 *
 * Declares the touchgfx::ZoomAnimationImage class.
 */
#ifndef ZOOMANIMATIONIMAGE_HPP
#define ZOOMANIMATIONIMAGE_HPP

#include <touchgfx/EasingEquations.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/ScalableImage.hpp>

namespace touchgfx
{
/**
 * Class for optimizing and wrapping move and zoom operations on a ScalableImage. The
 * ZoomAnimationImage takes two bitmaps representing the same image but at a small and a
 * large resolution. These bitmaps should be the sizes that are used when not animating
 * the image. The ZoomAnimationImage will use an Image for displaying the Bitmap when
 * its width and height matches either of them. When it does not match the size of one
 * of the bitmaps, it will use a ScalableImage instead. The main idea is that the
 * supplied bitmaps should be the end points of the zoom animation so that it ends up
 * using an Image when not animating. This is, however, not a requirement. You can
 * animate from and to sizes that are not equal the sizes of the bitmaps. The result is
 * a container that has the high performance of an ordinary image when the size matches
 * the pre-rendered bitmaps. Moreover it supplies easy to use animation functions that
 * lets you zoom and move the image.
 *
 * @note Since this container uses the ScalableImage it has the same restrictions as a
 *       ScaleableImage, i.e. 1bpp is not supported.
 */
class ZoomAnimationImage : public Container
{
public:
    /**
     * A ZoomMode describes in which direction the image will grow/shrink when do a zoom
     * animation. A FIXED direction means that the image will not grow/shrink in that
     * direction.
     */
    enum ZoomMode
    {
        FIXED_CENTER = 0,      ///< The small image will grow from the center of the large image
        FIXED_LEFT,            ///< The small image will grow from the middle of the left side of the large image
        FIXED_RIGHT,           ///< The small image will grow from the middle of the right side of the large image
        FIXED_TOP,             ///< The small image will grow from the middle of the top of the large image
        FIXED_BOTTOM,          ///< The small image will grow from the middle of the bottom of the large image
        FIXED_LEFT_AND_TOP,    ///< The small image will grow from the top left corner of the large image
        FIXED_RIGHT_AND_TOP,   ///< The small image will grow from the top right corner of the large image
        FIXED_LEFT_AND_BOTTOM, ///< The small image will grow from the bottom left corner of the large image
        FIXED_RIGHT_AND_BOTTOM ///< The small image will grow from the bottom right corner of the large image
    };

    ZoomAnimationImage();

    /**
     * Setup and starts the zoom animation. At end of the animation the image will have been
     * resized to the endWidth and endHeight. The development of the width and height during
     * the animation is described by the supplied EasingEquations. The container is
     * registered as a TimerWidget and automatically unregistered when the animation has
     * finished.
     *
     * @param  endWidth                  The width of the image at animation end.
     * @param  endHeight                 The height of the image at animation end.
     * @param  duration                  The duration of the animation measured in ticks.
     * @param  zoomMode                  (Optional) The zoom mode that will be used during the
     *                                   animation. Default is #FIXED_LEFT_AND_TOP.
     * @param  widthProgressionEquation  (Optional) The equation that describes the
     *                                   development of the width during the animation.
     *                                   Default is EasingEquations::linearEaseNone.
     * @param  heightProgressionEquation (Optional) The equation that describes the
     *                                   development of the height during the animation.
     *                                   Default is EasingEquations::linearEaseNone.
     *
     * @note The animation follows the specified ZoomMode so the X and Y coordinates of the image
     *       might change during animation.
     */
    void startZoomAnimation(int16_t endWidth, int16_t endHeight, uint16_t duration, ZoomMode zoomMode = FIXED_LEFT_AND_TOP, EasingEquation widthProgressionEquation = &EasingEquations::linearEaseNone, EasingEquation heightProgressionEquation = &EasingEquations::linearEaseNone);

    /**
     * Setup and starts the zoom and move animation. At end of the animation the image will
     * have been resized to the endWidth and endHeight and have moved from its original
     * position to the endX and endY. Please note that the ZoomMode might influence the
     * actual end position since the zoom transformation might change the X and Y of the
     * image. The ZoomMode #FIXED_LEFT_AND_TOP ensures that the endX and endY will be the
     * actual end position.
     *
     * The development of the width, height, X and Y during the animation is described by
     * the supplied EasingEquations. The container is registered as a TimerWidget and
     * automatically unregistered when the animation has finished.
     *
     * @param  endX                      The X position of the image at animation end.
     *                                   Relative to the container or view that holds the
     *                                   ZoomAnimationImage.
     * @param  endY                      The Y position of the image at animation end.
     *                                   Relative to the container or view that holds the
     *                                   ZoomAnimationImage.
     * @param  endWidth                  The width of the image at animation end.
     * @param  endHeight                 The height of the image at animation end.
     * @param  duration                  The duration of the animation measured in ticks.
     * @param  zoomMode                  (Optional) The zoom mode that will be used during the
     *                                   animation. Default is #FIXED_LEFT_AND_TOP.
     * @param  xProgressionEquation      (Optional) The equation that describes the
     *                                   development of the X position during the animation.
     *                                   Default is EasingEquations::linearEaseNone.
     * @param  yProgressionEquation      (Optional) The equation that describes the
     *                                   development of the Y position during the animation.
     *                                   Default is EasingEquations::linearEaseNone.
     * @param  widthProgressionEquation  (Optional) The equation that describes the
     *                                   development of the width during the animation.
     *                                   Default is EasingEquations::linearEaseNone.
     * @param  heightProgressionEquation (Optional) The equation that describes the
     *                                   development of the height during the animation.
     *                                   Default is EasingEquations::linearEaseNone.
     */
    void startZoomAndMoveAnimation(int16_t endX, int16_t endY, int16_t endWidth, int16_t endHeight, uint16_t duration, ZoomMode zoomMode = FIXED_LEFT_AND_TOP, EasingEquation xProgressionEquation = &EasingEquations::linearEaseNone, EasingEquation yProgressionEquation = &EasingEquations::linearEaseNone, EasingEquation widthProgressionEquation = &EasingEquations::linearEaseNone, EasingEquation heightProgressionEquation = &EasingEquations::linearEaseNone);

    /** Cancel zoom animation. The image is left in the position and size it is currently at. */
    void cancelZoomAnimation();

    virtual void handleTickEvent();

    /**
     * Initializes the bitmap of the image to be used. The bitmaps should represent the same
     * image in the two needed static resolutions.
     *
     * @param  smallBitmap The image in the smallest resolution.
     * @param  largeBitmap The image in the largest resolution.
     *
     * @see getSmallBitmap, getLargeBitmap
     *
     * @note The size of the bitmaps do not in any way limit the size of the ZoomAnimationImage
     *       and it is possible to scale the image beyond the sizes of these bitmaps.
     */
    void setBitmaps(const Bitmap& smallBitmap, const Bitmap& largeBitmap);

    /**
     * Gets the small bitmap.
     *
     * @return the small bitmap.
     *
     * @see setBitmaps
     */
    Bitmap getSmallBitmap() const
    {
        return smallBmp;
    }

    /**
     * Gets the large bitmap.
     *
     * @return the large bitmap.
     *
     * @see setBitmaps
     */
    Bitmap getLargeBitmap() const
    {
        return largeBmp;
    }

    /**
     * @copydoc Drawable::setPosition
     *
     * @note ZoomAnimationWidget diverts from the normal behavior by automatically invalidating
     *       which causes a redraw.
     */
    virtual void setPosition(int16_t x, int16_t y, int16_t width, int16_t height);

    /**
     * @copydoc Drawable::setWidth
     *
     * @note ZoomAnimationWidget diverts from the normal behavior by automatically invalidating
     *       which causes a redraw.
     */
    virtual void setWidth(int16_t width);

    /**
     * @copydoc Drawable::setHeight
     *
     * @note ZoomAnimationWidget diverts from the normal behavior by automatically invalidating
     *       which causes a redraw.
     */
    virtual void setHeight(int16_t height);

    /**
     * Sets the width and height of the image.
     *
     * @param  width  The new width.
     * @param  height The new height.
     *
     * @note ZoomAnimationWidget diverts from the normal behavior by automatically invalidating
     *       which causes a redraw.
     */
    virtual void setDimension(int16_t width, int16_t height);

    /**
     * Sets the algorithm to be used. In short, there is currently a value for fast (nearest
     * neighbor) and a value for slow (bilinear interpolation). Default is
     * ScalableImage::NEAREST_NEIGHBOR since moving images do not need to be of the best
     * quality, until they stop moving. If the image moves only a little bit, or
     * moves/resizes slowly, consider using ScaleableImage::BILINEAR_INTERPOLATION.
     *
     * @param  mode The algorithm to use when rendering.
     *
     * @see ScalableImage::ScalingAlgorithm, getScalingMode
     */
    virtual void setScalingMode(ScalableImage::ScalingAlgorithm mode);

    /**
     * Gets the scaling algorithm of the ScalableImage.
     *
     * @return the scaling algorithm used.
     *
     * @see setScalingMode
     */
    virtual ScalableImage::ScalingAlgorithm getScalingMode();

    /**
     * @copydoc Image::setAlpha
     */
    virtual void setAlpha(uint8_t newAlpha);

    /**
     * @copydoc Image::getAlpha
     */
    virtual uint8_t getAlpha() const;

    /**
     * Sets a delay on animations done by the ZoomAnimationImage. Defaults to 0 which means
     * that the animation starts immediately.
     *
     * @param  delay The delay in ticks.
     *
     * @see getAnimationDelay
     */
    virtual void setAnimationDelay(uint16_t delay);

    /**
     * Gets the current animation delay.
     *
     * @return The current animation delay. Expressed in ticks.
     *
     * @see setAnimationDelay
     */
    virtual uint16_t getAnimationDelay() const;

    /**
     * Associates an action to be performed when the animation ends.
     *
     * @param  callback The callback to be executed. The callback will be given a reference
     *                  to the ZoomAnimationImage.
     *
     * @see GenericCallback
     */
    void setAnimationEndedCallback(GenericCallback<const ZoomAnimationImage&>& callback)
    {
        animationEndedAction = &callback;
    }

    ///@cond
    /**
     * Is there currently an animation running.
     *
     * @return true if there is an animation running.
     *
     * @deprecated Use ZoomAnimationImage::isZoomAnimationRunning().
     */
    TOUCHGFX_DEPRECATED(
        "Use ZoomAnimationImage::isZoomAnimationRunning().",
        bool isRunning());
    ///@endcond

    /**
     * Is there currently an animation running.
     *
     * @return true if there is an animation running.
     */
    bool isZoomAnimationRunning() const;

protected:
    /** Animation states. */
    enum States
    {
        ANIMATE_ZOOM,          ///< Zoom animation state
        ANIMATE_ZOOM_AND_MOVE, ///< Zoom and move animation state
        NO_ANIMATION           ///< No animation state
    };

    States currentState;                        ///< The current animation state
    uint32_t animationCounter;                  ///< The progress counter for the animation
    uint16_t zoomAnimationDelay;                ///< A delay that is applied before animation start. Expressed in ticks.
    Bitmap smallBmp;                            ///< The bitmap representing the small image
    Bitmap largeBmp;                            ///< The bitmap representing the large image
    Image image;                                ///< The image for displaying the bitmap when the width/height is equal one of the bitmaps
    ScalableImage scalableImage;                ///< The scalable image for displaying the bitmap when the width/height is not equal one of the bitmaps
    ZoomMode currentZoomMode;                   ///< The ZoomMode to use by the animation
    int16_t zoomAnimationStartWidth;            ///< Width of the zoom animation start
    int16_t zoomAnimationStartHeight;           ///< Height of the zoom animation start
    int16_t zoomAnimationEndWidth;              ///< Width of the zoom animation end
    int16_t zoomAnimationEndHeight;             ///< Height of the zoom animation end
    int16_t zoomAnimationStartX;                ///< The zoom animation start x coordinate
    int16_t zoomAnimationStartY;                ///< The zoom animation start y coordinate
    int16_t zoomAnimationDeltaX;                ///< The zoom animation delta x
    int16_t zoomAnimationDeltaY;                ///< The zoom animation delta y
    int16_t moveAnimationEndX;                  ///< The move animation end x coordinate
    int16_t moveAnimationEndY;                  ///< The move animation end y coordinate
    uint16_t animationDuration;                 ///< Duration of the animation
    EasingEquation zoomAnimationWidthEquation;  ///< The zoom animation width equation
    EasingEquation zoomAnimationHeightEquation; ///< The zoom animation height equation
    EasingEquation moveAnimationXEquation;      ///< The move animation x coordinate equation
    EasingEquation moveAnimationYEquation;      ///< The move animation y coordinate equation

    GenericCallback<const ZoomAnimationImage&>* animationEndedAction; ///< The animation ended action

    /**
     * Chooses the optimal rendering of the image given the current width and height. If the
     * dimensions match either the small or large bitmap, that will be used, otherwise the
     * large image will be scaled using the defined scaling mode.
     *
     * @see setScalingMode, setBitmaps
     */
    virtual void updateRenderingMethod();

    /**
     * Sets the current animation state and reset the animation counter.
     *
     * @param  state The new state.
     */
    virtual void setCurrentState(States state);

    /**
     * Starts timer and set parameters. Contains code shared between startZoomAnimation()
     * and startZoomAndMoveAnimation(). If both delay and duration is zero, the end position
     * and size is applied and the animation is ended immediately.
     *
     * @param  endWidth                  The end width.
     * @param  endHeight                 The end height.
     * @param  duration                  The duration.
     * @param  zoomMode                  The zoom mode.
     * @param  widthProgressionEquation  The width progression equation.
     * @param  heightProgressionEquation The height progression equation.
     */
    void startTimerAndSetParameters(int16_t endWidth, int16_t endHeight, uint16_t duration, ZoomMode zoomMode, EasingEquation widthProgressionEquation, EasingEquation heightProgressionEquation);

    /**
     * Calculates the change in X and Y caused by the zoom animation given the current
     * #ZoomMode.
     */
    virtual void updateZoomAnimationDeltaXY();
};

} // namespace touchgfx

#endif // ZOOMANIMATIONIMAGE_HPP
