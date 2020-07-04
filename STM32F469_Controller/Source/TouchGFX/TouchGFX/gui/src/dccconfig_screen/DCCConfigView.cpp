#include <gui/dccconfig_screen/DCCConfigView.hpp>
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include <stdio.h>
#include "Common.h"
#include "DecodersConfig.h"
#include "AudioTask.h"
#include "dcc.h"
#include "ProgTrackDCC.h"		// this should be in model



DCCConfigView::DCCConfigView() :
    buttonProgTrackClickCallback(this, &DCCConfigView::buttonProgTrackClickHandler),
    buttonScanTrackClickCallback(this, &DCCConfigView::buttonScanTrackClickHandler),
    buttonReadAllCVsClickCallback(this, &DCCConfigView::buttonReadAllCVsClickHandler),
	editTextClickHandlerCallback(this, &DCCConfigView::editTextClickHandler),
    closeKeypadWindowCallback(this, &DCCConfigView::closeKeypadWindowHandler),
    closeKeyboardWindowCallback(this, &DCCConfigView::closeKeyboardWindowHandler),
    scrollWheelDecodersClickCallback(this, &DCCConfigView::scrollWheelDecodersClickHandler),
	waitButtonClickCallback(this, &DCCConfigView::waitButtonClickHandler),
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
//	boxDecoder.setClickAction(editTextClickHandlerCallback);
    scrollableContainer1.add(boxDecoder);

	
	// Address	
	yPos += 60;
    textAreaLabelAddress.setXY(x1, yPos);
    textAreaLabelAddress.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textAreaLabelAddress.setLinespacing(0);
    textAreaLabelAddress.setTypedText(touchgfx::TypedText(T_DCCCONFIGADDRESS));
    scrollableContainer1.add(textAreaLabelAddress);

	textAddress.setPosition(x2, yPos, 100, 50);
	textAddress.setClickAction(editTextClickHandlerCallback);
	textAddress.setFontId(Typography::NUMERIC40PX);
    scrollableContainer1.add(textAddress);

	// Name
	yPos += 60;
    textAreaLabelName.setXY(x1, yPos);
    textAreaLabelName.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textAreaLabelName.setLinespacing(0);
    textAreaLabelName.setTypedText(touchgfx::TypedText(T_DCCCONFIGNAME));
    scrollableContainer1.add(textAreaLabelName);

    textName.setPosition(x2, yPos, 360, 50);
	textName.setClickAction(editTextClickHandlerCallback);
	textName.setAlignment(CENTER);
    scrollableContainer1.add(textName);

	// Description
	yPos += 60;
    textAreaLabelDescription.setXY(x1, yPos);
    textAreaLabelDescription.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textAreaLabelDescription.setLinespacing(0);
    textAreaLabelDescription.setTypedText(touchgfx::TypedText(T_DCCCONFIGDESCRIPTION));
    scrollableContainer1.add(textAreaLabelDescription);


    textDescription.setPosition(x2, yPos, 360, 50);
    textDescription.setFontId(Typography::SANSSERIF20PX);
	textDescription.setClickAction(editTextClickHandlerCallback);
	textDescription.setAlignment(CENTER);
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
	
	
	waitButton.setXY(547, 107);
    waitButton.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID));
    waitButton.setLabelText(touchgfx::TypedText(T_WAITBUTTONOKID));
    waitButton.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    waitButton.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	waitButton.setAction(waitButtonClickCallback);
	
	waitWindow.add(waitText);
	waitWindow.add(waitButton);
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
			}
			scrollWheelDecoders.itemChanged(selectedDecoderItem);
			scrollWheelDecoders.invalidate();
			displayDecoder();
		}
		
	}
	//printf("DCCConfigView::scrollWheelDecodersClickHandler %s\n", RELEASED ? "Release" : "Press");
}

