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
 * @file touchgfx/ConstFont.hpp
 *
 * Declares the touchgfx::ConstFont class.
 */
#ifndef CONSTFONT_HPP
#define CONSTFONT_HPP

#include <touchgfx/Font.hpp>

namespace touchgfx
{
/**
 * A ConstFont is a Font implementation that has its contents defined at compile-time and
 * usually placed in read-only memory.
 *
 * @see Font
 *
 * @note Pure virtual class. Create an application-specific implementation of getPixelData().
 */
class ConstFont : public Font
{
public:
    /**
     * Initializes a new instance of the ConstFont class.
     *
     * @param  list         The array of glyphs known to this font.
     * @param  size         The number of glyphs in list.
     * @param  height       The height in pixels of the highest character in this font.
     * @param  pixBelowBase The maximum number of pixels that can be drawn below the baseline in
     *                      this font.
     * @param  bitsPerPixel The number of bits per pixel in this font.
     * @param  byteAlignRow The glyphs are saved with each row byte aligned.
     * @param  maxLeft      The maximum a character extends to the left.
     * @param  maxRight     The maximum a character extends to the right.
     * @param  fallbackChar The fallback character for the typography in case no glyph is
     *                      available.
     * @param  ellipsisChar The ellipsis character used for truncating long texts.
     */
    ConstFont(const GlyphNode* list, uint16_t size, uint16_t height, uint8_t pixBelowBase, uint8_t bitsPerPixel, uint8_t byteAlignRow, uint8_t maxLeft, uint8_t maxRight, const Unicode::UnicodeChar fallbackChar, const Unicode::UnicodeChar ellipsisChar);

    using Font::getGlyph;

    virtual const GlyphNode* getGlyph(Unicode::UnicodeChar unicode, const uint8_t*& pixelData, uint8_t& bitsPerPixel) const;

    /**
     * Gets the pixel date associated with this glyph.
     *
     * @param  glyph The glyph to get the pixels data from.
     *
     * @return Pointer to the pixel data of this glyph.
     */
    virtual const uint8_t* getPixelData(const GlyphNode* glyph) const = 0;

    virtual int8_t getKerning(Unicode::UnicodeChar prevChar, const GlyphNode* glyph) const = 0;

    /**
     * Finds the glyph data associated with the specified unicode.
     *
     * @param  unicode The character to look up.
     *
     * @return A pointer to the glyph node or null if the glyph was not found.
     */
    const GlyphNode* find(Unicode::UnicodeChar unicode) const;

protected:
    const GlyphNode* glyphList; ///< The list of glyphs
    uint16_t listSize;          ///< The size of the list of glyphs

private:
    ConstFont()
        : Font(0, 0, 0, 0, 0, 0, 0, 0), glyphList(0), listSize(0)
    {
    }
};

} // namespace touchgfx

#endif // CONSTFONT_HPP
