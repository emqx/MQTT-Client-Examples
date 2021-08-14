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
 * @file touchgfx/TextProvider.hpp
 *
 * Declares the touchgfx::TextProvider class.
 */
#ifndef TEXTPROVIDER_HPP
#define TEXTPROVIDER_HPP

#include <stdarg.h>
#include <touchgfx/Font.hpp>
#include <touchgfx/Unicode.hpp>

namespace touchgfx
{
/**
 * The TextProvider is used in drawing basic strings and strings with one or two wildcards. The
 * TextProvider enables wildcard expansion of the string at the time it is written to
 * the LCD.
 *
 * Wildcards specified as &lt;placeholder&gt; are converted to Unicode value 2 by the
 * text converter tool, and the placeholders are automatically expanded with the
 * specified wildcard buffers at runtime.
 */
class TextProvider
{
public:
    static const uint32_t MAX_32BIT_INTEGER_DIGITS = 33U; ///< Max number of digits used for the text representation of a 32 bit integer.

    /**
     * Initializes a new instance of the TextProvider class.
     *
     * @note The user must call initialize() before characters can be provided.
     */
    TextProvider();

    /**
     * Initializes the TextProvider. Each '\2' character in the format is replaced by one
     * UnicodeChar* argument from pArg.
     *
     * @param  stringFormat The string to format.
     * @param  pArg         Format arguments in the form of a va_list.
     * @param  gsubTable    (Optional) Pointer to GSUB table with Unicode substitution rules.
     */
    void initialize(const Unicode::UnicodeChar* stringFormat, va_list pArg, const uint16_t* gsubTable = 0);

    /**
     * Initializes the TextProvider. Each '\2' character in the format is replaced by one
     * UnicodeChar* argument from pArg.
     *
     * @param  stringFormat The string to format.
     * @param  gsubTable    (Optional) Pointer to GSUB table with Unicode substitution rules.
     * @param  ...          Variable arguments providing additional information.
     */
    void initialize(const Unicode::UnicodeChar* stringFormat, const uint16_t* gsubTable = 0, ...);

    /**
     * Gets the next character. For Arabic and Thai, it is important to use the
     * getNextLigature instead.
     *
     * @return The next character of the expanded string or 0 if end of string is reached.
     *
     * @see TextProvider::getNextLigature
     */
    Unicode::UnicodeChar getNextChar();

    /**
     * Tells if the end of the string has been reached.
     *
     * @return True if the end of the string has been reached, false if not.
     *
     * @see TextProvider::getNextLigature()
     */
    bool endOfString();

    /**
     * Gets the next ligature. For most languages this is simply the next Unicode character
     * from the buffer, but e.g. Arabic has different ligatures for each character. Thai
     * character placement might also depend on previous characters. It is recommended to
     * use getNextLigature with font and glyph parameters to ensure coming glyphs in a text
     * are placed correctly.
     *
     * @param  direction The direction.
     *
     * @return The next character of the expanded string or 0 if end of string is reached.
     *
     * @see TextProvider::getNextChar
     *
     * @note Functions getNextLigature() and getNextChar() will advance through the same buffer
     *       and mixing the use of those functions is not recommended and may cause
     *       undesired results. Instead create two TextProviders and user getNextChar() on
     *       one and getNextLigature() on the other.
     */
    Unicode::UnicodeChar getNextLigature(TextDirection direction);

    /**
     * Gets the next ligature. For most languages this is simply the next Unicode character
     * from the buffer, but e.g. Arabic has different ligatures for each character.
     *
     * Also gets a glyph for the ligature in a font. For non-Thai Unicodes, this is
     * identical to using Font::getGlyph(), but for Thai characters where diacritics glyphs
     * are not always placed at the same relative position, an adjusted GlyphNode will be
     * generated with correct relative X/Y coordinates.
     *
     * @param       direction The direction.
     * @param       font      The font.
     * @param [out] glyph     The glyph.
     *
     * @return The next character of the expanded string or 0 if end of string i reached.
     *
     * @see TextProvider::getNextChar, Font::getGlyph
     *
     * @note Functions getNextLigature() and getNextChar() will advance through the same buffer
     *       and mixing the use of those functions is not recommended and may cause
     *       undesired results. Instead create two TextProviders and user getNextChar() on
     *       one and getNextLigature() on the other.
     */
    Unicode::UnicodeChar getNextLigature(TextDirection direction, const Font* font, const GlyphNode*& glyph);

