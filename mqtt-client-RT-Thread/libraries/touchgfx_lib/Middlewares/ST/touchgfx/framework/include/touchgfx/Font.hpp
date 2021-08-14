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
 * @file touchgfx/Font.hpp
 *
 * Declares the touchgfx::Font class.
 */
#ifndef FONT_HPP
#define FONT_HPP

#include <touchgfx/Unicode.hpp>
#include <touchgfx/hal/Types.hpp>

namespace touchgfx
{
/** Glyph flag definitions. */
enum GlyphFlags
{
    GLYPH_DATA_KERNINGTABLEPOS_BIT8_10 = 0x07, ///< The 8th, 9th and 10th bit of the kerningTablePos
    GLYPH_DATA_WIDTH_BIT8 = 0x08,              ///< The 9th bit of "width"
    GLYPH_DATA_HEIGHT_BIT8 = 0x10,             ///< The 9th bit of "height"
    GLYPH_DATA_TOP_BIT8 = 0x20,                ///< The 9th bit of "top"
    GLYPH_DATA_TOP_BIT9 = 0x40,                ///< The sign bit of "top"
    GLYPH_DATA_ADVANCE_BIT8 = 0x80             ///< The 9th bit of "advance"
};

#pragma pack(2)

/** struct providing information about a glyph. Used by LCD when rendering. */
typedef struct GlyphNode
{
    uint32_t dataOffset;          ///< The index to the data of this glyph
    Unicode::UnicodeChar unicode; ///< The Unicode of this glyph.
    uint8_t _width;               ///< Width of the actual glyph data
    uint8_t _height;              ///< Height of the actual glyph data
    uint8_t _top;                 ///< Vertical offset from baseline of the glyph
    int8_t left;                  ///< Horizontal offset from the left of the glyph
    uint8_t _advance;             ///< Width of the glyph (including space to the left and right)
    uint8_t _kerningTablePos;     ///< Where are the kerning information for this glyph stored in the kerning table
    uint8_t kerningTableSize;     ///< How many entries are there in the kerning table (following kerningTablePos) for this glyph
    uint8_t flags;                ///< Additional glyph flags (font encoding and extra precision for width/height/top/advance)

    /**
     * Gets the "kerningTablePos" value where the 8th and 9th bits are stored in flags.
     *
     * @return the right value of "kerningTablePos".
     */
    FORCE_INLINE_FUNCTION uint16_t kerningTablePos() const
    {
        return ((flags & GLYPH_DATA_KERNINGTABLEPOS_BIT8_10) << 8) | _kerningTablePos;
    }

    /**
     * Gets the "width" value where the 9th bit is stored in flags.
     *
     * @return the right value of "width".
     */
    FORCE_INLINE_FUNCTION uint16_t width() const
    {
        return ((flags & GLYPH_DATA_WIDTH_BIT8) << 5) | _width;
    }

    /**
     * Gets the "height" value where the 9th bit is stored in flags.
     *
     * @return the right value of "height".
     */
    FORCE_INLINE_FUNCTION uint16_t height() const
    {
        return ((flags & GLYPH_DATA_HEIGHT_BIT8) << 4) | _height;
    }

    /**
     * Gets the "top" value where the 9th bit and the sign bit are stored in flags.
     *
     * @return the right value of "top".
     */
    FORCE_INLINE_FUNCTION int16_t top() const
    {
        int16_t num = ((flags & GLYPH_DATA_TOP_BIT8) << 3) | _top;
        return (flags & GLYPH_DATA_TOP_BIT9) ? num - 512 : num;
    }

    /**
     * Sets a new value for top. Used to adjust the vertical position of a glyph - this is
     * used when positioning some Thai glyphs and some Arabic glyphs.
     *
     * @param  newTop The new top.
     */
    FORCE_INLINE_FUNCTION void setTop(int16_t newTop)
    {
        _top = newTop & 0xFF;
        flags &= ~(GLYPH_DATA_TOP_BIT8 | GLYPH_DATA_TOP_BIT9);
        flags |= (newTop & 0x300) >> 3;
    }

    /**
     * Gets the "advance" value where the 9th bit is stored in flags.
     *
     * @return the right value of "advance".
     */
    FORCE_INLINE_FUNCTION uint16_t advance() const
    {
        return ((flags & GLYPH_DATA_ADVANCE_BIT8) << 1) | _advance;
    }
} GlyphNode;
#pragma pack()

#pragma pack(2)

/**
 * Structure providing information about a kerning for a given pair of characters. Used by LCD
 * when rendering, calculating text width etc.
 */
typedef struct
{
    Unicode::UnicodeChar unicodePrevChar; ///< The Unicode for the first character in the kerning pair
    int8_t distance;                      ///< The kerning distance
} KerningNode;
#pragma pack()

/** Defines an alias representing a Font ID. */
typedef uint16_t FontId;

/**
 * The font base class. This class is abstract and requires the implementation of getGlyph. It
 * provides utility functions such as obtaining string width and font height.
 */
class Font
{
public:
    /** Finalizes an instance of the Font class. */
    virtual ~Font()
    {
    }

