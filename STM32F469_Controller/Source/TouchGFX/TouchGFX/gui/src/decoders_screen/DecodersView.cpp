#include <gui/decoders_screen/DecodersView.hpp>
#include "Common.h"
#include "Decoders.h"


DecodersView::DecodersView()
{
    scrollWheelDecoders.setNumberOfItems(decoderCount);
	scrollWheelDecoders.animateToItem(activeDecoder,0);
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