void DCCConfigView::displayDecoder()
{
	bool hasDecoder = selectedDecoderItem >= 0 && selectedDecoderItem < uiDecodersConfig.Count();
	bool progTrackEnabled = toggleProgTrack.getState();
	if (hasDecoder)
	{
		Decoders &d = uiDecodersConfig[selectedDecoderItem];
		Unicode::snprintf(addressTextBuffer, countof(addressTextBuffer), (const Unicode::UnicodeChar *)u"%04d", d.getAddress());
		textAddress.setText(addressTextBuffer);
		textName.setText((const Unicode::UnicodeChar *)d.getName());
		textDescription.setText((const Unicode::UnicodeChar *)d.getDescription());
	}
	else
	{
		textAddress.setText((const Unicode::UnicodeChar *)u"----");
		textName.setText((const Unicode::UnicodeChar *)u"----");
		textDescription.setText((const Unicode::UnicodeChar *)u"");
	}
	textAddress.setEnabled(hasDecoder && progTrackEnabled);
	textName.setEnabled(hasDecoder);
	textDescription.setEnabled(hasDecoder);
	
	textAddress.invalidate();
	textName.invalidate();
	textDescription.invalidate();
}

void DCCConfigView::buttonProgTrackClickHandler(const touchgfx::AbstractButton& src)
{
	audioTask.PlaySound(EAudioSounds::KeyPressTone);
	bool trackEnabled = toggleProgTrack.getState();
	presenter->EnableProgTrack(trackEnabled);
	buttonScanTrack.Enable(trackEnabled);
	buttonReadAllCVs.Enable(trackEnabled);
	displayDecoder();
}

void DCCConfigView::buttonScanTrackClickHandler(const touchgfx::AbstractButton& src)
{
	ShowWaitWindow(u"Scanning Programming Track");	
	audioTask.PlaySound(EAudioSounds::KeyPressTone);
	state = Scanning;
	presenter->ScanProgrammingTrack();
}

void DCCConfigView::buttonReadAllCVsClickHandler(const touchgfx::AbstractButton& src)
{
	audioTask.PlaySound(EAudioSounds::KeyPressTone);
}

void DCCConfigView::EditNumeric(EField field, const char16_t *title, uint16_t value, int min, int max)
{
	numericKeypad.Reset();
    numericKeypad.setPosition(0, 0, 800, 480);
	numericKeypad.setTitle((const Unicode::UnicodeChar *)title);
	numericKeypad.setNumber(value);
	numericKeypad.setRange(min, max);
	numericKeypad.setVisible(true);
	numericKeypad.invalidate();
	state = Keypad;
	edittingField = field;
}

void DCCConfigView::EditText(EField field, const char16_t *title, const char16_t *text, uint16_t maxLen, FontId fontId, Alignment align)
{
	keyboard.Reset();
    keyboard.setPosition(0, 0, 800, 480);
	keyboard.setTitle((const Unicode::UnicodeChar *)title);
	keyboard.setText((const Unicode::UnicodeChar *)text);
	keyboard.setTextMaxLen(maxLen);
	keyboard.setEntryAreaFont(fontId);
	keyboard.setEntryAreaAlignment(align);
	keyboard.setVisible(true);
	keyboard.invalidate();	
	state = Keyboard;
	edittingField = field;
}

