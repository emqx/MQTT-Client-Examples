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
 * @file touchgfx/TypedText.hpp
 *
 * Declares the touchgfx::TypedText class.
 */
#ifndef TYPEDTEXT_HPP
#define TYPEDTEXT_HPP

#include <cassert>
#include <touchgfx/Font.hpp>
#include <touchgfx/Texts.hpp>
#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
const TypedTextId TYPED_TEXT_INVALID = 0xFFFFU; ///< The ID of an invalid text

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION < 6000000)
// Keil5 compiler issues irrelevant warning relating to missing ctor initialization for TypedTextData.
#pragma diag_suppress 368
#endif

/**
 * TypedText represents text (for characters) and typography (for font and alignment).
 * TypedText provides methods for adjusting the text, font and alignment.
 *
 * @see TextArea
 */
class TypedText
{
public:
    /**
     * The data structure for typed texts.
     */
    struct TypedTextData
    {
        const unsigned char fontIdx;       ///< The ID of the font associated with the typed text
        const Alignment alignment : 2;     ///< The alignment of the typed text (LEFT,CENTER,RIGHT)
        const TextDirection direction : 2; ///< The text direction (LTR,RTL,...) of the typed text
    };

    /**
     * Initializes a new instance of the TypedText class.
     *
     * @param  id (Optional) The identifier.
     */
    explicit TypedText(const TypedTextId id = TYPED_TEXT_INVALID)
        : typedTextId(id)
    {
    }

    /** Finalizes an instance of the TypedText class. */
    virtual ~TypedText()
    {
    }

    /**
     * Gets the id of the typed text.
     *
     * @return The id.
     */
    FORCE_INLINE_FUNCTION TypedTextId getId() const
    {
        return typedTextId;
    }

    /**
     * Has the TypedText been set to a proper value?
     *
     * @return Is the id valid.
     */
    FORCE_INLINE_FUNCTION bool hasValidId() const
    {
        return typedTextId != TYPED_TEXT_INVALID;
    }

    /**
     * Gets the text associated with this TypedText.
     *
     * @return The text.
     */
    FORCE_INLINE_FUNCTION const Unicode::UnicodeChar* getText() const
    {
        assertValid();
        return texts->getText(typedTextId);
    }

    /**
     * Gets the font associated with this TypedText.
     *
     * @return The font.
     */
    FORCE_INLINE_FUNCTION const Font* getFont() const
    {
        assertValid();
        return fonts[typedTexts[typedTextId].fontIdx];
    }

    /**
     * Gets the font ID associated with this TypedText.
     *
     * @return The font.
     */
    FORCE_INLINE_FUNCTION FontId getFontId() const
    {
        assertValid();
        return typedTexts[typedTextId].fontIdx;
    }

    /**
     * Gets the alignment associated with this TypedText.
     *
     * @return The alignment.
     */
    FORCE_INLINE_FUNCTION Alignment getAlignment() const
    {
        assertValid();
        return typedTexts[typedTextId].alignment;
    }

    /**
     * Gets the text direction associated with this TypedText.
     *
     * @return The alignment.
     */
    FORCE_INLINE_FUNCTION TextDirection getTextDirection() const
    {
        assertValid();
        return typedTexts[typedTextId].direction;
    }

    /**
     * Registers an array of typed texts. All typed text instances are bound to this
     * database. This function is called automatically when setting a new language. Use
     * Texts::setLanguage() instead of calling this function directly.
     *
     * @param  data A reference to the TypedTextData storage array.
     * @param  f    The fonts associated with the array.
     * @param  n    The number of typed texts in the array.
     */
    static void registerTypedTextDatabase(const TypedTextData* data, const Font* const* f, const uint16_t n)
    {
        typedTexts = data;
        fonts = f;
        numberOfTypedTexts = n;
    }

    /**
     * Registers an array of texts. This function is called automatically from
     * touchgfx_generic_init(). Should not be called under normal circumstances.
     *
     * @param  t The array of texts.
     */
    static void registerTexts(const Texts* t)
    {
        texts = t;
    }

private:
    FORCE_INLINE_FUNCTION void assertValid() const
    {
        assert(typedTexts != 0 && "TypedText database has not been initialized.");
        assert(typedTextId < numberOfTypedTexts && "typedTextId larger than numberOfTypedTexts.");
    }

    TypedTextId typedTextId;

    static const TypedTextData* typedTexts;
    static const Texts* texts;
    static const Font* const* fonts;
    static uint16_t numberOfTypedTexts;
};

} // namespace touchgfx

#endif // TYPEDTEXT_HPP
