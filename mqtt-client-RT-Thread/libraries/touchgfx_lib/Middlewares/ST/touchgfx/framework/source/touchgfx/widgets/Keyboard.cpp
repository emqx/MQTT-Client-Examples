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

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/widgets/Keyboard.hpp>

namespace touchgfx
{
Keyboard::Keyboard()
    : Container(), keyListener(0), bufferSize(0), bufferPosition(0), highlightImage(), cancelIsEmitted(false)
{
    setTouchable(true);

    keyMappingList = static_cast<KeyMappingList*>(0);
    buffer = static_cast<Unicode::UnicodeChar*>(0);
    layout = static_cast<Layout*>(0);

    image.setXY(0, 0);
    Container::add(image);

    highlightImage.setVisible(false);
    Container::add(highlightImage);

    enteredText.setColor(Color::getColorFrom24BitRGB(0, 0, 0));
    Container::add(enteredText);
}

void Keyboard::setBuffer(Unicode::UnicodeChar* newBuffer, uint16_t newBufferSize)
{
    buffer = newBuffer;
    bufferSize = newBufferSize;

    enteredText.setWildcard(buffer);

    // Place cursor at end of string if we already have something
    // in the edit buffer.
    bufferPosition = Unicode::strlen(buffer);
}

void Keyboard::setLayout(const Layout* newLayout)
{
    layout = newLayout;
    if (newLayout != 0)
    {
        image.setBitmap(Bitmap(newLayout->bitmap));

        enteredText.setTypedText(newLayout->textAreaFont);
        enteredText.setColor(newLayout->textAreaFontColor);
        enteredText.setPosition(newLayout->textAreaPosition.x, newLayout->textAreaPosition.y,
                                newLayout->textAreaPosition.width, newLayout->textAreaPosition.height);
    }
    invalidate();
}

void Keyboard::setTextIndentation()
{
    if (layout != 0)
    {
        uint8_t indentation = layout->textAreaFont.getFont()->getMaxPixelsLeft();
        enteredText.setPosition(layout->textAreaPosition.x - indentation, layout->textAreaPosition.y,
                                layout->textAreaPosition.width + indentation * 2, layout->textAreaPosition.height);
        enteredText.setIndentation(indentation);
    }
}

Keyboard::Key Keyboard::getKeyForCoordinates(int16_t x, int16_t y) const
{
    Key key;
    key.keyId = 0; // No key
    if (layout != 0)
    {
        for (uint8_t i = 0; i < layout->numberOfKeys; i++)
        {
            if (layout->keyArray[i].keyArea.intersect(x, y))
            {
                key = layout->keyArray[i];
                break;
            }
        }
    }
    return key;
}

Keyboard::CallbackArea Keyboard::getCallbackAreaForCoordinates(int16_t x, int16_t y) const
{
    CallbackArea area;
    area.callback = reinterpret_cast<GenericCallback<>*>(0);
    if (layout != 0)
    {
        for (uint8_t i = 0; i < layout->numberOfCallbackAreas; i++)
        {
            if (layout->callbackAreaArray[i].keyArea.intersect(x, y))
            {
                area = layout->callbackAreaArray[i];
                break;
            }
        }
    }
    return area;
}

void Keyboard::draw(const Rect& invalidatedArea) const
{
    assert(layout && "No layout configured for Keyboard");
    if (layout != 0)
    {
        Font* font = FontManager::getFont(layout->keyFont);
        assert(font && "Keyboard::draw: Unable to find font, is font db initialized?");
        if (font != 0)
        {
            // Setup visuals for h-center of "string"
            LCD::StringVisuals visuals;
            visuals.font = font;
            visuals.alignment = CENTER;
            visuals.color = layout->keyFontColor;
            // String with room for a single character
            Unicode::UnicodeChar character[2] = { 0, 0 }; // The last is important as string terminator.

            uint16_t fontHeight = font->getMinimumTextHeight();

            for (uint8_t i = 0; i < layout->numberOfKeys; i++)
            {
                const Key& key = layout->keyArray[i];
                if (key.keyArea.intersect(invalidatedArea))
                {
                    uint8_t keyId = key.keyId;
                    Unicode::UnicodeChar c = getCharForKey(keyId);
                    if (c != 0)
                    {
                        // Get a copy of the keyArea and v-center the area for the character
                        Rect keyArea = key.keyArea;
                        uint16_t offset = (keyArea.height - fontHeight) / 2;
                        keyArea.y += offset;
                        keyArea.height -= offset;
                        // Calculate the invalidated area relative to the key
                        Rect invalidatedAreaRelative = key.keyArea & invalidatedArea;
                        invalidatedAreaRelative.x -= keyArea.x;
                        invalidatedAreaRelative.y -= keyArea.y;
                        // Set up string with one character
                        character[0] = c;
                        translateRectToAbsolute(keyArea);
                        HAL::lcd().drawString(keyArea, invalidatedAreaRelative, visuals, character);
                    }
                }
            }
        }
    }
}

void Keyboard::handleClickEvent(const ClickEvent& evt)
{
    ClickEvent::ClickEventType type = evt.getType();
    int16_t x = evt.getX();
    int16_t y = evt.getY();
    Rect toDraw;

    Keyboard::CallbackArea callbackArea = getCallbackAreaForCoordinates(x, y);
    if (callbackArea.callback != 0)
    {
        if (type == ClickEvent::PRESSED)
        {
            highlightImage.setXY(callbackArea.keyArea.x, callbackArea.keyArea.y);
            highlightImage.setBitmap(Bitmap(callbackArea.highlightBitmapId));
            highlightImage.setVisible(true);
            toDraw = highlightImage.getRect();
            invalidateRect(toDraw);
        }

        if ((type == ClickEvent::RELEASED) && callbackArea.callback->isValid())
        {
            if (cancelIsEmitted)
            {
                cancelIsEmitted = false;
            }
            else
            {
                callbackArea.callback->execute();
                if (keyListener)
                {
                    keyListener->execute(0);
                }
            }
        }
    }
    else
    {
        Keyboard::Key key = getKeyForCoordinates(x, y);

        if (key.keyId != 0)
        {
            if (type == ClickEvent::PRESSED)
            {
                highlightImage.setXY(key.keyArea.x, key.keyArea.y);
                highlightImage.setBitmap(Bitmap(key.highlightBitmapId));
                highlightImage.setVisible(true);
                toDraw = highlightImage.getRect();
                invalidateRect(toDraw);
            }

            if (type == ClickEvent::RELEASED)
            {
                if (cancelIsEmitted)
                {
                    cancelIsEmitted = false;
                }
                else
                {
                    if (buffer)
                    {
                        Unicode::UnicodeChar c = getCharForKey(key.keyId);
                        if (c != 0)
                        {
                            uint16_t prevBufferPosition = bufferPosition;
                            if (bufferPosition < (bufferSize - 1))
                            {
                                buffer[bufferPosition++] = c;
                                buffer[bufferPosition] = 0;
                            }
                            if (prevBufferPosition != bufferPosition)
                            {
                                enteredText.invalidate();
                                if (keyListener)
                                {
                                    keyListener->execute(c);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (type == ClickEvent::RELEASED || type == ClickEvent::CANCEL)
    {
        toDraw = highlightImage.getRect();
        highlightImage.setVisible(false);
        invalidateRect(toDraw);

        if (type == ClickEvent::CANCEL)
        {
            cancelIsEmitted = true;
        }
    }
}

void Keyboard::handleDragEvent(const DragEvent& evt)
{
    if (highlightImage.isVisible() && (!highlightImage.getRect().intersect(static_cast<int16_t>(evt.getNewX()), static_cast<int16_t>(evt.getNewY()))) && (!cancelIsEmitted))
    {
        // Send a CANCEL click event, if user has dragged out of currently pressed/highlighted key.
        touchgfx::ClickEvent cancelEvent(touchgfx::ClickEvent::CANCEL, static_cast<int16_t>(evt.getOldX()), static_cast<int16_t>(evt.getOldY()));
        handleClickEvent(cancelEvent);
    }
}

Unicode::UnicodeChar Keyboard::getCharForKey(uint8_t keyId) const
{
    Unicode::UnicodeChar ch = 0;
    if (keyMappingList != 0)
    {
        for (uint8_t i = 0; i < keyMappingList->numberOfKeys; i++)
        {
            if (keyMappingList->keyMappingArray[i].keyId == keyId)
            {
                ch = keyMappingList->keyMappingArray[i].keyValue;
                break;
            }
        }
    }
    return ch;
}

void Keyboard::setupDrawChain(const Rect& invalidatedArea, Drawable** nextPreviousElement)
{
    // Keyboard is a Container, and they do not normally appear in the draw chain (they just draw children).
    // But this particular container actually has a draw() function implementation, so we must change default
    // behavior.
    // First, add children
    Container::setupDrawChain(invalidatedArea, nextPreviousElement);
    // Then add yourself
    Drawable::setupDrawChain(invalidatedArea, nextPreviousElement);
}

void Keyboard::setBufferPosition(uint16_t newPos)
{
    bufferPosition = newPos;
    enteredText.invalidate();
}

void Keyboard::setKeymappingList(const KeyMappingList* newKeyMappingList)
{
    keyMappingList = newKeyMappingList;
    invalidate();
}
} // namespace touchgfx
