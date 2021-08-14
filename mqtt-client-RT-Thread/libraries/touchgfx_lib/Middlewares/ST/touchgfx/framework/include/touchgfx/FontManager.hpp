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
 * @file touchgfx/FontManager.hpp
 *
 * Declares the touchgfx::FontProvider class.
 */
#ifndef FONTMANAGER_HPP
#define FONTMANAGER_HPP

#include <touchgfx/Font.hpp>

namespace touchgfx
{
/**
 * A generic pure virtual definition of a FontProvider, which is a class capable of returning a
 * Font based on a FontId. An application-specific derivation of this class must be
 * implemented.
 */
class FontProvider
{
public:
    /**
     * Gets a Font.
     *
     * @param  fontId The FontId of the font to get.
     *
     * @return The font with a font id of fontId.
     */
    virtual Font* getFont(FontId fontId) = 0;

    /** Finalizes an instance of the FontProvider class. */
    virtual ~FontProvider()
    {
    }

private:
};

/**
 * This class is the entry point for looking up a font based on a font id. Must be initialized
 * with the appropriate FontProvider by the application.
 */
class FontManager
{
public:
    /**
     * Sets the font provider. Must be initialized with the appropriate FontProvider by the
     * application.
     *
     * @param [in] fontProvider Sets the font provider. Must be initialized with the
     *                          appropriate FontProvider by the application.
     */
    static void setFontProvider(FontProvider* fontProvider);

    /**
     * Gets a font.
     *
     * @param  fontId The FontId of the font to get.
     *
     * @return The font with a FontId of fontId.
     */
    static Font* getFont(FontId fontId);

private:
    static FontProvider* provider;
};

} // namespace touchgfx

#endif // FONTMANAGER_HPP
