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
#include "AudioTask.h"


namespace touchgfx
{
Keyboard::Keyboard()
    : keyListener(0), bufferSize(0), bufferPosition(0), keyDown(0), cancelIsEmitted(false), title(NULL)
{
    setTouchable(true);
	
    keyMappingList = static_cast<KeyMappingList*>(0);
    buffer = static_cast<Unicode::UnicodeChar*>(0);
    layout = static_cast<Layout*>(0);
}

void Keyboard::setBuffer(Unicode::UnicodeChar* newBuffer, uint16_t newBufferSize)
{
    buffer = newBuffer;
    bufferSize = newBufferSize;

    // Place cursor at end of string if we already have something in the edit buffer.
    bufferPosition = Unicode::strlen(buffer);
	enteredTextInvalidate();
}

void Keyboard::setLayout(const Layout* newLayout)
{
    layout = newLayout;
	entryAreaFontId = layout->textAreaFont;
	entryAreaAlignment = layout->textAreaAlignment;
    invalidate();
}
	
Keyboard::Key Keyboard::getKeyForCoordinates(int16_t x, int16_t y) const
{
    Key key;
    key.keyId = 0; // No key
    if (layout != 0)
    {
        for (uint8_t i = 0; i < layout->numberOfKeys; i++)
        {
	        Rect keyArea = layout->keyArray[i].keyArea;
	        translateToAbsolute(keyArea);
            if (keyArea.intersect(x, y))
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
	        Rect keyArea = layout->callbackAreaArray[i].keyArea;
	        translateToAbsolute(keyArea);
            if (keyArea.intersect(x, y))
            {
                area = layout->callbackAreaArray + i;
                break;
            }
        }
    }
    return area;
}

	
void Keyboard::DrawKey(const Rect& invalidatedArea, const Rect &_origKeyArea, const LCD::StringVisuals &visuals, Unicode::UnicodeChar const *str, BitmapId bitmapId, bool isKeyDown ) const
{
	Rect origKeyArea = _origKeyArea;
	translateToAbsolute(origKeyArea);
    if (origKeyArea.intersect(invalidatedArea))
    {
	    // Get a copy of the keyArea and v-center the area for the character
		Rect keyArea = origKeyArea;
	                    
	    // Draw key
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
		    if ( !buttonRect.isEmpty() )
                HAL::lcd().fillRect(buttonRect, layout->keyBackColor, layout->keyAlpha);
		    
		    Rect bottomShadow(keyArea.x + layout->shadowDepth, keyArea.bottom(), keyArea.width, layout->shadowDepth);
		    bottomShadow = bottomShadow & invalidatedArea;
		    if ( !bottomShadow.isEmpty() )
                HAL::lcd().fillRect(bottomShadow, layout->shadowColor, layout->keyAlpha);

		    Rect rightShadow(keyArea.right(), keyArea.y + layout->shadowDepth, layout->shadowDepth, keyArea.height);
		    rightShadow = rightShadow & invalidatedArea;
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

			HAL::lcd().drawString(keyAreaDirty, invalidatedAreaRelative, visuals, str);
	    }
	    else if (bitmapId > 0)
	    {
            Bitmap bmp(bitmapId);
		    
		    // Center icon
		    Rect iconRect(keyArea);
		    iconRect.x += (iconRect.width - bmp.getWidth()) / 2;
		    iconRect.y += (iconRect.height - bmp.getHeight()) / 2;
		    iconRect.width = bmp.getWidth();
		    iconRect.height = bmp.getHeight();
		    // Does it need drawing?
            Rect dirty = invalidatedArea & iconRect;
            if (!dirty.isEmpty())
            {
	            // dirty is the portion that needs to be painted, relative to iconRect.x/y
	            dirty.x -= iconRect.x;
	            dirty.y -= iconRect.y;
                HAL::lcd().drawPartialBitmap(bmp, iconRect.x, iconRect.y, dirty, 255);
            }
	    }
    }
}

