#include <gui/about_screen/AboutView.hpp>


const char16_t *aboutText = 
uR"(STM32F469 Disco DCC LCC
Model Railway Controller
Copyright )" u"\xA9" uR"( 2020 Frank Tkalcevic 
www.franksworkshop.com.au

This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions


Code Libraries or snippets:
yxml: Copyright )" u"\xA9" uR"( 2013-2014 Yoran Heling
FatFs: Copyright )" u"\xA9" uR"( 2017, ChaN, all right reserved.
FreeRTOS Kernel: Copyright )" u"\xA9" uR"( 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
OpenTX: Copyright )" u"\xA9" uR"( OpenTX
BSP, HAL, TouchGFX: COPYRIGHT )" u"\xA9" uR"( 2017 STMicroelectronics
Line Clipping: Copyright )" u"\xA9" uR"( Eric Richards http://www.richardssoftware.net/2014/07/clipping-lines-to-rectangle-using-cohen.html
Line drawing: https://gist.github.com/bert/1085538
OpenLCB: 

Artwork:
background.png: https://wallpaperaccess.com/train
Splash.png: https://www.pinterest.com.au/pin/320177854738516822/
locoIcon.png: Icons made by Freepik from www.flaticon.com
DieselIcon.png: Based on 'Locomotive Icon #7964' https://icon-library.net/icon/locomotive-icon-17.html
spkrIcon.png: https://commons.wikimedia.org/wiki/File:Speaker_Icon.svg
)";
	
AboutView::AboutView()
{
	textArea1.setWildcard((Unicode::UnicodeChar*)aboutText); 
	const int MARGIN = 5;
	if (textArea1.getHeight()+2*MARGIN > backgroundImage.getHeight())
	{
		backgroundImage.setHeight(textArea1.getHeight()+2*MARGIN);
	}
}

void AboutView::setupScreen()
{
    AboutViewBase::setupScreen();
}

void AboutView::tearDownScreen()
{
    AboutViewBase::tearDownScreen();
}

void AboutView::handleGestureEvent(const GestureEvent & evt)
{
	//printf("PreferencesView::%d gesture %d %d\n", count++, evt.getType(), evt.getVelocity());
	if (evt.getType() == GestureEvent::SWIPE_VERTICAL && evt.getVelocity() < -10 && evt.getX() > 250)
	{
		application().returnToPreferences();
	}
	else
		AboutViewBase::handleGestureEvent(evt); 
}