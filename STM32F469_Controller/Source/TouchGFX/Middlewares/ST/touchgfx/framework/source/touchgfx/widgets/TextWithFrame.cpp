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

#include <touchgfx/widgets/TextWithFrame.hpp>
#include <touchgfx/hal/HAL.hpp>
#include "UIDefaults.h"


namespace touchgfx
{
	TextWithFrame::TextWithFrame() :
        Widget(), 
		enabled(true),
		text(nullptr),
	    fontId(DefaultFontId), 
		color(DefaultTextColour), 
		disabledColor(DefaultDisabledTextColour), 
		boxColor(DefaultBoxColor), 
		disabledBoxColor(DefaultDisabledBoxColor), 
		linespace(0), 
		alpha(255), 
		indentation(0), 
		wideTextAction(WIDE_TEXT_NONE)
{
}

	
int16_t TextWithFrame::getTextHeight()
{
    return calculateTextHeight(text, 0, 0);
}

uint16_t TextWithFrame::getTextWidth() const
{
	Font *font = FontManager::getFont(fontId);
	if (font != nullptr)
		return font->getStringWidth(TEXT_DIRECTION_LTR, text, 0, 0);
	else
		return 0;
}

void TextWithFrame::draw(const Rect& invalidatedArea) const
{
	Rect dirty = invalidatedArea;
	translateRectToAbsolute(dirty);
	if (!dirty.isEmpty())
		HAL::lcd().fillRect(dirty, enabled ? boxColor : disabledBoxColor, alpha);
	
	Font *font = FontManager::getFont(fontId);
	if (font != nullptr)
    {
        LCD::StringVisuals visuals(font, enabled ? color : disabledColor, alpha, alignment, linespace, TEXT_ROTATE_0, TEXT_DIRECTION_LTR, indentation, wideTextAction);
        HAL::lcd().drawString(getAbsoluteRect(), invalidatedArea, visuals, text, 0, 0);
    }
}

void TextWithFrame::resizeToCurrentText()
{
}

void TextWithFrame::resizeToCurrentTextWithAlignment()
{
}

void TextWithFrame::resizeHeightToCurrentText()
{
}

int16_t TextWithFrame::calculateTextHeight(const Unicode::UnicodeChar* format, ...) const
{
	const Font *fontToDraw = FontManager::getFont(fontId);
	if (fontToDraw != nullptr)
    {
        return 0;
    }

    va_list pArg;
    va_start(pArg, format);

    int16_t textHeight = fontToDraw->getMinimumTextHeight();

    TextProvider textProvider;
    textProvider.initialize(format, pArg, fontToDraw->getGSUBTable());

    int16_t numLines = LCD::getNumLines(textProvider, wideTextAction, TEXT_ROTATE_0, fontToDraw, getWidth());

    va_end(pArg);
    return (textHeight + linespace > 0) ? (numLines * textHeight + (numLines - 1) * linespace) : (numLines > 0) ? (textHeight) : 0;
}
} // namespace touchgfx