void Keyboard::DrawText(const Unicode::UnicodeChar *str, FontId fontId, Alignment align, colortype color, const Rect textArea, const Rect invalidatedArea) const
{
	if (str != nullptr && textArea.intersect(invalidatedArea))
	{
		Font *font = FontManager::getFont(fontId);
		if (font != nullptr)
		{
	        LCD::StringVisuals visuals;
            visuals.font = font;
            visuals.alignment = align;
            visuals.color = color;

	        // Draw text
	        Rect adjTextArea = textArea;
            uint16_t fontHeight = font->getMinimumTextHeight();
	        uint16_t offset = (adjTextArea.height - fontHeight) / 2;
	        adjTextArea.y += offset;
	        adjTextArea.height -= offset;
			
	        // Calculate the invalidated area relative to the box
	        Rect invalidatedAreaRelative = textArea & invalidatedArea;
	        invalidatedAreaRelative.x -= textArea.x;
	        invalidatedAreaRelative.y -= textArea.y;

	        HAL::lcd().drawString(adjTextArea, invalidatedAreaRelative, visuals, str);
		}
	}
}

void Keyboard::draw(const Rect& invalidatedArea) const
{
    assert(layout && "No layout configured for Keyboard");
    if (layout != 0)
    {
	    {
	        // background
            Rect dirty = invalidatedArea;
            HAL::lcd().fillRect(dirty, touchgfx::Color::getColorFrom24BitRGB(0, 0, 0), 128);        // Grey out background
		    
		    // keyboard
            dirty = keyboardRect & invalidatedArea;
            HAL::lcd().fillRect(dirty, layout->backColor, layout->backAlpha);
		    
		    // Frame - todo make this 3d
		    uint16_t borderWidth = 2;
		    colortype borderColor = touchgfx::Color::getColorFrom24BitRGB(0, 0, 0);
		    Rect left(keyboardRect.x, keyboardRect.y, borderWidth, keyboardRect.height);
		    Rect right(keyboardRect.right()-borderWidth, keyboardRect.y, borderWidth, keyboardRect.height);
		    Rect top(keyboardRect.x, keyboardRect.y, keyboardRect.width, borderWidth );
		    Rect bottom(keyboardRect.x, keyboardRect.bottom() - borderWidth, keyboardRect.width, borderWidth );
		    
		    left &= invalidatedArea;
		    right &= invalidatedArea;
		    top &= invalidatedArea;
		    bottom &= invalidatedArea;
		    if ( !left.isEmpty() )  HAL::lcd().fillRect(left, borderColor, 255);
		    if ( !right.isEmpty() )  HAL::lcd().fillRect(right, borderColor, 255);
		    if ( !top.isEmpty() )  HAL::lcd().fillRect(top, borderColor, 255);
		    if ( !bottom.isEmpty() )  HAL::lcd().fillRect(bottom, borderColor, 255);
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
	            DrawKey(invalidatedArea, key.keyArea, visuals, str, key.highlightBitmapId, keyDown == key.keyId);
            }
            for (uint8_t i = 0; i < layout->numberOfCallbackAreas; i++)
            {
                const CallbackArea& callbackArea = layout->callbackAreaArray[i];
	            DrawKey(invalidatedArea, callbackArea.keyArea, visuals, (Unicode::UnicodeChar*)callbackArea.str, callbackArea.highlightBitmapId, keyDown == callbackArea.keyId);
            }
        }
	    
	    // Entered text
	    Rect textAreaPosition = layout->textAreaPosition;
	    translateToAbsolute(textAreaPosition);
        Rect invalidatedTextAreaRelative = textAreaPosition;
	    invalidatedTextAreaRelative = invalidatedTextAreaRelative & invalidatedArea;
	    if (!invalidatedTextAreaRelative.isEmpty())
	    {
		    HAL::lcd().fillRect(invalidatedTextAreaRelative, layout->textAreaBackColor, layout->keyAlpha);

			DrawText(buffer, entryAreaFontId, entryAreaAlignment, layout->textAreaFontColor, textAreaPosition, invalidatedArea);
	    }
	    
	    // title
	    Rect titlePosition = layout->titlePosition;
	    translateToAbsolute(titlePosition);
		DrawText(title, layout->titleFont, layout->titleAlignment, layout->titleFontColor, titlePosition, invalidatedArea);
    }
}

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
	        audioTask.PlaySound(EAudioSounds::KeyPressTone);
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
	            audioTask.PlaySound(EAudioSounds::KeyPressTone);
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
	translateToAbsolute(toDraw);
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
	translateToAbsolute(temp);
	invalidateRect(temp);
}
	
Rect Keyboard::getSolidRect() const
{
	return keyboardRect;
}
	
} // namespace touchgfx



/*
 TODO - 
    - numeric range and digit checking
    - numeric - disable unavailable keys
    - toggle keys - caps lock
    - if just starting, typing deletes what's there.
 **/