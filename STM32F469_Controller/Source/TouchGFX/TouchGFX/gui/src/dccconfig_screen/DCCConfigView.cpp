#include <gui/dccconfig_screen/DCCConfigView.hpp>
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include <stdio.h>
#include "Common.h"
#include "DecodersConfig.h"
#include "DecodersConfig.h"


DCCConfigView::DCCConfigView() :
    buttonProgTrackClickCallback(this, &DCCConfigView::buttonProgTrackClickHandler),
    buttonScanTrackClickCallback(this, &DCCConfigView::buttonScanTrackClickHandler),
    buttonReadAllCVsClickCallback(this, &DCCConfigView::buttonReadAllCVsClickHandler),
	editTextClickHandlerCallback(this, &DCCConfigView::editTextClickHandler),
    closeKeypadWindowCallback(this, &DCCConfigView::closeKeypadWindowHandler),
    closeKeyboardWindowCallback(this, &DCCConfigView::closeKeyboardWindowHandler),
    scrollWheelDecodersClickCallback(this, &DCCConfigView::scrollWheelDecodersClickHandler),
	state(Editting),
	selectedDecoderItem(-1)
{
	scrollWheelDecoders.setClickAction(scrollWheelDecodersClickCallback);
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
	
	// Decoder
	int16_t yPos = 275, x1 = 17, x2 = 417;
    textAreaLabelDecoder.setXY(x1, yPos);
    textAreaLabelDecoder.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textAreaLabelDecoder.setLinespacing(0);
    textAreaLabelDecoder.setTypedText(touchgfx::TypedText(T_DCCCONFIGDECODER));
    scrollableContainer1.add(textAreaLabelDecoder);

    boxDecoder.setPosition(x2, yPos, 360, 50);
    boxDecoder.setColor(touchgfx::Color::getColorFrom24BitRGB(163, 163, 163));
	boxDecoder.setClickAction(editTextClickHandlerCallback);
    scrollableContainer1.add(boxDecoder);

	
	// Address	
	yPos += 60;
    textAreaLabelAddress.setXY(x1, yPos);
    textAreaLabelAddress.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textAreaLabelAddress.setLinespacing(0);
    textAreaLabelAddress.setTypedText(touchgfx::TypedText(T_DCCCONFIGADDRESS));
    scrollableContainer1.add(textAreaLabelAddress);
	
    boxAddress.setPosition(x2, yPos, 100, 50);
    boxAddress.setColor(touchgfx::Color::getColorFrom24BitRGB(163, 163, 163));
	boxAddress.setClickAction(editTextClickHandlerCallback);
    scrollableContainer1.add(boxAddress);

    textAddress.setPosition(x2, yPos, 100, 50);
    textAddress.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textAddress.setLinespacing(0);
    textAddress.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT40PXNUMERICID));
    scrollableContainer1.add(textAddress);

	// Name
	yPos += 60;
    textAreaLabelName.setXY(x1, yPos);
    textAreaLabelName.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textAreaLabelName.setLinespacing(0);
    textAreaLabelName.setTypedText(touchgfx::TypedText(T_DCCCONFIGNAME));
    scrollableContainer1.add(textAreaLabelName);

    boxName.setPosition(x2, yPos, 360, 50);
    boxName.setColor(touchgfx::Color::getColorFrom24BitRGB(163, 163, 163));
	boxName.setClickAction(editTextClickHandlerCallback);
    scrollableContainer1.add(boxName);

    textName.setPosition(x2, yPos, 360, 50);
    textName.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textName.setLinespacing(0);
    textName.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTCENTERED40PXID));
    scrollableContainer1.add(textName);

	// Description
	yPos += 60;
    textAreaLabelDescription.setXY(x1, yPos);
    textAreaLabelDescription.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textAreaLabelDescription.setLinespacing(0);
    textAreaLabelDescription.setTypedText(touchgfx::TypedText(T_DCCCONFIGDESCRIPTION));
    scrollableContainer1.add(textAreaLabelDescription);

    boxDescription.setPosition(x2, yPos, 360, 50);
    boxDescription.setColor(touchgfx::Color::getColorFrom24BitRGB(163, 163, 163));
	boxDescription.setClickAction(editTextClickHandlerCallback);
    scrollableContainer1.add(boxDescription);

    textDescription.setPosition(x2, yPos, 360, 50);
    textDescription.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textDescription.setLinespacing(0);
    textDescription.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTCENTER20PXID));
    scrollableContainer1.add(textDescription);

	// Config
	yPos += 60;
	uint16_t yConfig = 10 + 6*65 + 10;
    textAreaLabelConfig.setXY(x1, yPos);
    textAreaLabelConfig.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textAreaLabelConfig.setLinespacing(0);
    textAreaLabelConfig.setTypedText(touchgfx::TypedText(T_DCCCONFIGCONFIG));
    scrollableContainer1.add(textAreaLabelConfig);

    boxConfig.setPosition(x2, yPos, 360, yConfig);
    boxConfig.setColor(touchgfx::Color::getColorFrom24BitRGB(163, 163, 163));
    scrollableContainer1.add(boxConfig);

	
	chkMFDirection.setPosition(x2+10, yPos + 10 + 65*0, 340, 65 );
	chkMFFLLocation.setPosition(x2+10, yPos + 10 + 65*1, 340, 65 );
	chkMFPowerSourceConversion.setPosition(x2+10, yPos + 10 + 65*2, 340, 65 );
	chkMFBiDirectionalComms.setPosition(x2+10, yPos + 10 + 65*3, 340, 65 );
	chkMFSpeedTable.setPosition(x2+10, yPos + 10 + 65*4, 340, 65 );
	chkMFTwoByteAddressing.setPosition(x2+10, yPos + 10 + 65*5, 340, 65 );
	chkMFDirection.setText(u"Normal Direction of Travel");
	chkMFFLLocation.setText(u"Advanced Speed Control");
	chkMFPowerSourceConversion.setText(u"Analog Mode Conversion");
	chkMFBiDirectionalComms.setText(u"Bi-direction Communications");
	chkMFSpeedTable.setText(u"Speed Table");
	chkMFTwoByteAddressing.setText(u"Two Byte Addressing");
	chkMFDirection.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));
	chkMFFLLocation.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));
	chkMFPowerSourceConversion.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));
	chkMFBiDirectionalComms.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));
	chkMFSpeedTable.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));
	chkMFTwoByteAddressing.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));

	chkAccBiDirectionalComms.setPosition(x2+10, yPos + 10 + 65*0, 340, 65 );;
	chkAccType.setPosition(x2+10, yPos + 10 + 65*1, 340, 65 );
	chkAccAddressMethod.setPosition(x2+10, yPos + 10 + 65*2, 340, 65 );
	chkAccBiDirectionalComms.setText(u"Bi-direction Communications");
	chkAccType.setText(u"Extended Type");
	chkAccAddressMethod.setText(u"Advanced Addressing");
	chkAccBiDirectionalComms.setVisible(false);
	chkAccType.setVisible(false);
	chkAccAddressMethod.setVisible(false);
	chkAccBiDirectionalComms.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));
	chkAccType.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));
	chkAccAddressMethod.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));

	scrollableContainer1.add(chkMFDirection);
	scrollableContainer1.add(chkMFFLLocation);
	scrollableContainer1.add(chkMFPowerSourceConversion);
	scrollableContainer1.add(chkMFBiDirectionalComms);
	scrollableContainer1.add(chkMFSpeedTable);
	scrollableContainer1.add(chkMFTwoByteAddressing);
	scrollableContainer1.add(chkAccBiDirectionalComms);
	scrollableContainer1.add(chkAccType);
	scrollableContainer1.add(chkAccAddressMethod);
	
	// All CVs
	yPos += yConfig + 10;
    textAreaLabelAllCVs.setXY(x1, yPos);
    textAreaLabelAllCVs.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textAreaLabelAllCVs.setLinespacing(0);
    textAreaLabelAllCVs.setTypedText(touchgfx::TypedText(T_DCCCONFIGALLCVS));
	scrollableContainer1.add(textAreaLabelAllCVs);

	yPos += 60 + 20;
    backgroundImage.setPosition(0, 0, 800, yPos);
	
    scrollableContainer1.setScrollbarsPermanentlyVisible();
    scrollableContainer1.setScrollbarsVisible(false);
	
	numericKeypad.setVisible(false);
	numericKeypad.setCloseWindowCallback(closeKeypadWindowCallback);
	add(numericKeypad);
	
	keyboard.setVisible(false);
	keyboard.setCloseWindowCallback(closeKeyboardWindowCallback);
	add(keyboard);
	
	// scroll wheel contains all decoders, plus 1 row for <new>
	scrollWheelDecoders.setNumberOfItems(uiDecodersConfig.Count() + 1);
	
	displayDecoder();
}

