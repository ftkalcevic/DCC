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

#include <gui/common/ButtonWithLabelAndEnable.hpp>
#include <touchgfx/FontManager.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/Color.hpp>

namespace touchgfx
{
ButtonWithLabelAndEnable::ButtonWithLabelAndEnable()
	: ButtonWithLabel()
{
}
	
void ButtonWithLabelAndEnable::setBitmaps(const Bitmap& newBackgroundReleased, const Bitmap& newBackgroundPressed, const Bitmap& newBackgroundDisabled )
{
    Button::setBitmaps(newBackgroundReleased, newBackgroundPressed);
	backgroundDisabled = newBackgroundDisabled;
	buttonEnabled = true;
}

void ButtonWithLabelAndEnable::handleClickEvent(const ClickEvent & event)
{
	if (buttonEnabled)
	{
		ButtonWithLabel::handleClickEvent(event);
	}
}
	
void ButtonWithLabelAndEnable::draw(const Rect& invalidatedArea) const
{
	if (buttonEnabled)
	{
		ButtonWithLabel::draw(invalidatedArea);
	}
	else
	{
        Bitmap bmp(backgroundDisabled);
        Rect dirty(0, 0, bmp.getWidth(), bmp.getHeight());
        dirty &= invalidatedArea;
        if ((bmp.getId() != BITMAP_INVALID) && !dirty.isEmpty())
        {
            Rect r;
            translateRectToAbsolute(r);
            HAL::lcd().drawPartialBitmap(bmp, r.x, r.y, dirty, alpha);
        }
		
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
            Rect dirty = labelRect & invalidatedArea;

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
	
}
} // namespace touchgfx