    /**
     * Gets the glyph data associated with the specified Unicode. Please note that in case
     * of Thai letters and Arabic letters where diacritics can be placed relative to the
     * previous character(s), please use TextProvider::getNextLigature() instead as it will
     * create a temporary GlyphNode that will be adjusted with respect to X/Y position.
     *
     * @param       unicode      The character to look up.
     * @param       pixelData    Pointer to the pixel data for the glyph if the glyph is
     *                           found. This is set by this method.
     * @param [out] bitsPerPixel Reference where to place the number of bits per pixel.
     *
     * @return A pointer to the glyph node or null if the glyph was not found.
     */
    virtual const GlyphNode* getGlyph(Unicode::UnicodeChar unicode, const uint8_t*& pixelData, uint8_t& bitsPerPixel) const = 0;

    /**
     * Gets the glyph data associated with the specified Unicode. Please note that in case
     * of Thai letters and Arabic letters where diacritics can be placed relative to the
     * previous character(s), please use TextProvider::getNextLigature() instead as it will
     * create a temporary GlyphNode that will be adjusted with respect to X/Y position.
     *
     * @param  unicode The character to look up.
     *
     * @return A pointer to the glyph node or null if the glyph was not found.
     *
     * @see TextProvider::getNextLigature
     */
    virtual const GlyphNode* getGlyph(Unicode::UnicodeChar unicode) const
    {
        if (unicode == 0)
        {
            return 0;
        }
        const uint8_t* dummyPixelDataPointer = 0;
        uint8_t bitsPerPixelDummy = 0;
        const GlyphNode* glyph = getGlyph(unicode, dummyPixelDataPointer, bitsPerPixelDummy);
        return glyph;
    }

    /**
     * Gets fallback character for the given font. The fallback character is the character
     * used when no glyph is available for some character. If 0 (zero) is returned, there is
     * no default character.
     *
     * @return The default character for the typography in case no glyph is available.
     */
    virtual Unicode::UnicodeChar getFallbackChar() const
    {
        return fallbackCharacter;
    }

    /**
     * Gets ellipsis character for the given font. This is the character which is used when
     * truncating long lines.
     *
     * @return The ellipsis character for the typography.
     *
     * @see TextArea::setWideTextAction
     */
    virtual Unicode::UnicodeChar getEllipsisChar() const
    {
        return ellipsisCharacter;
    }

    /**
     * Gets the width in pixels of the specified string. If the string contains multiple
     * lines, the width of the widest line is found. Please note that the correct number of
     * arguments must be given if the text contains wildcards.
     *
     * It is recommended to use the getStringWidth() implementation with the TextDirection
     * parameter to ensure correct calculation of the width. Kerning could result in
     * different results depending on the TextDirection. This method assumes TextDirection
     * to be TEXT_DIRECTION_LTR.
     *
     * @param  text A null-terminated Unicode string with arguments to insert if the text
     *              contains wildcards.
     * @param  ...  Variable arguments providing additional information inserted at wildcard
     *              placeholders.
     *
     * @return The width in pixels of the longest line of the specified string.
     */
    virtual uint16_t getStringWidth(const Unicode::UnicodeChar* text, ...) const;

