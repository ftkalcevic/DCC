#include <gui/preferences_screen/PreferencesView.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>
#include <stdio.h>
#include "Common.h"
#include "AudioTask.h"

enum EPreferencesButton
{
	Settings = 1,
	DCCSettings,
	DCCProgramming,
	DCCConfiguration,
	LCCConfig,
	AudioConfig,
	About
};

static struct SPreferencesButton
{
	const char16_t *label;
	BitmapId bmpId;
	EPreferencesButton id;
} buttonCfg[] = 
{
	{ u"Settings", BITMAP_PREFERENCESICON_ID, EPreferencesButton::Settings },
	{ u"DCC Settings", BITMAP_PREFERENCESICON_ID, EPreferencesButton::DCCSettings },
	{ u"DCC Config", BITMAP_LOCOICON_ID, EPreferencesButton::DCCConfiguration },
	{ u"LCC Config", BITMAP_LOCOICON_ID, EPreferencesButton::LCCConfig },
	{ u"Audio", BITMAP_SPKRICON_ID, EPreferencesButton::AudioConfig },
	{ u"About", BITMAP_ABOUTICON_ID, EPreferencesButton::About },
};

PreferencesView::PreferencesView():
    buttonClickCallback(this, &PreferencesView::buttonClickHandler)
{
	// Add buttons (15, 15) 200x200 spacing
	for(int i = 0 ; i < sizeof(buttonCfg) / sizeof(buttonCfg[0]) ; i++)
	{
		std::shared_ptr<touchgfx::CustomButton> button = std::make_shared<touchgfx::CustomButton>();
		buttons.push_back(button);
		button->setXY(15 + (i%4)*200, 15 + (i/4)*200);
		button->setLabelText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));
		button->setLabelText(buttonCfg[i].label);
		button->setBitmaps(touchgfx::Bitmap(BITMAP_BUTTONUP_ID), touchgfx::Bitmap(BITMAP_BUTTONDOWN_ID), touchgfx::Bitmap(buttonCfg[i].bmpId), touchgfx::Bitmap(buttonCfg[i].bmpId), false, false );
		button->setId(buttonCfg[i].id);
		button->setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
		button->setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
		button->setAction(buttonClickCallback);
		swipeContainerPage1.add(*button);
	}
}

void PreferencesView::setupScreen()
{
    PreferencesViewBase::setupScreen();
}

void PreferencesView::tearDownScreen()
{
    PreferencesViewBase::tearDownScreen();
}

void PreferencesView::buttonClickHandler(const touchgfx::AbstractButton& src)
{
	const touchgfx::CustomButton &button = (const touchgfx::CustomButton &)src;
	audioTask.PlaySound(EAudioSounds::KeyPressTone);
	
    if (button.getId() == Settings)
    {
        application().gotoSettingsScreen();
    }
    else if (button.getId() == DCCSettings)
    {
        application().gotoDCCSettingsScreen();
    }
    else if (button.getId() == DCCConfiguration)
    {
        application().gotoDCCConfigScreen();
    }
    else if (button.getId() == AudioConfig)
    {
        application().gotoAudioConfigScreen();
    }
    else if (button.getId() == About)
    {
        application().gotoAboutScreen();
    }
}








	static int count = 0;

void PreferencesView::handleClickEvent(const ClickEvent & evt)
{
	//printf("PreferencesView::%d click\n", count++);
	PreferencesViewBase::handleClickEvent(evt);
}
void PreferencesView::handleDragEvent(const DragEvent & evt)
{
	//printf("PreferencesView::%d drag\n", count++);
	PreferencesViewBase::handleDragEvent(evt);
}
void PreferencesView::handleGestureEvent(const GestureEvent & evt)
{
	//printf("PreferencesView::%d gesture %d %d\n", count++, evt.getType(), evt.getVelocity());
	if (evt.getType() == GestureEvent::SWIPE_VERTICAL && evt.getVelocity() < 0 && evt.getX() > EXITSCREEN_SWIPE_VELOCITY)
	{
		application().returnToMainScreen();
	}
	PreferencesViewBase::handleGestureEvent(evt); 
}
void PreferencesView::handleKeyEvent(uint8_t key)
{
	PreferencesViewBase::handleKeyEvent(key);
}