    /**
     * Gets the next ligature. For most languages this is simply the next Unicode character
     * from the buffer, but e.g. Arabic has different ligatures for each character.
     *
     * Also gets a glyph for the ligature in a font. For non-Thai Unicodes, this is
     * identical to using Font::getGlyph(), but for Thai characters where diacritics glyphs
     * are not always placed at the same relative position, an adjusted GlyphNode will be
     * generated with correct relative X/Y coordinates.
     *
     * Furthermore a pointer to the glyph data and the bit depth of the font are returned in
     * parameters.
     *
     * @param       direction    The direction.
     * @param       font         The font.
     * @param [out] glyph        The glyph.
     * @param [out] pixelData    Information describing the pixel.
     * @param [out] bitsPerPixel The bits per pixel.
     *
     * @return The next character of the expanded string or 0 if end of string is reached.
     *
     * @see TextProvider::getNextChar, Font::getGlyph
     *
     * @note Functions getNextLigature() and getNextChar() will advance through the same buffer
     *       and mixing the use of those functions is not recommended and may cause
     *       undesired results. Instead create two TextProviders and user getNextChar() on
     *       one and getNextLigature() on the other.
     */
    Unicode::UnicodeChar getNextLigature(TextDirection direction, const Font* font, const GlyphNode*& glyph, const uint8_t*& pixelData, uint8_t& bitsPerPixel);

private:
    Unicode::UnicodeChar getNextCharInternal();
    const Unicode::UnicodeChar* original_format_string;
    const Unicode::UnicodeChar* format;
    va_list formatArgs;
    const Unicode::UnicodeChar* substringPointer;
    bool isWritingWildcard;
    template <unsigned int size>
    class circularBuffer
    {
    public:
        circularBuffer()
            : pos(0), used(0)
        {
        }
        FORCE_INLINE_FUNCTION void flush()
        {
            used = 0;
        }
        FORCE_INLINE_FUNCTION bool isEmpty() const
        {
            return used == 0;
        }
        FORCE_INLINE_FUNCTION bool isFull() const
        {
            return used == size;
        }
        FORCE_INLINE_FUNCTION Unicode::UnicodeChar peekChar()
        {
            assert(used > 0);
            return buffer[pos];
        }
        FORCE_INLINE_FUNCTION Unicode::UnicodeChar peekChar(uint16_t offset)
        {
            assert(offset < used);
            const uint16_t index = pos + offset;
            return buffer[index < size ? index : index - size];
        }
        FORCE_INLINE_FUNCTION void dropFront(uint16_t num = 1)
        {
            assert(used >= num);
            used -= num;
            pos += num;
            if (pos >= size)
            {
                pos -= size;
            }
        }
        Unicode::UnicodeChar popFront()
        {
            assert(used > 0);
            const Unicode::UnicodeChar ch = buffer[pos];
            used--;
            pos++;
            if (pos >= size)
            {
                pos -= size;
            }
            return ch;
        }
        Unicode::UnicodeChar popBack()
        {
            assert(used > 0);
            return peekChar(used-- - 1);
        }
        void allocateFront(uint16_t num)
        {
            assert(used + num <= size);
            used += num;
            if (pos < num)
            {
                pos += size;
            }
            pos -= num;
        }
        void pushFrontForce(Unicode::UnicodeChar newChar)
        {
            // "use" one more entry, if already full overwrite back entry ("used" is unchanged)
            if (used < size)
            {
                used++;
            }
            // Move "pos" one back with overflow check
            if (pos == 0)
            {
                pos += size;
            }
            pos--;
            replaceAt0(newChar);
        }
        void pushFront(Unicode::UnicodeChar newChar)
        {
            allocateFront(1);
            replaceAt0(newChar);
        }
        FORCE_INLINE_FUNCTION void pushBack(Unicode::UnicodeChar newChar)
        {
            assert(used < size);
            replaceAt(++used - 1, newChar);
        }
        FORCE_INLINE_FUNCTION void replaceAt0(Unicode::UnicodeChar newChar)
        {
            buffer[pos] = newChar;
        }
        FORCE_INLINE_FUNCTION void replaceAt1(Unicode::UnicodeChar newChar)
        {
            assert(used > 1);
            const uint16_t index = pos + 1;
            buffer[index < size ? index : 0] = newChar;
        }
        FORCE_INLINE_FUNCTION void replaceAt(uint16_t offset, Unicode::UnicodeChar newChar)
        {
            assert(used > offset);
            const uint16_t index = pos + offset;
            buffer[index < size ? index : index - size] = newChar;
        }

