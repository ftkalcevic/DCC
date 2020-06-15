#include <gui/main_screen/MainView.hpp>
#include "BitmapDatabase.hpp"
#include <gui/common/BinFileLoader.h>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>
#include <gui/common/CustomButton.hpp>
#include <stdio.h>
#include "Decoders.h"
#ifndef SIMULATOR
#include "AudioTask.h"
#endif

enum EConfigPages
{
	Preferences=1,
	Loco
};


/****************************************************************/

/****************************************************************/
int activeDecoder = 0;
int decoderCount=4;
Decoders decoders[4] =
{ 
	{	
		{.name =  u"Loco #0000" }, 
		{.description =  u"Marklin 8840 -  DB Class 140 Electric" }, 
		.address = 0, 
		.type = EDecoderType::Multifunction, 
		{ .UserIconFile = "" }, 
		{.UserBackgroundFile = ""}, 
		.loco = { 
			.speedSteps = ESpeedSteps::ss28, 
			.FL = EFrontLightPosition::SpeedDirInsBit4 }, 
		.func = { 
			.Fn1 = EFunctionAction::FrontLight, 
			.Fn2 = EFunctionAction::None, 
			.Fn3 = EFunctionAction::None, 
			.Fn4 = EFunctionAction::None, 
			.Fn5 = EFunctionAction::None } 
	},
	{	
		{.name =  u"Loco #0001" }, 
		.address = 1, 
		.type = EDecoderType::Multifunction, 
		{ .UserIconFile = "" }, 
		{.UserBackgroundFile = ""}, 
		.loco = { 
			.speedSteps = ESpeedSteps::ss28, 
			.FL = EFrontLightPosition::SpeedDirInsBit4 }, 
		.func = { 
			.Fn1 = EFunctionAction::FrontLight, 
			.Fn2 = EFunctionAction::None, 
			.Fn3 = EFunctionAction::None, 
			.Fn4 = EFunctionAction::None, 
			.Fn5 = EFunctionAction::None } 
	},
	{	{.name =  u"Switch #10" }, 
		{.description =  u"Turnout to Odek Enterprises" }, 
		.address = 10, 
		.type = EDecoderType::Accessory, 
		{ .UserIconFile = "" }, 
		{ .UserBackgroundFile = "" }, 
		.func = { 
			.Fn1 = EFunctionAction::ToggleSwitch, 
			.Fn2 = EFunctionAction::None, 
			.Fn3 = EFunctionAction::None, 
			.Fn4 = EFunctionAction::None, 
			.Fn5 = EFunctionAction::None }  
	},		
	{	{.name =  u"Switch #11" }, 
		.address = 11, 
		.type = EDecoderType::Accessory, 
		{ .UserIconFile = "" }, 
		{ .UserBackgroundFile = "" }, 
		.func = { 
			.Fn1 = EFunctionAction::ToggleSwitch, 
			.Fn2 = EFunctionAction::None, 
			.Fn3 = EFunctionAction::None, 
			.Fn4 = EFunctionAction::None, 
			.Fn5 = EFunctionAction::None }  
	}		
};
	

MainView::MainView():
    buttonClickCallback(this, &MainView::buttonClickHandler)
{
	bmpId = BinFileLoader::makeBitmap("/Images/background"); 
	if (bmpId != BITMAP_INVALID)
	{
		backgroundImage.setBitmap(Bitmap(bmpId));
	}
	
	pages = new touchgfx::Container *[(decoderCount + 1)/8+1];
	buttons = new touchgfx::CustomButton * [decoderCount + 1];
	
	touchgfx::Container *page = NULL;
	for (int i = 0; i < decoderCount + 1; i++)
	{
		if ((i % 8) == 0)
		{
			// New page for every 8 icons		
			page = new touchgfx::Container();
			page->setWidth(800);
			page->setHeight(430);
			swipeContainer.add(*page);
			pages[i%8] = page;
		}

		// Add buttons (15, 15) 200x200 spacing
		touchgfx::CustomButton *button = new touchgfx::CustomButton();
		buttons[i] = button;
		button->setXY(15 + (i%4) * 200, 15 + ((i%8)/4)*200);
		button->setLabelText(touchgfx::TypedText(T_WILDCARDTEXTID));
		button->setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
		button->setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
		button->setAction(buttonClickCallback);
		
		if (i == decoderCount)	// Last icons is always the preferences button
		{
			button->setBitmaps(touchgfx::Bitmap(BITMAP_BUTTONUP_ID), touchgfx::Bitmap(BITMAP_BUTTONDOWN_ID), touchgfx::Bitmap(BITMAP_PREFERENCESICON_ID), touchgfx::Bitmap(BITMAP_PREFERENCESICON_ID));
			button->setLabelText("Preferences");
			button->setId(i);
		}
		else
		{
			if ( decoders[i].type == EDecoderType::Multifunction )
				button->setBitmaps(touchgfx::Bitmap(BITMAP_BUTTONUP_ID), touchgfx::Bitmap(BITMAP_BUTTONDOWN_ID), touchgfx::Bitmap(BITMAP_LOCOICON_ID), touchgfx::Bitmap(BITMAP_LOCOICON_ID));
			else
				button->setBitmaps(touchgfx::Bitmap(BITMAP_BUTTONUP_ID), touchgfx::Bitmap(BITMAP_BUTTONDOWN_ID), touchgfx::Bitmap(BITMAP_ACCESSORYICON_ID), touchgfx::Bitmap(BITMAP_ACCESSORYICON_ID));
			button->setLabelText( decoders[i].name);
			button->setId(i);
		}
		page->add(*button);
	}
	swipeContainer.setSelectedPage(0);
}

void MainView::deletePage(Drawable& d)
{
	touchgfx::Container *page = (touchgfx::Container *)&d;
	delete page;
}

MainView::~MainView()
{
	if (bmpId != BITMAP_INVALID)
	{
		Bitmap::dynamicBitmapDelete(bmpId);
		bmpId = BITMAP_INVALID;
	}

	for (int i = 0; i < decoderCount + 1; i++)
		delete buttons[i];
	delete[] buttons;
	
	for (int i = 0; i < (decoderCount + 1)/8+1; i++)
		delete pages[i];
	delete [] pages;
}

void MainView::setupScreen() 
{

}

void MainView::tearDownScreen()
{
}


void MainView::buttonClickHandler(const touchgfx::AbstractButton& src)
{
	const touchgfx::CustomButton &button = (const touchgfx::CustomButton &)src;

    if (button.getId() == decoderCount)		// Last index is the preferences button
    {
        application().gotoPreferences();
    }
	else
	{
		activeDecoder = button.getId();
        application().gotoDecodersScreen();
	}
}



	static int count = 0;

void MainView::handleClickEvent(const ClickEvent & evt)
{
	printf("%d click\n", count++);
	MainViewBase::handleClickEvent(evt);
}
void MainView::handleDragEvent(const DragEvent & evt)
{
	printf("%d drag\n", count++);
	MainViewBase::handleDragEvent(evt);
}
void MainView::handleGestureEvent(const GestureEvent & evt)
{
	printf("%d gesture %d %d\n", count++, evt.getType(), evt.getVelocity());
	MainViewBase::handleGestureEvent(evt);
}
void MainView::handleKeyEvent(uint8_t key)
{
	printf("%d key %d\n", count++, key);
	MainViewBase::handleKeyEvent(key);
}

