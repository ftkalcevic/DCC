#include <gui/main_screen/MainView.hpp>
#include "BitmapDatabase.hpp"
#include <gui/common/BinFileLoader.h>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>
#include <gui/common/CustomButton.hpp>
#include <stdio.h>
#include "DecodersConfig.h"
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
	
	pages.reserve( (uiDecodersConfig.Count() + 1)/8+1 );
	buttons.reserve( uiDecodersConfig.Count() + 1 );
	
	std::shared_ptr<touchgfx::Container> page;
	for (int i = 0; i < uiDecodersConfig.Count() + 1; i++)
	{
		if ((i % 8) == 0)
		{
			// New page for every 8 icons		
			page = std::make_shared<touchgfx::Container>();
			page->setWidth(800);
			page->setHeight(430);
			swipeContainer.add(*page);
			pages.push_back(page);
		}

		// Add buttons (15, 15) 200x200 spacing
		std::shared_ptr<touchgfx::CustomButton> button = std::make_shared<touchgfx::CustomButton>();
		buttons.push_back(button);
		button->setXY(15 + (i%4) * 200, 15 + ((i%8)/4)*200);
		button->setLabelText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));
		button->setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
		button->setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
		button->setAction(buttonClickCallback);
		
		if (i == uiDecodersConfig.Count())	// Last icons is always the preferences button
		{
			button->setBitmaps(touchgfx::Bitmap(BITMAP_BUTTONUP_ID), touchgfx::Bitmap(BITMAP_BUTTONDOWN_ID), touchgfx::Bitmap(BITMAP_PREFERENCESICON_ID), touchgfx::Bitmap(BITMAP_PREFERENCESICON_ID));
			button->setLabelText("Preferences");
			button->setId(i);
		}
		else
		{
			Decoders &d = uiDecodersConfig[i];
		
			if (d.getSmallImageType() == EUserImage::UserFile)
			{
				button->setBitmaps(touchgfx::Bitmap(BITMAP_BUTTONUP_ID), touchgfx::Bitmap(BITMAP_BUTTONDOWN_ID), touchgfx::Bitmap(BITMAP_LOCOICON_ID), touchgfx::Bitmap(BITMAP_LOCOICON_ID));
			}
			else
			{
				BitmapId bmpId = EUserImageClass::UserImage(d.getSmallImageType()).bmpId;
				button->setBitmaps(touchgfx::Bitmap(BITMAP_BUTTONUP_ID), touchgfx::Bitmap(BITMAP_BUTTONDOWN_ID), touchgfx::Bitmap(bmpId), touchgfx::Bitmap(bmpId));
			}
			button->setLabelText( d.getName());
			button->setId(i);
		}
		page->add(*button);
	}
	swipeContainer.setSelectedPage(0);
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
	audioTask.PlaySound(EAudioSounds::KeyPressTone);
	
    if (button.getId() == uiDecodersConfig.Count())		// Last index is the preferences button
    {
        application().gotoPreferences();
    }
	else
	{
		uiDecodersConfig.setActiveDecoder(button.getId());
        application().gotoDecodersScreen();
	}
}

//
//
//	static int count = 0;
//
//void MainView::handleClickEvent(const ClickEvent & evt)
//{
//	printf("%d click (%d,%d):%d\n", count++, evt.getX(), evt.getY(), evt.getForce() );
//	MainViewBase::handleClickEvent(evt);
//}
//void MainView::handleDragEvent(const DragEvent & evt)
//{
//	printf("%d drag d(%d,%d) n(%d,%d) o(%d,%d)\n", count++, evt.getDeltaX(), evt.getDeltaY(), evt.getNewX(), evt.getNewY(), evt.getOldX(), evt.getOldY());
//	MainViewBase::handleDragEvent(evt);
//}
//void MainView::handleGestureEvent(const GestureEvent & evt)
//{
//	printf("%d gesture %d %d (%d,%d)\n", count++, evt.getType(), evt.getVelocity(), evt.getX(), evt.getX());
//	MainViewBase::handleGestureEvent(evt);
//}
//void MainView::handleKeyEvent(uint8_t key)
//{
//	printf("%d key %d\n", count++, key);
//	MainViewBase::handleKeyEvent(key);
//}
//
