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

#ifndef BUTTONWITHLABELANDENABLE_HPP
#define BUTTONWITHLABELANDENABLE_HPP

#include <touchgfx/widgets/ButtonWithLabel.hpp>
#include <touchgfx/TypedText.hpp>

namespace touchgfx
{

class ButtonWithLabelAndEnable : public ButtonWithLabel
{
public:

    ButtonWithLabelAndEnable();

    virtual void draw(const Rect& area) const;
	void setBitmaps(const Bitmap& newBackgroundReleased, const Bitmap& newBackgroundPressed, const Bitmap& newBackgroundDisabled);
	void Enable(bool enable) {buttonEnabled = enable; invalidate();}
	bool isEnabled() const { return buttonEnabled;}
	virtual void handleClickEvent(const ClickEvent & event);
protected:
	Bitmap  backgroundDisabled;
	bool buttonEnabled;
};
} // namespace touchgfx

#endif // BUTTONWITHLABELANDENABLE_HPP
