/**
  ******************************************************************************
  * This file is part of the TouchGFX 4.13.0 distribution.
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
    : Container(), keyListener(0), bufferSize(0), bufferPosition(0), keyDown(0), cancelIsEmitted(false)
{
    setTouchable(true);

    keyMappingList = static_cast<KeyMappingList*>(0);
    buffer = static_cast<Unicode::UnicodeChar*>(0);
    layout = static_cast<Layout*>(0);

//    highlightImage.setVisible(false);
//    Container::add(highlightImage);

//    enteredText.setColor(Color::getColorFrom24BitRGB(0, 0, 0));
//    Container::add(enteredText);
}

void Keyboard::setBuffer(Unicode::UnicodeChar* newBuffer, uint16_t newBufferSize)
{
    buffer = newBuffer;
    bufferSize = newBufferSize;

    // Place cursor at end of string if we already have something
    // in the edit buffer.
    bufferPosition = Unicode::strlen(buffer);
	enteredTextInvalidate();
}

void Keyboard::setLayout(const Layout* newLayout)
{
    layout = newLayout;
    invalidate();
}

void Keyboard::setTextIndentation()
{
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

Keyboard::CallbackArea *Keyboard::getCallbackAreaForCoordinates(int16_t x, int16_t y) const
{
    CallbackArea *area = NULL;
    if (layout != 0)
    {
        for (uint8_t i = 0; i < layout->numberOfCallbackAreas; i++)
        {
            if (layout->callbackAreaArray[i].keyArea.intersect(x, y))
            {
                area = layout->callbackAreaArray + i;
                break;
            }
        }
    }
    return area;
}

	
void Keyboard::DrawKey(const Rect& invalidatedArea, const Rect &origKeyArea, const LCD::StringVisuals &visuals, Unicode::UnicodeChar const *str, bool isKeyDown ) const
{
    if (origKeyArea.intersect(invalidatedArea))
    {
	    // Get a copy of the keyArea and v-center the area for the character
		Rect keyArea = origKeyArea;
	                    
	    // Draw key
	    //if ( keyUp )
	    {
	        uint16_t shadowDepth = layout->shadowDepth;
		    if (isKeyDown)
		    {
			    keyArea.x += layout->shadowDepth - 1;
			    keyArea.y += layout->shadowDepth - 1;
			    shadowDepth = 1;
		    }
			    
            Rect buttonRect = keyArea;
		    buttonRect = buttonRect & invalidatedArea;
            translateRectToAbsolute(buttonRect);
		    if ( !buttonRect.isEmpty() )
                HAL::lcd().fillRect(buttonRect, layout->keyBackColor, layout->keyAlpha);
		    
		    Rect bottomShadow(keyArea.x + layout->shadowDepth, keyArea.bottom(), keyArea.width, layout->shadowDepth);
		    bottomShadow = bottomShadow & invalidatedArea;
            translateRectToAbsolute(bottomShadow);
		    if ( !bottomShadow.isEmpty() )
                HAL::lcd().fillRect(bottomShadow, layout->shadowColor, layout->keyAlpha);

		    Rect rightShadow(keyArea.right(), keyArea.y + layout->shadowDepth, layout->shadowDepth, keyArea.height);
		    rightShadow = rightShadow & invalidatedArea;
            translateRectToAbsolute(rightShadow);
		    if ( !rightShadow.isEmpty() )
                HAL::lcd().fillRect(rightShadow, layout->shadowColor, layout->keyAlpha);
	    }
        Rect keyAreaDirty = keyArea;
	             
	    if (str[0] != 0)
	    {
            uint16_t fontHeight = visuals.font->getMinimumTextHeight();
            uint16_t offset = (keyArea.height - fontHeight) / 2;
            keyAreaDirty.y += offset;
            keyAreaDirty.height -= offset;
            // Calculate the invalidated area relative to the key
            Rect invalidatedAreaRelative = keyArea & invalidatedArea;
            invalidatedAreaRelative.x -= keyAreaDirty.x;
            invalidatedAreaRelative.y -= keyAreaDirty.y;
	        // Set up string with one character

            translateRectToAbsolute(keyAreaDirty);
            HAL::lcd().drawString(keyAreaDirty, invalidatedAreaRelative, visuals, str);
	    }
    }
}

void Keyboard::draw(const Rect& invalidatedArea) const
{
    assert(layout && "No layout configured for Keyboard");
    if (layout != 0)
    {
	    // background
	    {
            Rect dirty = invalidatedArea;
            translateRectToAbsolute(dirty);
            HAL::lcd().fillRect(dirty, layout->backColor, layout->backAlpha);
	    }
	    
        Font* font = FontManager::getFont(layout->keyFont);
        assert(font && "Keyboard::draw: Unable to find font, is font db initialized?");
        if (font != 0)
        {
            // Setup visuals for h-center of "string"
            LCD::StringVisuals visuals;
            visuals.font = font;
            visuals.alignment = CENTER;
            visuals.color = layout->keyFontColor;

            for (uint8_t i = 0; i < layout->numberOfKeys; i++)
            {
                const Key& key = layout->keyArray[i];
                Unicode::UnicodeChar c = getCharForKey(key.keyId);
		        Unicode::UnicodeChar str[2] = { c, 0 };
	            DrawKey(invalidatedArea, key.keyArea, visuals, str, keyDown == key.keyId);
            }
            for (uint8_t i = 0; i < layout->numberOfCallbackAreas; i++)
            {
                const CallbackArea& callbackArea = layout->callbackAreaArray[i];
	            DrawKey(invalidatedArea, callbackArea.keyArea, visuals, (Unicode::UnicodeChar*)callbackArea.str, keyDown == callbackArea.keyId);
            }
        }
	    
        Rect invalidatedTextAreaRelative = layout->textAreaPosition;
	    invalidatedTextAreaRelative = invalidatedTextAreaRelative & invalidatedArea;
        translateRectToAbsolute(invalidatedTextAreaRelative);
	    if (!invalidatedTextAreaRelative.isEmpty())
	    {
		    HAL::lcd().fillRect(invalidatedTextAreaRelative, layout->textAreaBackColor, layout->keyAlpha);

		    font = FontManager::getFont(layout->textAreaFont);
		    assert(font && "Keyboard::draw: Unable to find textArea font, is font db initialized?");
		    if (font != 0)
		    {
			    LCD::StringVisuals visuals;
                visuals.font = font;
                visuals.alignment = CENTER;
                visuals.color = layout->textAreaFontColor;

			    Rect textArea = layout->textAreaPosition;
			    // Draw text
                uint16_t fontHeight = font->getMinimumTextHeight();
	            uint16_t offset = (textArea.height - fontHeight) / 2;
	            textArea.y += offset;
	            textArea.height -= offset;
	            // Calculate the invalidated area relative to the box
	            Rect invalidatedAreaRelative = layout->textAreaPosition & invalidatedArea;
	            invalidatedAreaRelative.x -= textArea.x;
	            invalidatedAreaRelative.y -= textArea.y;

	            HAL::lcd().drawString(textArea, invalidatedAreaRelative, visuals, buffer);
		    }
	    }
    }
}
#include <stdio.h>
void Keyboard::handleClickEvent(const ClickEvent& evt)
{
    ClickEvent::ClickEventType type = evt.getType();
    int16_t x = evt.getX();
    int16_t y = evt.getY();
	
    Keyboard::CallbackArea *callbackArea = getCallbackAreaForCoordinates(x, y);
    if (callbackArea != NULL)
    {
        if (type == ClickEvent::PRESSED)
        {
	        invalidateKeyRect(keyDown);
            invalidateKeyRect(callbackArea->keyArea);
	        keyDown = callbackArea->keyId;
        }

        if (type == ClickEvent::RELEASED)
        {
	        invalidateKeyRect(keyDown);
	        keyDown = 0;
            if (cancelIsEmitted)
            {
                cancelIsEmitted = false;
            }
            else if ( callbackArea->callback != NULL && callbackArea->callback->isValid() )
            {
                callbackArea->callback->execute();
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
	            invalidateKeyRect(keyDown);
	            invalidateKeyRect(key.keyArea);
	            keyDown = key.keyId;
            }

            if (type == ClickEvent::RELEASED)
            {
	            invalidateKeyRect(keyDown);
	            keyDown = 0;
	            
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
                                enteredTextInvalidate();
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
	    invalidateKeyRect(keyDown);
	    keyDown = 0;

        if (type == ClickEvent::CANCEL)
        {
            cancelIsEmitted = true;
        }
    }
}

void Keyboard::invalidateKeyRect(uint8_t keyId)
{
	if (layout != 0)
	{
		for (uint8_t i = 0; i < layout->numberOfKeys; i++)
			if (layout->keyArray[i].keyId == keyId)
			{
				invalidateKeyRect(layout->keyArray[i].keyArea);
				break;
			}
		for (uint8_t i = 0; i < layout->numberOfCallbackAreas; i++)
			if (layout->callbackAreaArray[i].keyId == keyId)
			{
				invalidateKeyRect(layout->callbackAreaArray[i].keyArea);
				break;
			}
	}
}
	
void Keyboard::invalidateKeyRect(const Rect &rect)
{
	Rect toDraw(rect.x, rect.y, rect.width+layout->shadowDepth, rect.height+layout->shadowDepth);
    invalidateRect(toDraw);
}

void Keyboard::handleDragEvent(const DragEvent& evt)
{
//    if (highlightImage.isVisible() && (!highlightImage.getRect().intersect(static_cast<int16_t>(evt.getNewX()), static_cast<int16_t>(evt.getNewY()))) && (!cancelIsEmitted))
//    {
//        // Send a CANCEL click event, if user has dragged out of currently pressed/highlighted key.
//        touchgfx::ClickEvent cancelEvent(touchgfx::ClickEvent::CANCEL, static_cast<int16_t>(evt.getOldX()), static_cast<int16_t>(evt.getOldY()));
//        handleClickEvent(cancelEvent);
//    }
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
    enteredTextInvalidate();
}

void Keyboard::setKeymappingList(const KeyMappingList* newKeyMappingList)
{
    keyMappingList = newKeyMappingList;
    invalidate();
}

void Keyboard::enteredTextInvalidate() const
{
	Rect temp(layout->textAreaPosition);
	invalidateRect(temp);
}
} // namespace touchgfx
