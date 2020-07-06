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

#include <gui/common/CustomButton.hpp>
#include <touchgfx/FontManager.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/Color.hpp>

namespace touchgfx
{
CustomButton::CustomButton()
    : Button(), color(0), colorPressed(0), rotation(TEXT_ROTATE_0), textHeightIncludingSpacing(0), textWidth(0), buttonId(0)
{
}
	
void CustomButton::setBitmaps(const Bitmap& newBackgroundReleased, const Bitmap& newBackgroundPressed,
                                const Bitmap& newIconReleased, const Bitmap& newIconPressed)
{
    Button::setBitmaps(newBackgroundReleased, newBackgroundPressed);

    iconReleased = newIconReleased;
    iconPressed = newIconPressed;

	//    iconX = (getWidth() / 2) - (newIconPressed.getWidth() / 2);
	//    iconY = (getHeight() / 2) - (newIconPressed.getHeight() / 2);
	const int TEXT_HEIGHT = 25;
    iconX = ((getWidth()-SHADOW_SIZE) / 2) - (newIconPressed.getWidth() / 2);
    iconY = ((getHeight()-SHADOW_SIZE-TEXT_HEIGHT) / 2) - (newIconPressed.getHeight() / 2);
	if (iconX < 0)
		iconX = 0;
	if (iconY < 0)
		iconY = 0;
}

void CustomButton::draw(const Rect& area) const
{
    Button::draw(area);
	
    Bitmap bmp((pressed ? iconPressed : iconReleased));
    Rect iconRect(iconX, iconY, bmp.getWidth(), bmp.getHeight());
    Rect dirty = area & iconRect;
    if ((bmp.getId() != BITMAP_INVALID) && !dirty.isEmpty())
    {
	    int offset = 0;
	    if (pressed)
	    {
		    offset = 5;
	    }
        Rect r;
        translateRectToAbsolute(r);
        dirty.x -= iconX;
        dirty.y -= iconY;
        HAL::lcd().drawPartialBitmap(bmp, r.x + iconX + offset, r.y + iconY + offset, dirty, alpha);
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
                    offsety = this->getHeight() - height - SHADOW_SIZE - 2;
                    offsetx = (this->getWidth() - textWidth)/2;
                    labelRect = Rect(offsetx, offsety, textWidth, height);
                    break;
                case TEXT_ROTATE_90:
                case TEXT_ROTATE_270:
                    offsety = this->getWidth() - height - SHADOW_SIZE - 2;
                    labelRect = Rect(offsety, 0, height, this->getHeight());
                    break;
            }

	        if (pressed)
	        {
		        labelRect.x += 5;
		        labelRect.y += 5;
	        }
	        Rect dirty = labelRect & area;
            if (!dirty.isEmpty())
            {
                dirty.x -= labelRect.x;
                dirty.y -= labelRect.y;
                translateRectToAbsolute(labelRect);
                LCD::StringVisuals visuals(fontToDraw, pressed ? colorPressed : color, alpha, typedText.getAlignment(), 0, rotation, typedText.getTextDirection(), 0, WIDE_TEXT_NONE);
                HAL::lcd().drawString(labelRect, dirty, visuals, text);
            }
	    }
    }
}
} // namespace touchgfx
