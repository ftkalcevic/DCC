#include <gui/main_screen/MainView.hpp>
#include "BitmapDatabase.hpp"
#include <gui/common/BinFileLoader.h>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>
#include <gui/common/CustomButton.hpp>
#include <stdio.h>
#ifndef SIMULATOR
#include "AudioTask.h"
#endif

enum EConfigPages
{
	Preferences=1,
	Loco
};


/****************************************************************/
const int MAX_PATH = 64;

// Enums are assigned values as these are used in config file.
enum EDecoderType: uint8_t
{
	Multifunction=0,
	Accessory=1
};
enum ESpeedSteps: uint8_t
{
	ss14=0,
	ss28=1,
	ss128=2
};

enum FrontLightPosition: uint8_t
{
	SpeedDirInsBit4=0,		
	FuncGroupOneBit4=1
};

struct LocoSettings 
{
	ESpeedSteps speedSteps;
	FrontLightPosition FL;
};

struct AccessorySettings
{
	
};


enum FunctionAction
{
	None = 0,
	FrontLight = 1,
	ToggleSwitch,
};

struct Functions
{
	FunctionAction Fn1, Fn2, Fn3, Fn4, Fn5;
};
struct Decoders
{
	char16_t name[20];
	uint16_t address;
	// model file
	EDecoderType type;
	char UserIconFile[MAX_PATH];
	char UserBackgroundFile[MAX_PATH];
	union
	{
		LocoSettings loco;
		AccessorySettings accessory;
	};

	Functions func;
};

/****************************************************************/
static int decoderCount=2;
static Decoders decoders[2] =
{ 
	{	{.name =  u"Loco #0000" }, 
		.address = 0, 
		.type = EDecoderType::Multifunction, 
		{ .UserIconFile = "" }, 
		{.UserBackgroundFile = ""}, 
		.loco = { 
			.speedSteps = ESpeedSteps::ss28, 
			.FL = FrontLightPosition::SpeedDirInsBit4 }, 
		.func = { 
			.Fn1 = FunctionAction::FrontLight, 
			.Fn2 = FunctionAction::None, 
			.Fn3 = FunctionAction::None, 
			.Fn4 = FunctionAction::None, 
			.Fn5 = FunctionAction::None } 
	},
	{	{.name =  u"Switch #1" }, 
		.address = 10, 
		.type = EDecoderType::Accessory, 
		{ .UserIconFile = "" }, 
		{ .UserBackgroundFile = "" }, 
		.func = { 
			.Fn1 = FunctionAction::ToggleSwitch, 
			.Fn2 = FunctionAction::None, 
			.Fn3 = FunctionAction::None, 
			.Fn4 = FunctionAction::None, 
			.Fn5 = FunctionAction::None }  
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
}



	static int count = 0;

void MainView::handleClickEvent(const ClickEvent & evt)
{
	//printf("%d click\n", count++);
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
	MainViewBase::handleKeyEvent(key);
}

