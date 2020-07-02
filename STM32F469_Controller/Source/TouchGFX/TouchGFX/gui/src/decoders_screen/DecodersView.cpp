#include <gui/decoders_screen/DecodersView.hpp>
#include "Common.h"
#include "DecodersConfig.h"


DecodersView::DecodersView() :
    takeControlCallback(this, &DecodersView::takeControlHandler)
{
    scrollWheelDecoders.setNumberOfItems(uiDecodersConfig.Count());
	scrollWheelDecoders.animateToItem(uiDecodersConfig.getActiveDecoder(),0);
	
	for ( int i = 0; i < scrollWheelDecodersListItems.getNumberOfDrawables(); i++ )
		scrollWheelDecodersListItems[i].setTakeControlCallback(takeControlCallback);
}

void DecodersView::setupScreen()
{
    DecodersViewBase::setupScreen();
}

void DecodersView::tearDownScreen()
{
    DecodersViewBase::tearDownScreen();
}


void DecodersView::scrollWheelDecodersUpdateItem(Decoder& item, int16_t itemIndex)
{
	item.setIndex(itemIndex);
}


void DecodersView::handleGestureEvent(const GestureEvent & evt)
{
	//printf("DCCConfigView gesture %d %d\n", evt.getType(), evt.getVelocity());
	if (evt.getType() == GestureEvent::SWIPE_VERTICAL && evt.getVelocity() < -SWIPE_VELOCITY )
	{
		application().returnToMainScreen();
	}
	else
		DecodersViewBase::handleGestureEvent(evt); 
}

void DecodersView::takeControlHandler(int decoderIndex, bool control)
{
	presenter->TakeControl(decoderIndex, control);
}
