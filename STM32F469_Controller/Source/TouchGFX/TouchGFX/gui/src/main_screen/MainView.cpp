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

MainView::MainView():
    buttonClickCallback(this, &MainView::buttonClickHandler)
{
	bmpId = BinFileLoader::makeBitmap("/Images/background"); 
	if (bmpId != BITMAP_INVALID)
	{
		backgroundImage.setBitmap(Bitmap(bmpId));
	}
	
	touchgfx::Container *page = new touchgfx::Container();
	page->setWidth(800);
    page->setHeight(430);
    swipeContainer.add(*page);
    swipeContainer.setSelectedPage(0);
	
	// Add buttons (15, 15) 200x200 spacing
	touchgfx::CustomButton *button = new touchgfx::CustomButton();
	button->setXY(15, 15);
    button->setBitmaps(touchgfx::Bitmap(BITMAP_BUTTONUP_ID), touchgfx::Bitmap(BITMAP_BUTTONDOWN_ID), touchgfx::Bitmap(BITMAP_PREFERENCESICON_ID), touchgfx::Bitmap(BITMAP_PREFERENCESICON_ID));
	//button->setIconXY(4, 4);
    button->setLabelText(touchgfx::TypedText(T_WILDCARDTEXTID));
    button->setLabelText("Preferences");
	button->setId(Preferences);
    button->setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    button->setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    button->setAction(buttonClickCallback);
    page->add(*button);
	
	
	button = new touchgfx::CustomButton();
	button->setXY(215, 15);
    button->setBitmaps(touchgfx::Bitmap(BITMAP_BUTTONUP_ID), touchgfx::Bitmap(BITMAP_BUTTONDOWN_ID), touchgfx::Bitmap(BITMAP_LOCOICON_ID), touchgfx::Bitmap(BITMAP_LOCOICON_ID));
	//button->setIconXY(15, 6);
    button->setLabelText(touchgfx::TypedText(T_WILDCARDTEXTID));
    button->setLabelText("Loco #3456");
	button->setId(Loco);
    button->setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    button->setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    button->setAction(buttonClickCallback);
    page->add(*button);
	
	
	
}

MainView::~MainView()
{
	if (bmpId != BITMAP_INVALID)
	{
		Bitmap::dynamicBitmapDelete(bmpId);
		bmpId = BITMAP_INVALID;
	}
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

    if (button.getId() == Preferences)
    {
        //Interaction1
        //When button1 clicked change screen to Preferences
        //Go to Preferences with screen transition towards East
        application().gotoPreferencesNorth();
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