    /**
     * Gets the width in pixels of the specified string. If the string contains multiple
     * lines, the width of the widest line is found. Please note that the correct number of
     * arguments must be given if the text contains wildcards.
     *
     * The TextDirection should be set correctly for the text supplied. For example the
     * string "10 20 30" will be calculated differently depending on the TextDirection. If
     * TextDirection is TEXT_DIRECTION_LTR the width is calculated as the with of "10
     * 20 30" (with kerning between all characters) but for TEXT_DIRECTION_RTL it is
     * calculated as "10"+" "+"20"+" "+"30" (with kerning only between characters in the
     * substrings and not between substrings). For most fonts there might not be a
     * difference between the two calculations, but some fonts might cause slightly
     * different results.
     *
     * @param  textDirection The text direction.
     * @param  text          A null-terminated Unicode string with arguments to insert if the text
     *                       contains wildcards.
     * @param  ...           Variable arguments providing additional information inserted at
     *                       wildcard placeholders.
     *
     * @return The width in pixels of the longest line of the specified string.
     */
    virtual uint16_t getStringWidth(TextDirection textDirection, const Unicode::UnicodeChar* text, ...) const;

    /**
     * Gets the width in pixels of the specified character.
     *
     * @param  c The Unicode character.
     *
     * @return The width in pixels of the specified character.
     */
    virtual uint16_t getCharWidth(const Unicode::UnicodeChar c) const;

    /**
     * Gets the number of blank pixels at the top of the given text.
     *
     * @param  text A null-terminated Unicode string.
     * @param  ...  Variable arguments providing additional information inserted at wildcard
     *              placeholders.
     *
     * @return The number of blank pixels above the text.
     */
    virtual uint8_t getSpacingAbove(const Unicode::UnicodeChar* text, ...) const;

    /**
     * Gets the height of the highest character in a given string. The height includes the
     * spacing above the text which is included in the font.
     *
     * @param  text A null-terminated Unicode string.
     * @param  ...  Variable arguments providing additional information inserted at wildcard
     *              placeholders.
     *
     * @return The height if the given text.
     */
    virtual uint16_t getMaxTextHeight(const Unicode::UnicodeChar* text, ...) const;

    /**
     * Returns the height in pixels of this font. The returned value corresponds to the
     * maximum height occupied by a character in the font.
     *
     * @return The height in pixels of this font.
     *
     * @note It is not sufficient to allocate text areas with this height. Use
     *       getMinimumTextHeight for this.
     */
    FORCE_INLINE_FUNCTION virtual uint16_t getFontHeight() const
    {
        return fontHeight;
    }

    /**
     * Returns the minimum height needed for a text field that uses this font. Takes into
     * account that certain characters (eg 'g') have pixels below the baseline, thus making
     * the text height larger than the font height.
     *
     * @return The minimum height needed for a text field that uses this font.
     */
    FORCE_INLINE_FUNCTION virtual uint16_t getMinimumTextHeight() const
    {
        return fontHeight + pixelsBelowBaseline;
    }

    /**
     * Gets bits per pixel for this font.
     *
     * @return The number of bits used per pixel in this font.
     */
    FORCE_INLINE_FUNCTION virtual uint8_t getBitsPerPixel() const
    {
        return bPerPixel;
    }

    /**
     * Are the glyphs saved with each glyph row byte aligned?
     *
     * @return True if each glyph row is stored byte aligned, false otherwise.
     */
    FORCE_INLINE_FUNCTION virtual uint8_t getByteAlignRow() const
    {
        return bAlignRow;
    }

    /**
     * Gets maximum pixels left of any glyph in the font. This is the max value of "left"
     * for all glyphs. The value is negated so if a "g" has left=-6 maxPixelsLeft is 6. This
     * value is calculated by the font converter.
     *
     * @return The maximum pixels left.
     */
    FORCE_INLINE_FUNCTION uint8_t getMaxPixelsLeft() const
    {
        return maxPixelsLeft;
    }

    /**
     * Gets maximum pixels right of any glyph in the font. This is the max value of
     * "width+left-advance" for all glyphs. The is the number of pixels a glyph reaches to
     * the right of its normal area. This value is calculated by the font converter.
     *
     * @return The maximum pixels right.
     */
    FORCE_INLINE_FUNCTION uint8_t getMaxPixelsRight() const
    {
        return maxPixelsRight;
    }

    /**
     * Gets the kerning distance between two characters.
     *
     * @param  prevChar The Unicode value of the previous character.
     * @param  glyph    the glyph object for the current character.
     *
     * @return The kerning distance between prevChar and glyph char.
     */
    virtual int8_t getKerning(Unicode::UnicodeChar prevChar, const GlyphNode* glyph) const
    {
        return 0;
    }

