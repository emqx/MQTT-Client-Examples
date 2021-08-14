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

#include <touchgfx/widgets/ButtonWithLabel.hpp>
#include <touchgfx/FontManager.hpp>
#include <touchgfx/hal/HAL.hpp>

namespace touchgfx
{
ButtonWithLabel::ButtonWithLabel()
    : Button(), color(0), colorPressed(0), rotation(TEXT_ROTATE_0), textHeightIncludingSpacing(0)
{
}

void ButtonWithLabel::draw(const Rect& area) const
{
    Button::draw(area);

    if (typedText.hasValidId())
    {
        const Font* fontToDraw = typedText.getFont(); //never return 0
        uint8_t height = textHeightIncludingSpacing;
        int16_t offset;
        Rect labelRect;
        switch (rotation)
        {
        default:
        case TEXT_ROTATE_0:
        case TEXT_ROTATE_180:
            offset = (this->getHeight() - height) / 2;
            labelRect = Rect(0, offset, this->getWidth(), height);
            break;
        case TEXT_ROTATE_90:
        case TEXT_ROTATE_270:
            offset = (this->getWidth() - height) / 2;
            labelRect = Rect(offset, 0, height, this->getHeight());
            break;
        }
        Rect dirty = labelRect & area;

        if (!dirty.isEmpty())
        {
            dirty.x -= labelRect.x;
            dirty.y -= labelRect.y;
            translateRectToAbsolute(labelRect);
            LCD::StringVisuals visuals(fontToDraw, pressed ? colorPressed : color, alpha, typedText.getAlignment(), 0, rotation, typedText.getTextDirection(), 0, WIDE_TEXT_NONE);
            HAL::lcd().drawString(labelRect, dirty, visuals, typedText.getText());
        }
    }
}
} // namespace touchgfx
