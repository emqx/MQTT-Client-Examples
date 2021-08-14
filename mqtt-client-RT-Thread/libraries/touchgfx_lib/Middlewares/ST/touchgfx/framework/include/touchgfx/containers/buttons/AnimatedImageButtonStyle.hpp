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
 * @file touchgfx/containers/buttons/AnimatedImageButtonStyle.hpp
 *
 * Declares the touchgfx::AnimatedImageButtonStyle class.
 */
#ifndef ANIMATEDIMAGEBUTTONSTYLE_HPP
#define ANIMATEDIMAGEBUTTONSTYLE_HPP

#include <touchgfx/widgets/AnimatedImage.hpp>

namespace touchgfx
{
/**
 * An animated image button style.  An animated image button style. This class is supposed to be
 * used with one of the ButtonTrigger classes to create a functional button. This class
 * will show the first or last image of an animated image depending on the state of the
 * button (pressed or released). When the state changes the button will show the
 * sequence of images in forward or reversed order.
 *
 * The AnimatedImageButtonStyle will set the size of the enclosing container (normally
 * AbstractButtonContainer) to the size of the first Bitmap. This can be overridden by
 * calling setWidth/setHeight after setting the bitmaps.
 *
 * The position of the bitmap can be adjusted with setBitmapXY (default is upper left
 * corner).
 *
 * @tparam T Generic type parameter. Typically a AbstractButtonContainer subclass.
 *
 * @see AbstractButtonContainer
 */
template <class T>
class AnimatedImageButtonStyle : public T
{
public:
    AnimatedImageButtonStyle()
        : T(), buttonAnimatedImage()
    {
        buttonAnimatedImage.setXY(0, 0);
        T::add(buttonAnimatedImage);
    }

    /**
     * Sets the bitmaps.
     *
     * @param  bmpStart The bitmap start.
     * @param  bmpEnd   The bitmap end.
     */
    void setBitmaps(const Bitmap& bmpStart, const Bitmap& bmpEnd)
    {
        buttonAnimatedImage.setBitmaps(bmpStart.getId(), bmpEnd.getId());

        AbstractButtonContainer::setWidth(bmpStart.getWidth());
        AbstractButtonContainer::setHeight(bmpStart.getHeight());

        handlePressedUpdated();
    }

    /**
     * Sets bitmap x and y.
     *
     * @param  x An uint16_t to process.
     * @param  y An uint16_t to process.
     */
    void setBitmapXY(uint16_t x, uint16_t y)
    {
        buttonAnimatedImage.setXY(x, y);
    }

    /**
     * Sets update ticks interval.
     *
     * @param  updateInterval The update interval.
     */
    void setUpdateTicksInterval(uint8_t updateInterval)
    {
        buttonAnimatedImage.setUpdateTicksInterval(updateInterval);
    }

protected:
    AnimatedImage buttonAnimatedImage; ///< The button animated image

    /** @copydoc AbstractButtonContainer::handlePressedUpdated() */
    virtual void handlePressedUpdated()
    {
        buttonAnimatedImage.startAnimation(AbstractButtonContainer::pressed, true, false);
        T::handlePressedUpdated();
    }

    /** @copydoc AbstractButtonContainer::handleAlphaUpdated() */
    virtual void handleAlphaUpdated()
    {
        buttonAnimatedImage.setAlpha(T::getAlpha());
        T::handleAlphaUpdated();
    }
};

} // namespace touchgfx

#endif // ANIMATEDIMAGEBUTTONSTYLE_HPP