    /**
     * Count the number of lines in a given text.
     *
     * @param  text The text.
     * @param  ...  Variable arguments providing additional information.
     *
     * @return The number of lines.
     */
    virtual uint16_t getNumberOfLines(const Unicode::UnicodeChar* text, ...) const;

    /**
     * Gets GSUB table. Currently only used for Devanagari fonts.
     *
     * @return The GSUB table or null if font has GSUB no table.
     */
    virtual const uint16_t* getGSUBTable() const
    {
        return 0;
    }

    /**
     * Query if 'character' is invisible, zero width.
     *
     * @param  character The character.
     *
     * @return True if invisible, zero width, false if not.
     */
    FORCE_INLINE_FUNCTION static bool isInvisibleZeroWidth(Unicode::UnicodeChar character)
    {
        return character == 0xFEFF || character == 0x200B;
    }

protected:
    /**
     * Gets the width in pixels of the specified string. If the string contains multiple
     * lines, the width of the widest line is found. Please note that the correct number of
     * arguments must be given if the text contains wildcards.
     *
     * @param  textDirection The text direction.
     * @param  text          A null-terminated Unicode string with arguments to insert if the text
     *                       contains wildcards.
     * @param  pArg          Variable arguments providing additional information inserted at
     *                       wildcard placeholders.
     *
     * @return The width in pixels of the longest line of the specified string.
     *
     * @note The string is assumed to be purely left-to-right.
     */
    uint16_t getStringWidthLTR(TextDirection textDirection, const Unicode::UnicodeChar* text, va_list pArg) const;

    /**
     * Gets the width in pixels of the specified string. If the string contains multiple
     * lines, the width of the widest line is found. Please note that the correct number of
     * arguments must be given if the text contains wildcards.
     *
     * The string is handled as a right-to-left string and subdivided into smaller text
     * strings to correctly handle mixing of left-to-right and right-to-left strings.
     *
     * @param  textDirection The text direction.
     * @param  text          A null-terminated Unicode string with arguments to insert if the text
     *                       contains wildcards.
     * @param  pArg          Variable arguments providing additional information inserted at
     *                       wildcard placeholders.
     *
     * @return The string width RTL.
     */
    uint16_t getStringWidthRTL(TextDirection textDirection, const Unicode::UnicodeChar* text, va_list pArg) const;

    /**
     * Initializes a new instance of the Font class. The protected constructor of a Font.
     *
     * @param  height       The font height in pixels.
     * @param  pixBelowBase The number of pixels below the base line.
     * @param  bitsPerPixel The number of bits per pixel.
     * @param  byteAlignRow The glyphs are saved with each row byte aligned.
     * @param  maxLeft      The maximum left extend for a glyph in the font.
     * @param  maxRight     The maximum right extend for a glyph in the font.
     * @param  fallbackChar The fallback character for the typography in case no glyph is
     *                      available.
     * @param  ellipsisChar The ellipsis character used for truncating long texts.
     */
    Font(uint16_t height, uint8_t pixBelowBase, uint8_t bitsPerPixel, uint8_t byteAlignRow, uint8_t maxLeft, uint8_t maxRight, const Unicode::UnicodeChar fallbackChar, const Unicode::UnicodeChar ellipsisChar)
        : fontHeight(height),
          pixelsBelowBaseline(pixBelowBase),
          bPerPixel(bitsPerPixel),
          bAlignRow(byteAlignRow),
          maxPixelsLeft(maxLeft),
          maxPixelsRight(maxRight),
          fallbackCharacter(fallbackChar),
          ellipsisCharacter(ellipsisChar)
    {
    }

    uint16_t fontHeight;                    ///< The font height in pixels
    uint8_t pixelsBelowBaseline;            ///< The number of pixels below the base line
    uint8_t bPerPixel : 7;                  ///< The number of bits per pixel
    uint8_t bAlignRow : 1;                  ///< The glyphs are saved with each row byte aligned
    uint8_t maxPixelsLeft;                  ///< The maximum number of pixels a glyph extends to the left
    uint8_t maxPixelsRight;                 ///< The maximum number of pixels a glyph extends to the right
    Unicode::UnicodeChar fallbackCharacter; ///< The fallback character to use when no glyph exists for the wanted character
    Unicode::UnicodeChar ellipsisCharacter; ///< The ellipsis character used for truncating long texts.

private:
    Font();
};

} // namespace touchgfx

#endif // FONT_HPP
