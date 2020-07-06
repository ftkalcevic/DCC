#pragma once

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

#include <touchgfx/widgets/Button.hpp>
#include <touchgfx/Bitmap.hpp>


#define MAX_TOGGLEBUTTONICONANDTEXT_TEXT_LEN 20
	

class ToggleButtonIconAndText : public Button
{
public:
    ToggleButtonIconAndText() : Button(), originalPressed(BITMAP_INVALID), rotation(TEXT_ROTATE_0)
    {
    }

    virtual void setBitmaps(const Bitmap& bmpReleased, const Bitmap& bmpPressed)
    {
        originalPressed = bmpPressed;
        Button::setBitmaps(bmpReleased, bmpPressed);
    }

    virtual void setBitmaps(const Bitmap& bmpReleased, const Bitmap& bmpPressed, const Bitmap& iconReleased, const Bitmap& iconPressed)
    {
	    this->icon = iconReleased;
	    this->iconPressed = iconPressed;
	    setBitmaps(bmpReleased, bmpPressed);
    }
	
    void forceState(bool activeState)
    {
        if (activeState)
        {
            // up should equal originalPressed
            if (up != originalPressed)
            {
                down = up;
                up = originalPressed;
            }
        }
        else
        {
            // down should equal originalPressed
            if (down != originalPressed)
            {
                up = down;
                down = originalPressed;
            }
        }
    }
	
    bool getState() const
    {
        return up == originalPressed;
    }

    virtual void handleClickEvent(const ClickEvent& event)
    {
        if (pressed && (event.getType() == ClickEvent::RELEASED))
        {
            Bitmap tmp = up;
            up = down;
            down = tmp;
        }
        Button::handleClickEvent(event);
    }

    void setTypedText(TypedText t)
    {
        typedText = t;
	    UpdateTextPosition();
    }
	
    void setLabelColor(colortype col)
    {
        textColor = col;
    }	
	void setLabelColorPressed(colortype col, bool performInvalidate = false)
	{
		textColorPressed = col;
	}
	
    void draw(const Rect& invalidatedArea) const
    {
        Bitmap bmp(AbstractButton::pressed ? down : up);
	    uint16_t width = bmp.getWidth(), height = bmp.getHeight();
	    uint16_t x = 0, y = 0;
	    
        Rect dirty(0, 0, width, height);
        dirty &= invalidatedArea;
        if ((bmp.getId() != BITMAP_INVALID) && !dirty.isEmpty())
        {
            Rect r;
            translateRectToAbsolute(r);
            HAL::lcd().drawPartialBitmap(bmp, r.x, r.y, dirty, alpha);
        }
	    
        Bitmap bmpIcon(AbstractButton::pressed ? iconPressed : icon);
	    if (bmpIcon.getId() != BITMAP_INVALID)
	    {
		    if (height > bmpIcon.getHeight())
			    y = x = (height - bmpIcon.getHeight()) / 2;
            Rect dirty(x, y, bmpIcon.getWidth(), bmpIcon.getHeight());
            dirty &= invalidatedArea;
            if ( !dirty.isEmpty())
            {
                Rect r(dirty);
                translateRectToAbsolute(r);
                dirty.x -= x;
                dirty.y -= y;
                HAL::lcd().drawPartialBitmap(bmpIcon, r.x, r.y, dirty, alpha);
            }
		    x += bmpIcon.getWidth();
	    }
	    
	    if (text != nullptr)
	    {
            if (typedText.hasValidId())
            {
                const Font* fontToDraw = typedText.getFont(); //never return 0
                uint8_t height = textHeightIncludingSpacing;
                int16_t offsety;
                int16_t offsetx;
                Rect labelRect;
                switch (rotation)
                {
                    default:
                    case TEXT_ROTATE_0:
                    case TEXT_ROTATE_180:
                        offsety = (this->getHeight() - height)/2;
                        offsetx = ((this->getWidth()-x) - textWidth)/2;
                        labelRect = Rect(offsetx, offsety, textWidth, height);
                        break;
                    case TEXT_ROTATE_90:
                    case TEXT_ROTATE_270:
                        offsety = (this->getWidth() - height)/2;
                        labelRect = Rect(offsety, 0, height, this->getHeight());
                        break;
                }

    //	        if (pressed)
    //	        {
    //		        labelRect.x += 5;
    //		        labelRect.y += 5;
    //	        }
	            Rect dirty = labelRect & invalidatedArea;
                if (!dirty.isEmpty())
                {
                    dirty.x -= labelRect.x;
                    dirty.y -= labelRect.y;
                    translateRectToAbsolute(labelRect);
                    LCD::StringVisuals visuals(fontToDraw, pressed ? textColorPressed : textColor, alpha, typedText.getAlignment(), 0, rotation, typedText.getTextDirection(), 0, WIDE_TEXT_NONE);
                    HAL::lcd().drawString(labelRect, dirty, visuals, text);
                }
            }
	    }
	    
    }
	
	void setText(const Unicode::UnicodeChar *txt)
	{
		Unicode::strncpy(text,txt,MAX_TOGGLEBUTTONICONANDTEXT_TEXT_LEN);
		text[MAX_TOGGLEBUTTONICONANDTEXT_TEXT_LEN-1] = 0;
		UpdateTextPosition();
	}
	
	void setText(const char16_t *txt)
	{
		this->setText((const Unicode::UnicodeChar *)txt);
	}
	
    void UpdateTextPosition()
    {
        if (typedText.hasValidId())
        {
            const Font* f = typedText.getFont();
            textHeightIncludingSpacing = f->getMaxTextHeight(text) * f->getNumberOfLines(text) + f->getSpacingAbove(text);
            textWidth = f->getStringWidth(text);
        }
        else
        {
            textHeightIncludingSpacing = 0;
	        textWidth = 0;
        }
    }
	
    void setLabelRotation(TextRotation rotation)
    {
        this->rotation = rotation;
    }

	
protected:
    int textHeightIncludingSpacing;
    int textWidth;
    TextRotation rotation;                   ///< The rotation used for the label.
    TypedText typedText;
	colortype textColor;
	colortype textColorPressed;
    Bitmap originalPressed; ///< Contains the bitmap that was originally being displayed when button is pressed.
	Bitmap icon;
	Bitmap iconPressed;
	Unicode::UnicodeChar text[MAX_TOGGLEBUTTONICONANDTEXT_TEXT_LEN];
};