    private:
        Unicode::UnicodeChar buffer[size];
        uint16_t pos;
        uint16_t used;
    };
    static const int NUM_PREV_CHARS = 2;
    static const int NUM_NEXT_CHARS = 10; // input + lookahead + delta(substitution)
    static const int NUM_XTRA_CHARS = 2;
    circularBuffer<NUM_PREV_CHARS> prevCharacters;
    circularBuffer<NUM_NEXT_CHARS> nextCharacters;
    circularBuffer<NUM_XTRA_CHARS> xtraCharacters; // In case we insert
    void replaceInputCharacters(uint16_t existingNumChars, uint16_t newNumChars, const Unicode::UnicodeChar* newChars);
    void fillInputBuffer();
    const uint16_t* fontGsubTable;
    void substituteGlyphs();
    uint16_t gsubTableBinarySearch(const uint16_t numEntries, const uint16_t* unicodeLookupTable, const Unicode::UnicodeChar key) const;
    bool applyGsubRules(const uint16_t* nextTableEntry, const Unicode::UnicodeChar key);
    bool gsubRuleMatch(const uint16_t* tableEntry, uint16_t backtrack, uint16_t input, uint16_t lookahead);

    void initializeInternal();
    void unicodeConverterInit();
    Unicode::UnicodeChar unicodeConverter(const TextDirection direction);

    const Unicode::UnicodeChar* binarySearch(uint16_t key, const Unicode::UnicodeChar contextualFormTable[][5], int maxIndex) const;
    FORCE_INLINE_FUNCTION const Unicode::UnicodeChar* contextualFormForChar(const Unicode::UnicodeChar currChar) const;

    FORCE_INLINE_FUNCTION void adjustGlyph(Unicode::UnicodeChar originalCharacter, Unicode::UnicodeChar currentCharacter, const GlyphNode*& glyph, const Font* font);
    const GlyphNode* adjustHindiGlyph(const GlyphNode* glyph);
    const GlyphNode* thaiLookupGlyph(const GlyphNode* glyph, const Font* font, Unicode::UnicodeChar unicode) const;
    const GlyphNode* adjustThaiGlyph(const Font* font, const GlyphNode* glyph);
    const GlyphNode* adjustArabicGlyph(const Font* font, const GlyphNode* glyph, Unicode::UnicodeChar originalUnicode);
    GlyphNode modifiedGlyph;
    int16_t glyphPosTop;
    int16_t glyphPosBottom;
    int16_t glyphPosLeft;

    bool isContextualBeginning;
    bool lastGlyphIsAccent;
    static const Unicode::UnicodeChar contextualForms4Long[][5];
    static const Unicode::UnicodeChar contextualForms3Long[][5];
    static const Unicode::UnicodeChar contextualForms2Long[][5];
    static const Unicode::UnicodeChar contextualForms0621_063a[][4];
    static const Unicode::UnicodeChar contextualForms0641_064a[][4];
    static const Unicode::UnicodeChar contextualForms06XX[][5];
};

} // namespace touchgfx

#endif // TEXTPROVIDER_HPP