void DCCConfigView::setupScreen()
{
    DCCConfigViewBase::setupScreen();
}

void DCCConfigView::tearDownScreen()
{
    DCCConfigViewBase::tearDownScreen();
}

const int16_t FINGER_WIGGLE_SPACE = 15;
const TickType_t FINGER_HOLD_TIME = 750;

// Use tap and hold to select a decoder
void DCCConfigView::scrollWheelDecodersClickHandler(const touchgfx::ScrollWheel& src, const ClickEvent& evt)
{
	if (evt.getType() == ClickEvent::PRESSED)
	{
		// Remember when and where we clicked.
		selectStartX = evt.getX();
		selectStartY = evt.getY();
		selectStartTime = xTaskGetTickCount();
	}
	else if (evt.getType() == ClickEvent::RELEASED)
	{
		printf("%d, %d\n", selectStartX - evt.getX(), selectStartY - evt.getY());
		if ( (abs(selectStartX - evt.getX()) < FINGER_WIGGLE_SPACE) &&
		     (abs(selectStartY - evt.getY()) < FINGER_WIGGLE_SPACE) &&
			 (xTaskGetTickCount() - selectStartTime > pdMS_TO_TICKS(FINGER_HOLD_TIME)) )
		{
			//printf("Select item %d", scrollWheelDecoders.getSelectedItem());
			//scrollWheelDecoders.animateToItem(n,0);
			selectedDecoderItem = scrollWheelDecoders.getSelectedItem();

			if (selectedDecoderItem >= uiDecodersConfig.Count())
			{
				// <New> selected.  <New> becomes the new item.  Add another row.
				uiDecodersConfig.newDecoder();
				scrollWheelDecoders.setNumberOfItems(uiDecodersConfig.Count() + 1);
				scrollWheelDecoders.itemChanged(selectedDecoderItem);
				scrollWheelDecoders.invalidate();
			}
			displayDecoder();
		}
		
	}
	//printf("DCCConfigView::scrollWheelDecodersClickHandler %s\n", RELEASED ? "Release" : "Press");
}

