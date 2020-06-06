#include <gui/dccconfig_screen/DCCConfigView.hpp>
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include <stdio.h>
#include "Common.h"

DCCConfigView::DCCConfigView() :
    buttonProgTrackClickCallback(this, &DCCConfigView::buttonProgTrackClickHandler),
    buttonScanTrackClickCallback(this, &DCCConfigView::buttonScanTrackClickHandler),
    buttonReadAllCVsClickCallback(this, &DCCConfigView::buttonReadAllCVsClickHandler)
{
	toggleProgTrack.setAction(buttonProgTrackClickCallback);
	
    buttonScanTrack.setXY(547, 107);
    buttonScanTrack.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_PRESSED_ID), touchgfx::Bitmap(BITMAP_ROUND_EDGE_MEDIUM_DISABLED_ID));
    buttonScanTrack.setLabelText(touchgfx::TypedText(T_RESOURCEIDSCANTRACK));
    buttonScanTrack.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    buttonScanTrack.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	buttonScanTrack.Enable(false);
	buttonScanTrack.setAction(buttonScanTrackClickCallback);
    scrollableContainer1.add(buttonScanTrack);

    buttonReadAllCVs.setXY(547, 185);
    buttonReadAllCVs.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_PRESSED_ID), touchgfx::Bitmap(BITMAP_ROUND_EDGE_MEDIUM_DISABLED_ID));
    buttonReadAllCVs.setLabelText(touchgfx::TypedText(T_RESOURCEREADALLCVS));
    buttonReadAllCVs.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    buttonReadAllCVs.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	buttonReadAllCVs.Enable(false);
	buttonReadAllCVs.setAction(buttonReadAllCVsClickCallback);
    scrollableContainer1.add(buttonReadAllCVs);
}

void DCCConfigView::setupScreen()
{
    DCCConfigViewBase::setupScreen();
}

void DCCConfigView::tearDownScreen()
{
    DCCConfigViewBase::tearDownScreen();
}

void DCCConfigView::buttonProgTrackClickHandler(const touchgfx::AbstractButton& src)
{
	bool trackEnabled = toggleProgTrack.getState();
	presenter->EnableProgTrack(trackEnabled);
	buttonScanTrack.Enable(trackEnabled);
	buttonReadAllCVs.Enable(trackEnabled);
}

void DCCConfigView::buttonScanTrackClickHandler(const touchgfx::AbstractButton& src)
{
	presenter->ScanProgrammingTrack();
}

void DCCConfigView::buttonReadAllCVsClickHandler(const touchgfx::AbstractButton& src)
{
}
	

void DCCConfigView::handleGestureEvent(const GestureEvent & evt)
{
	//printf("DCCConfigView gesture %d %d\n", evt.getType(), evt.getVelocity());
	if (evt.getType() == GestureEvent::SWIPE_VERTICAL && evt.getVelocity() < -SWIPE_VELOCITY )
	{
		application().gotoPreferencesSouth();
	}
	else
		DCCConfigViewBase::handleGestureEvent(evt); 
}
