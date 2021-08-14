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
 * @file touchgfx/widgets/TextAreaWithWildcard.hpp
 *
 * Declares the touchgfx::TextAreaWithOneWildcard and touchgfx::TextAreaWithTwoWildcards classes.
 */
#ifndef TEXTAREAWITHWILDCARD_HPP
#define TEXTAREAWITHWILDCARD_HPP

#include <touchgfx/TextProvider.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/widgets/TextArea.hpp>

namespace touchgfx
{
/**
 * TextArea with one wildcard. The format string (i.e. the TypedText set in setTypedText()) is
 * expected to contain a wildcard &lt;placeholder> from the text.
 *
 * @note the text converter tool converts the <...> to ascii value 2 which is then being
 *       replaced by a wildcard text.
 */
class TextAreaWithOneWildcard : public TextArea
{
public:
    TextAreaWithOneWildcard()
        : TextArea(), wildcard(0)
    {
    }

    virtual int16_t getTextHeight()
    {
        return typedText.hasValidId() ? calculateTextHeight(typedText.getText(), wildcard, 0) : 0;
    }

    virtual void draw(const Rect& area) const;

    /**
     * Sets the wildcard used in the TypedText where &lt;placeholder> is placed. Wildcard
     * string must be a null-terminated UnicodeChar array.
     *
     * @param  value A pointer to the UnicodeChar to set the wildcard to.
     *
     * @note The pointer passed is saved, and must be accessible whenever TextAreaWithOneWildcard
     *       may need it.
     */
    void setWildcard(const Unicode::UnicodeChar* value)
    {
        wildcard = value;
    }

    /**
     * Gets the wildcard used in the TypedText as previously set using setWildcard().
     *
     * @return The wildcard used in the text.
     */
    const Unicode::UnicodeChar* getWildcard() const
    {
        return wildcard;
    }

    virtual uint16_t getTextWidth() const
    {
        return typedText.hasValidId() ? typedText.getFont()->getStringWidth(typedText.getTextDirection(), typedText.getText(), wildcard, 0) : 0;
    }

protected:
    const Unicode::UnicodeChar* wildcard; ///< Pointer to the wildcard string. Must be null-terminated.
};

/**
 * TextArea with two wildcards. The format string (i.e. the TypedText set in setTypedText()) is
 * expected to contain two wildcards &lt;placeholders> from the text.
 *
 *
 * @note the text converter tool converts the <...> to ascii value 2 which is what is
 *       being replaced by a wildcard text.
 */
class TextAreaWithTwoWildcards : public TextArea
{
public:
    TextAreaWithTwoWildcards()
        : TextArea(), wc1(0), wc2(0)
    {
    }

    virtual int16_t getTextHeight()
    {
        return typedText.hasValidId() ? calculateTextHeight(typedText.getText(), wc1, wc2) : 0;
    }

    virtual void draw(const Rect& area) const;

    /**
     * Sets the wildcard used in the TypedText where first &lt;placeholder> is placed.
     * Wildcard string must be a null-terminated UnicodeChar array.
     *
     * @param  value A pointer to the UnicodeChar to set the wildcard to.
     *
     * @note The pointer passed is saved, and must be accessible whenever TextAreaWithTwoWildcard
     *       may need it.
     */
    void setWildcard1(const Unicode::UnicodeChar* value)
    {
        wc1 = value;
    }

    /**
     * Gets the first wildcard used in the TypedText as previously set using setWildcard1().
     *
     * @return The first wildcard used in the text.
     */
    const Unicode::UnicodeChar* getWildcard1() const
    {
        return wc1;
    }

    /**
     * Sets the wildcard used in the TypedText where second &lt;placeholder> is placed.
     * Wildcard string must be a null-terminated UnicodeChar array.
     *
     * @param  value A pointer to the UnicodeChar to set the wildcard to.
     *
     * @note The pointer passed is saved, and must be accessible whenever TextAreaWithTwoWildcard
     *       may need it.
     */
    void setWildcard2(const Unicode::UnicodeChar* value)
    {
        wc2 = value;
    }

    /**
     * Gets the second wildcard used in the TypedText as previously set using setWildcard1().
     *
     * @return The second wildcard used in the text.
     */
    const Unicode::UnicodeChar* getWildcard2() const
    {
        return wc2;
    }

    virtual uint16_t getTextWidth() const
    {
        return typedText.hasValidId() ? typedText.getFont()->getStringWidth(typedText.getTextDirection(), typedText.getText(), wc1, wc2) : 0;
    }

protected:
    const Unicode::UnicodeChar* wc1; ///< Pointer to the first wildcard string. Must be null-terminated.
    const Unicode::UnicodeChar* wc2; ///< Pointer to the second wildcard string. Must be null-terminated.
};

} // namespace touchgfx

#endif // TEXTAREAWITHWILDCARD_HPP