void DCCConfigView::displayDecoder()
{
	if (selectedDecoderItem >= 0 && selectedDecoderItem < uiDecodersConfig.Count())
	{
		Decoders &d = uiDecodersConfig[selectedDecoderItem];
		Unicode::snprintf(addressTextBuffer, countof(addressTextBuffer), (const Unicode::UnicodeChar *)u"%04d", d.address);
		textAddress.setWildcard(addressTextBuffer);
		textName.setWildcard((const Unicode::UnicodeChar *)d.name);
		textDescription.setWildcard((const Unicode::UnicodeChar *)d.description);
	}
	else
	{
		textAddress.setWildcard((const Unicode::UnicodeChar *)u"----");
		textName.setWildcard((const Unicode::UnicodeChar *)u"----");
		textDescription.setWildcard((const Unicode::UnicodeChar *)u"");
	}
	textAddress.invalidate();
	textName.invalidate();
	textDescription.invalidate();
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
	state = Scanning;
	presenter->ScanProgrammingTrack();
}

void DCCConfigView::buttonReadAllCVsClickHandler(const touchgfx::AbstractButton& src)
{
}

void DCCConfigView::editTextClickHandler(const Box& b, const ClickEvent& evt)
{
	if (state == Editting)
	{
	    if (&b == static_cast<const Box *>(&boxAddress) && evt.getType() == ClickEvent::RELEASED )
	    {
		    printf("clicked address\n");
		    numericKeypad.setTitle((const Unicode::UnicodeChar *)u"Address");
            numericKeypad.setPosition(0, 0, 800, 480);
		    numericKeypad.setNumber(0);
		    numericKeypad.setRange(0, 9999);
		    numericKeypad.setVisible(true);
		    numericKeypad.invalidate();
		    state = Keypad;
	    }
	    else if (&b == static_cast<const Box *>(&boxName) && evt.getType() == ClickEvent::RELEASED )
	    {
		    printf("clicked name\n");
            keyboard.setPosition(0, 0, 800, 480);
		    keyboard.setTitle((const Unicode::UnicodeChar *)u"Name");
		    keyboard.setText((const Unicode::UnicodeChar *)u"Decoder name");
		    keyboard.setVisible(true);
		    keyboard.invalidate();
		    state = Keyboard;
	    }
	}
}

void DCCConfigView::closeKeypadWindowHandler(bool success)
{
	state = Editting;
	numericKeypad.setVisible(false);
	numericKeypad.invalidate();
	if (success)
	{
		uint16_t newValue = numericKeypad.getNumber();
		// put it back where?
	}
}

void DCCConfigView::closeKeyboardWindowHandler(bool success)
{
	state = Editting;
	keyboard.setVisible(false);
	keyboard.invalidate();
	if (success)
	{
		//const Unicode::UnicodeChar * = keyboard.buffer;
		// put it back where?
	}
}

void DCCConfigView::ScanTrackReply(int16_t address, int16_t config, int16_t extendedAddress, int16_t manufacturer, int16_t version)
{
	if ( state == Scanning )
	{
		if (address < 0)
		{
			
		}
		// If this matches an existing configuration, load it up.
	}
}


void DCCConfigView::handleGestureEvent(const GestureEvent & evt)
{
	if (state == Keyboard || state == Keypad)
		return;
	
	//printf("DCCConfigView gesture %d %d\n", evt.getType(), evt.getVelocity());
	if (evt.getType() == GestureEvent::SWIPE_VERTICAL && evt.getVelocity() < -SWIPE_VELOCITY )
	{
		application().returnToPreferences();
	}
	else if(scrollWheelDecoders.getRect().intersect(evt.getX(), evt.getY()))
	{
		scrollWheelDecoders.handleGestureEvent(evt);
	}
	else
	{
		DCCConfigViewBase::handleGestureEvent(evt); 
	}
}


void DCCConfigView::handleDragEvent(const DragEvent & evt)
{
	if (scrollWheelDecoders.getRect().intersect(evt.getOldX(), evt.getOldY()))
	{
		scrollWheelDecoders.handleDragEvent(evt);
	}
	else
	{
		DCCConfigViewBase::handleDragEvent(evt); 
	}
}


void DCCConfigView::scrollWheelDecodersUpdateItem(ListItemDecoder& item, int16_t itemIndex)
{
	item.setIndex(itemIndex, itemIndex == selectedDecoderItem);
}