//void DCCConfigView::editTextClickHandler(const Box& b, const ClickEvent& evt)
void DCCConfigView::editTextClickHandler(const TextWithFrame& b, const ClickEvent& evt)
{
	if (state == Editting && selectedDecoderItem >= 0 )
	{
		Decoders &d = uiDecodersConfig[selectedDecoderItem];
	    if (&b == static_cast<const TextWithFrame *>(&textAddress) && evt.getType() == ClickEvent::RELEASED )
	    {
		    // todo 2 vs 4 digit address
			bool trackEnabled = toggleProgTrack.getState();
		    if (trackEnabled)
		    {
				audioTask.PlaySound(EAudioSounds::KeyPressTone);
				EditNumeric(EField::Address, u"Address", d.getAddress(), 0, 9999);
		    }
		    else
			    audioTask.PlaySound(EAudioSounds::BadKeyPressTone);

	    }
	    else if (&b == static_cast<const TextWithFrame *>(&textName) && evt.getType() == ClickEvent::RELEASED )
	    {
			audioTask.PlaySound(EAudioSounds::KeyPressTone);
		    EditText(EField::Name, u"Name", d.getName(), d.getNameMaxLen(), Typography::SANSSERIF40PX, CENTER);
	    }
	    else if (&b == static_cast<const TextWithFrame *>(&textDescription) && evt.getType() == ClickEvent::RELEASED )
	    {
			audioTask.PlaySound(EAudioSounds::KeyPressTone);
		    EditText(EField::Description, u"Description", d.getDescription(), d.getDescriptionMaxLen(), Typography::SANSSERIF28PX, CENTER);
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
		Decoders &d = uiDecodersConfig[selectedDecoderItem];
		switch (edittingField)
		{
			case Address:
			{
				int newAddress = numericKeypad.getNumber();
				if (newAddress != d.getAddress())
				{
					bool progTrackEnabled = toggleProgTrack.getState();
					if (isProgTrackEnabled())	
					{
						ProgrammingTrack_DCC_WriteCV(CV_PRIMARY_ADDRESS, newAddress);
						ShowWaitWindow(u"Programming Address");
					}
					d.setAddress(newAddress);
					displayDecoder();		// TODO: Lazy-this reloads everything.
				}
				break;
			}
			default:
				assert(false && "Unexpected edittingField");
				break;
		}
	}
}

void DCCConfigView::closeKeyboardWindowHandler(bool success)
{
	state = Editting;
	keyboard.setVisible(false);
	keyboard.invalidate();
	if (success)
	{
		Decoders &d = uiDecodersConfig[selectedDecoderItem];
		switch (edittingField)
		{
			case Name:
				d.setName( (const char16_t *)keyboard.getText() );
				displayDecoder();
				break;
			case Description:
				d.setDescription( (const char16_t *)keyboard.getText() );
				displayDecoder();
				break;
			default:
				assert(false && "Unexpected edittingField");
				break;
		}
	}
}

void DCCConfigView::ScanTrackReply(EErrorCode::EErrorCode result, int16_t address, int16_t config, int16_t extendedAddress, int16_t manufacturer, int16_t version)
{
	CloseWaitWindow(result);
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


void DCCConfigView::ShowWaitWindow(const char16_t *msg, bool button)
{
	if (!button)
	{
		
		waitWindow.setWindowSize(600, 240);
		waitWindow.setWindowBorderColor(Color::getColorFrom24BitRGB(3, 129, 174));

		waitText.setPosition(25, 85, 550, 50);
		waitText.setFontId(Typography::SANSSERIF40PX);
		waitText.setAlignment(CENTER);
		waitText.setBoxColor(Color::getColorFrom24BitRGB(255, 255, 255));
		
		waitButton.setVisible(false);
	}
	else
	{
		waitWindow.setWindowSize(600, 240);
		waitWindow.setWindowBorderColor(Color::getColorFrom24BitRGB(3, 129, 174));

		waitText.setPosition(25, 55, 550, 50);
		waitText.setFontId(Typography::SANSSERIF40PX);
		waitText.setAlignment(CENTER);
		waitText.setBoxColor(Color::getColorFrom24BitRGB(255, 255, 255));

		waitButton.setVisible(true);
		waitButton.setXY((600-170)/2, 150);
	}

	waitText.setText(msg);
	add(waitWindow);
	waitWindow.setVisible(true);
	waitWindow.invalidate();
}


void DCCConfigView::CloseWaitWindow(EErrorCode::EErrorCode result)
{
	waitWindow.setVisible(false);
	waitWindow.invalidate();
	remove(waitWindow);
	
	if (result != EErrorCode::Success)
	{
		ShowWaitWindow(ErrorCodeText(result), true);
	}
}


void DCCConfigView::waitButtonClickHandler(const touchgfx::AbstractButton& src)
{
	CloseWaitWindow(EErrorCode::Success);
}
