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

#include <touchgfx/containers/ModalBoxWindow.hpp>
#include <touchgfx/Color.hpp>

namespace touchgfx
{
ModalBoxWindow::ModalBoxWindow() :
	windowColor( Color::getColorFrom24BitRGB(255, 255, 255) ),
	windowBorderColor( Color::getColorFrom24BitRGB(0, 0, 0) ),
	windowBorderSize( 10 )
{
    Container::setWidth(HAL::DISPLAY_WIDTH);
    Container::setHeight(HAL::DISPLAY_HEIGHT);

    int defaultShadeAlpha = 96;
    colortype defaultShadeColor = Color::getColorFrom24BitRGB(0x0, 0x0, 0x0);

    backgroundShade.setPosition(0, 0, getWidth(), getHeight());
    backgroundShade.setColor(defaultShadeColor);
    backgroundShade.setTouchable(true);
    ModalBoxWindow::setShadeAlpha(defaultShadeAlpha);
    Container::add(backgroundShade);

    Container::add(windowContainer);
	
	windowBox.setColor(windowColor);
	windowBox.setBorderColor(windowBorderColor);
	windowBox.setBorderSize(windowBorderSize);
    windowContainer.add(windowBox);
}

ModalBoxWindow::~ModalBoxWindow()
{
}

void ModalBoxWindow::setWindowSize(const uint16_t width, const uint16_t height)
{
	windowBox.setPosition(0,0,width,height);
    windowContainer.setPosition((getWidth() - windowBox.getWidth()) / 2, (getHeight() - windowBox.getHeight()) / 2, windowBox.getWidth(), windowBox.getHeight());
    invalidate();
}
	
	
void ModalBoxWindow::setWindowColor(colortype color) 
{ 
    windowColor = color; 
	windowBox.setColor(windowColor);
	windowBox.invalidate();
}
	
void ModalBoxWindow::setWindowBorderColor(colortype color) 
{ 
	windowBorderColor = color; 
	windowBox.setBorderColor(windowBorderColor);
	windowBox.invalidate();
}
	
void ModalBoxWindow::setWindowBorderSize(uint8_t size) 
{ 
	windowBorderSize = size; 
	windowBox.setBorderSize(windowBorderSize);
	windowBox.invalidate();
}


void ModalBoxWindow::add(Drawable& d)
{
    windowContainer.add(d);
}

bool ModalBoxWindow::remove(Drawable& d)
{
    return windowContainer.remove(d);
}

void ModalBoxWindow::setShadeAlpha(uint8_t alpha)
{
    backgroundShade.setAlpha(alpha);
    backgroundShade.invalidate();
}

uint8_t ModalBoxWindow::getShadeAlpha() const
{
    return backgroundShade.getAlpha();
}

void ModalBoxWindow::setShadeColor(colortype color)
{
    backgroundShade.setColor(color);
    backgroundShade.invalidate();
}

touchgfx::colortype ModalBoxWindow::getShadeColor() const
{
    return backgroundShade.getColor();
}

void ModalBoxWindow::show()
{
    setVisible(true);
    invalidate();
}

void ModalBoxWindow::hide()
{
    setVisible(false);
    invalidate();
}

bool ModalBoxWindow::isShowing() const
{
    return isVisible();
}
}
