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

#ifndef TEXTAREAWITHWILDCARDWITHBOX_HPP
#define TEXTAREAWITHWILDCARDWITHBOX_HPP

#include <touchgfx/TextProvider.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>

namespace touchgfx
{

class TextAreaWithWildcardWithBox : public TextAreaWithOneWildcard
{
public:
    TextAreaWithWildcardWithBox()
		: boxColor(touchgfx::Color::getColorFrom24BitRGB(127, 127, 127))
    {
    }
	
	virtual void draw(const Rect& invalidatedArea) const
	{
		Rect dirty = invalidatedArea;
		translateRectToAbsolute(dirty);
		if (!dirty.isEmpty())
			HAL::lcd().fillRect(dirty, boxColor, 255);
		TextAreaWithOneWildcard::draw(invalidatedArea);
	}
	
	void setBoxColor(colortype _boxColor)
	{
		boxColor = _boxColor;
	}
	
	virtual void setPosition(int16_t x, int16_t y, int16_t width, int16_t height)
	{
		rect = Rect(x, y, width, height);
		TextAreaWithOneWildcard::setPosition(x, y, width, height);
	}
	virtual const Rect getRect() const
	{
		return rect;
	}
private:
	colortype boxColor;
	Rect rect;
};
	
} // namespace touchgfx

#endif // TEXTAREAWITHWILDCARD_HPP
