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

#ifndef CUSTOMBUTTON_HPP
#define CUSTOMBUTTON_HPP

#include <touchgfx/widgets/Button.hpp>
#include <touchgfx/TypedText.hpp>

namespace touchgfx
{

class CustomButton : public Button
{
public:
	const int SHADOW_SIZE = 10;
	
    CustomButton();

    void setLabelText(TypedText t)
    {
        typedText = t;
        updateTextPosition();
    }

    void setLabelText(const char *t)
    {
	    Unicode::fromUTF8((const uint8_t *)t, text, sizeof(text)/sizeof(text[0]));
        updateTextPosition();
    }

    TypedText getLabelText() const
    {
        return typedText;
    }

    void setLabelColor(colortype col, bool performInvalidate = false)
    {
        color = col;
        if (performInvalidate)
        {
            invalidate();
        }
    }

    void setLabelColorPressed(colortype col, bool performInvalidate = false)
    {
        colorPressed = col;
        if (performInvalidate)
        {
            invalidate();
        }
    }

    void setLabelRotation(TextRotation rotation)
    {
        this->rotation = rotation;
    }

    TextRotation getLabelRotation()
    {
        return rotation;
    }

    void updateTextPosition()
    {
        if (typedText.hasValidId())
        {
            const Font* f = typedText.getFont();
            const Unicode::UnicodeChar* s = text;
            textHeightIncludingSpacing = f->getMaxTextHeight(s) * f->getNumberOfLines(s) + f->getSpacingAbove(s);
            textWidth = f->getStringWidth(s);
        }
        else
        {
            textHeightIncludingSpacing = 0;
	        textWidth = 0;
        }
    }

    virtual Rect getSolidRect() const
    {
        return Button::getSolidRect();
    }

    virtual void draw(const Rect& area) const;
    void setBitmaps(const Bitmap& newBackgroundReleased, const Bitmap& newBackgroundPressed,
                                const Bitmap& newIconReleased, const Bitmap& newIconPressed);
    void setIconXY(int16_t x, int16_t y)
    {
        iconX = x;
        iconY = y;
    }
	void setId(int id)
	{
		buttonId = id;
	}
	int getId() const
	{
		return buttonId; 
	}
	
protected:
    TypedText    typedText;                  ///< The TypedText used for the button label.
    colortype    color;                      ///< The color used for the label when not pressed.
    colortype    colorPressed;               ///< The color used for the label when pressed.
    TextRotation rotation;                   ///< The rotation used for the label.
    uint8_t      textHeightIncludingSpacing; ///< Total height of the label (text height + spacing).
	uint8_t      textWidth;
	Unicode::UnicodeChar text[30];
    Bitmap  iconReleased; ///< Icon to display when button is not pressed.
    Bitmap  iconPressed;  ///< Icon to display when button is pressed.
    int16_t iconX;        ///< x coordinate offset for icon.
    int16_t iconY;        ///< y coordinate offset for icon.	
	int buttonId;
};
} // namespace touchgfx

#endif // CUSTOMBUTTON_HPP
