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
 * @file touchgfx/containers/progress_indicators/TextProgress.hpp
 *
 * Declares the touchgfx::TextProgress class.
 */
#ifndef TEXTPROGRESS_HPP
#define TEXTPROGRESS_HPP

#include <touchgfx/containers/progress_indicators/AbstractProgressIndicator.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>

namespace touchgfx
{
/**
 * A text progress will display progress as a number with a given number of decimals.
 *
 * @note The implementation does not use floating point variables to calculate the progress.
 */
class TextProgress : public AbstractProgressIndicator
{
public:
    TextProgress();

    /**
     * Sets the position and dimensions of the text progress indicator.
     *
     * Sets the position and dimensions of the text progress indicator relative to the
     * background image.
     *
     * @param  x      The x coordinate.
     * @param  y      The y coordinate.
     * @param  width  The width of the text progress indicator.
     * @param  height The height of the text progress indicator.
     */
    virtual void setProgressIndicatorPosition(int16_t x, int16_t y, int16_t width, int16_t height);

    /**
     * Sets the typed text. The text should have exactly one wildcard and could for example
     * look like this: "&lt;progress&gt;\%".
     *
     * @param  t The TypedText to process.
     *
     * @see getTypedText
     */
    virtual void setTypedText(const TypedText& t);

    /**
     * Gets the typed text.
     *
     * @return The typed text.
     *
     * @see setTypedText
     */
    virtual TypedText getTypedText() const;

    /**
     * Sets the color of the text in the used text area.
     *
     * @param  color The color.
     *
     * @see getColor, TextArea::setColor
     */
    virtual void setColor(colortype color);

    /**
     * Gets the color of the text in the used text area.
     *
     * @return The color.
     */
    virtual colortype getColor() const;

    /**
     * Sets the alpha of the text area.
     *
     * @param  alpha The alpha.
     *
     * @see getAlpha, TextArea::setAlpha
     */
    virtual void setAlpha(uint8_t alpha);

    /**
     * Gets the alpha of the text area.
     *
     * @return The alpha.
     *
     * @see setAlpha, TextArea::getAlpha
     */
    virtual uint8_t getAlpha() const;

    /**
     * Sets the new value for the progress indicator.
     *
     * @param  value The value.
     */
    virtual void setValue(int value);

    /**
     * Sets number of decimals when displaying progress.
     *
     * @param  numberOfDecimals Number of decimals. Only up to two decimals is supported.
     *
     * @see getNumberOfDecimals
     */
    virtual void setNumberOfDecimals(uint16_t numberOfDecimals);

    /**
     * Gets number of decimals.
     *
     * @return The number of decimals.
     *
     * @see setNumberOfDecimals
     */
    virtual uint16_t getNumberOfDecimals() const;

protected:
    TextAreaWithOneWildcard textArea;   ///< The text area
    Unicode::UnicodeChar textBuffer[9]; ///< Room for 100.0000
    uint16_t decimals;                  ///< The number of decimals
};

} // namespace touchgfx

#endif // TEXTPROGRESS_HPP
