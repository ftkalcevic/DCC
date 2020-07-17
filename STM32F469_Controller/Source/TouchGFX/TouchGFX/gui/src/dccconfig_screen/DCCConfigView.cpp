#include <gui/dccconfig_screen/DCCConfigView.hpp>
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include <stdio.h>
#include "Common.h"
#include "Utility.h"
#include "DecodersConfig.h"
#include "AudioTask.h"
#include "dcc.h"
#include "ProgTrackDCC.h"		// this should be in model
#include "DecoderDefConfig.h"


enum EDecoderDisplay
{
	Loco = 0x01,
	Acc = 0x02,
	Both = Loco | Acc
};

DCCConfigView::DCCConfigView() :
    buttonProgTrackClickCallback(this, &DCCConfigView::buttonProgTrackClickHandler),
    buttonScanTrackClickCallback(this, &DCCConfigView::buttonScanTrackClickHandler),
    buttonReadAllCVsClickCallback(this, &DCCConfigView::buttonReadAllCVsClickHandler),
	buttonDeleteClickCallback(this, &DCCConfigView::buttonDeleteClickHandler),
	editTextClickHandlerCallback(this, &DCCConfigView::editTextClickHandler),
    closeKeypadWindowCallback(this, &DCCConfigView::closeKeypadWindowHandler),
    closeKeyboardWindowCallback(this, &DCCConfigView::closeKeyboardWindowHandler),
    scrollWheelDecodersClickCallback(this, &DCCConfigView::scrollWheelDecodersClickHandler),
	waitOKButtonClickCallback(this, &DCCConfigView::waitOKButtonClickHandler),
	waitCancelButtonClickCallback(this, &DCCConfigView::waitCancelButtonClickHandler),
	selectListOKButtonClickCallback(this, &DCCConfigView::selectListOKButtonClickHandler),
	selectListCancelButtonClickCallback(this, &DCCConfigView::selectListCancelButtonClickHandler),
    cboSpeedStepsSelectionChangedCallback(this, &DCCConfigView::cboSpeedStepsSelectionChangedHandler),
    buttonCVDisplayClickCallback(this, &DCCConfigView::buttonCVDisplayClickHandler),
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
	buttonScanTrack.setEnabled(false);
	buttonScanTrack.setAction(buttonScanTrackClickCallback);
    scrollableContainer1.add(buttonScanTrack);

    buttonReadAllCVs.setXY(547, 185);
    buttonReadAllCVs.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_PRESSED_ID), touchgfx::Bitmap(BITMAP_ROUND_EDGE_MEDIUM_DISABLED_ID));
    buttonReadAllCVs.setLabelText(touchgfx::TypedText(T_RESOURCEREADALLCVS));
    buttonReadAllCVs.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    buttonReadAllCVs.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	buttonReadAllCVs.setEnabled(false);
	buttonReadAllCVs.setAction(buttonReadAllCVsClickCallback);
    scrollableContainer1.add(buttonReadAllCVs);

	int16_t yPos = 275;
    buttonDelete.setXY(x1, yPos);
    buttonDelete.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_PRESSED_ID), touchgfx::Bitmap(BITMAP_ROUND_EDGE_MEDIUM_DISABLED_ID));
    buttonDelete.setLabelText(touchgfx::TypedText(T_RESOURCEDELETE));
    buttonDelete.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    buttonDelete.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	buttonDelete.setEnabled(false);
	buttonDelete.setAction(buttonDeleteClickCallback);
    scrollableContainer1.add(buttonDelete);
	
	// Decoder
	yPos += 70 + LINE_SPACING;
    textAreaLabelDecoder.setXY(x1, yPos);
    textAreaLabelDecoder.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textAreaLabelDecoder.setLinespacing(0);
    textAreaLabelDecoder.setTypedText(touchgfx::TypedText(T_DCCCONFIGDECODER));
    scrollableContainer1.add(textAreaLabelDecoder);

    textDecoder.setPosition(x2, yPos, 360, 50);
	textName.setAlignment(CENTER);
	textDecoder.setClickAction(editTextClickHandlerCallback);
    scrollableContainer1.add(textDecoder);

	// Address	
	yPos += 50 + LINE_SPACING;
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
	yPos += 50 + LINE_SPACING;
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
	yPos += 50 + LINE_SPACING;
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

	decoderSpecificYStartPos = yPos;
	
	HideAllCustomConfigs();
	
	cboSpeedSteps.addComboItem(u"14 Steps", ESpeedSteps::ss14);
	cboSpeedSteps.addComboItem(u"28 Steps", ESpeedSteps::ss28);
	cboSpeedSteps.addComboItem(u"128 Steps", ESpeedSteps::ss128);
	
	scrollableContainer1.add(textAreaLabelSpeedSteps);
	scrollableContainer1.add(cboSpeedSteps);
	scrollableContainer1.add(textAreaLabelConfig);
	scrollableContainer1.add(boxConfig);
	scrollableContainer1.add(chkMFDirection);
	scrollableContainer1.add(chkMFFLLocation);
	scrollableContainer1.add(chkMFPowerSourceConversion);
	scrollableContainer1.add(chkMFBiDirectionalComms);
	scrollableContainer1.add(chkMFSpeedTable);
	scrollableContainer1.add(chkMFTwoByteAddressing);
	scrollableContainer1.add(chkAccBiDirectionalComms);
	scrollableContainer1.add(chkAccType);
	scrollableContainer1.add(chkAccAddressMethod);
	
    btnCVs.setXY(x1, yPos);
    btnCVs.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_PRESSED_ID));
	btnCVs.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT40PXID));
	btnCVs.setText(u"Config");
	btnCVs.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	btnCVs.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	btnCVs.setVisible(false);
	btnCVs.setAction(buttonCVDisplayClickCallback);
	btnCVs.forceState(true);
    scrollableContainer1.add(btnCVs);
	
    btnGroup.setXY(x1 + 260, yPos);
    btnGroup.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_PRESSED_ID));
	btnGroup.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT40PXID));
	btnGroup.setText(u"Grouped");
	btnGroup.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	btnGroup.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	btnGroup.setVisible(false);
	btnGroup.setAction(buttonCVDisplayClickCallback);
    scrollableContainer1.add(btnGroup);
	
    btnRaw.setXY(x1 + 260*2, yPos);
    btnRaw.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_PRESSED_ID));
	btnRaw.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT40PXID));
	btnRaw.setText(u"Raw");
	btnRaw.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	btnRaw.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	btnRaw.setVisible(false);
	btnRaw.setAction(buttonCVDisplayClickCallback);
    scrollableContainer1.add(btnRaw);

	yPos += 50 + LINE_SPACING;
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
	
	waitText.setFontId(Typography::SANSSERIF40PX);
	waitText.setAlignment(CENTER);
	waitText.setBoxColor(Color::getColorFrom24BitRGB(255, 255, 255));

	waitSubText.setFontId(Typography::SANSSERIF28PX);
	waitSubText.setAlignment(CENTER);
	waitSubText.setBoxColor(Color::getColorFrom24BitRGB(255, 255, 255));
	
	waitOKButton.setXY(547, 107);
    waitOKButton.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID));
    waitOKButton.setLabelText(touchgfx::TypedText(T_WAITBUTTONOKID));
    waitOKButton.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    waitOKButton.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	waitOKButton.setAction(waitOKButtonClickCallback);
	
	waitCancelButton.setXY(547, 107);
    waitCancelButton.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID));
    waitCancelButton.setLabelText(touchgfx::TypedText(T_WAITBUTTONCANCELID));
    waitCancelButton.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    waitCancelButton.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	waitCancelButton.setAction(waitCancelButtonClickCallback);
	
	waitWindow.add(waitText);
	waitWindow.add(waitSubText);
	waitWindow.add(waitOKButton);
	waitWindow.add(waitCancelButton);
	
	
	selectText.setFontId(Typography::SANSSERIF40PX);
	selectText.setAlignment(CENTER);
	selectText.setBoxColor(Color::getColorFrom24BitRGB(255, 255, 255));
	
	selectOKButton.setXY(547, 107);
    selectOKButton.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID));
    selectOKButton.setLabelText(touchgfx::TypedText(T_WAITBUTTONOKID));
    selectOKButton.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    selectOKButton.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	selectOKButton.setAction(selectListOKButtonClickCallback);
	
	selectCancelButton.setXY(547, 107);
    selectCancelButton.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID));
    selectCancelButton.setLabelText(touchgfx::TypedText(T_WAITBUTTONCANCELID));
    selectCancelButton.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    selectCancelButton.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	selectCancelButton.setAction(selectListCancelButtonClickCallback);

	cboSelectList.setPosition(547, 107, 360, 50);
	cboSelectList.addComboItem(u"Other",-1);
	for ( int i = 0; i < decoderDefinitions.Count(); i++)
		cboSelectList.addComboItem(decoderDefinitions[i],i);
	
	selectWindow.add(selectText);
	selectWindow.add(selectOKButton);
	selectWindow.add(selectCancelButton);
	selectWindow.add(cboSelectList);
}

void DCCConfigView::buttonCVDisplayClickHandler(const touchgfx::AbstractButton& src)
{
	bool raw = false, cv = false, group = false;
	
	if (&src == static_cast<AbstractButton *>(&btnRaw))
		raw = true;
	else if (&src == static_cast<AbstractButton *>(&btnCVs))
		cv = true;
	else if (&src == static_cast<AbstractButton *>(&btnGroup))
		group = true;
	
	btnRaw.forceState(raw);
	btnCVs.forceState(cv);
	btnGroup.forceState(group);
	btnRaw.invalidate();
	btnCVs.invalidate();
	btnGroup.invalidate();
	
	clearCVDisplay();
	displayCVDisplay();
}

void DCCConfigView::HideAllCustomConfigs()
{
	bool vis = false;
	
	textAreaLabelSpeedSteps.setVisible(vis);
	cboSpeedSteps.setVisible(vis);
	textAreaLabelConfig.setVisible(vis);
	boxConfig.setVisible(vis);
	chkMFDirection.setVisible(vis);
	chkMFFLLocation.setVisible(vis);
	chkMFPowerSourceConversion.setVisible(vis);
	chkMFBiDirectionalComms.setVisible(vis);
	chkMFSpeedTable.setVisible(vis);
	chkMFTwoByteAddressing.setVisible(vis);
	textAreaLabelConfig.setVisible(vis);
	boxConfig.setVisible(vis);
	chkAccBiDirectionalComms.setVisible(vis);
	chkAccType.setVisible(vis);
	chkAccAddressMethod.setVisible(vis);
	chkAccBiDirectionalComms.setVisible(vis);
	chkAccType.setVisible(vis);
	chkAccAddressMethod.setVisible(vis);
}

void DCCConfigView::showDecoderSpecificSettings( bool loco )
{
	bool acc = !loco;
	
	int16_t dx = scrollableContainer1.getScrolledX();
	int16_t dy = scrollableContainer1.getScrolledY();
	scrollableContainer1.doScroll(-dx, -dy);
	
	HideAllCustomConfigs();
	
	int16_t yPos = decoderSpecificYStartPos;
	uint16_t yConfig = 0;
	if (loco)
	{
		// Multifunction Decoder Options
		yPos += 50 + LINE_SPACING;
		textAreaLabelSpeedSteps.setXY(x1, yPos);
		textAreaLabelSpeedSteps.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
		textAreaLabelSpeedSteps.setLinespacing(0);
		textAreaLabelSpeedSteps.setTypedText(touchgfx::TypedText(T_DCCCONFIGSPEEDSTEPS));
		textAreaLabelSpeedSteps.setVisible(true);

		cboSpeedSteps.setPosition(x2, yPos, 360, 50);
		cboSpeedSteps.setSelectionChangedCallback(&cboSpeedStepsSelectionChangedCallback);
		cboSpeedSteps.setNumberOfItems(3);
		cboSpeedSteps.setVisible(true);
	
		// Config
		yPos += 50 + LINE_SPACING;
		yConfig = 10 + 6 * 65 + 10;
		textAreaLabelConfig.setXY(x1, yPos);
		textAreaLabelConfig.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
		textAreaLabelConfig.setLinespacing(0);
		textAreaLabelConfig.setTypedText(touchgfx::TypedText(T_DCCCONFIGCONFIG));
		textAreaLabelConfig.setVisible(true);

		boxConfig.setPosition(x2, yPos, 360, yConfig);
		boxConfig.setColor(touchgfx::Color::getColorFrom24BitRGB(163, 163, 163));
		boxConfig.setVisible(true);

	
		chkMFDirection.setPosition(x2 + 10, yPos + 10 + 65 * 0, 340, 65);
		chkMFFLLocation.setPosition(x2 + 10, yPos + 10 + 65 * 1, 340, 65);
		chkMFPowerSourceConversion.setPosition(x2 + 10, yPos + 10 + 65 * 2, 340, 65);
		chkMFBiDirectionalComms.setPosition(x2 + 10, yPos + 10 + 65 * 3, 340, 65);
		chkMFSpeedTable.setPosition(x2 + 10, yPos + 10 + 65 * 4, 340, 65);
		chkMFTwoByteAddressing.setPosition(x2 + 10, yPos + 10 + 65 * 5, 340, 65);
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

		chkMFDirection.setVisible(true);
		chkMFFLLocation.setVisible(true);
		chkMFPowerSourceConversion.setVisible(true);
		chkMFBiDirectionalComms.setVisible(true);
		chkMFSpeedTable.setVisible(true);
		chkMFTwoByteAddressing.setVisible(true);
	}
	
	
	if (acc)
	{
		// Config
		yPos += 50 + LINE_SPACING;
		yConfig = 10 + 3 * 65 + 10;
		textAreaLabelConfig.setXY(x1, yPos);
		textAreaLabelConfig.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
		textAreaLabelConfig.setLinespacing(0);
		textAreaLabelConfig.setTypedText(touchgfx::TypedText(T_DCCCONFIGCONFIG));
		textAreaLabelConfig.setVisible(true);

		boxConfig.setPosition(x2, yPos, 360, yConfig);
		boxConfig.setColor(touchgfx::Color::getColorFrom24BitRGB(163, 163, 163));
		boxConfig.setVisible(true);

		chkAccBiDirectionalComms.setPosition(x2 + 10, yPos + 10 + 65 * 0, 340, 65);
		chkAccType.setPosition(x2 + 10, yPos + 10 + 65 * 1, 340, 65);
		chkAccAddressMethod.setPosition(x2 + 10, yPos + 10 + 65 * 2, 340, 65);
		chkAccBiDirectionalComms.setText(u"Bi-direction Communications");
		chkAccType.setText(u"Extended Type");
		chkAccAddressMethod.setText(u"Advanced Addressing");
		chkAccBiDirectionalComms.setVisible(true);
		chkAccType.setVisible(true);
		chkAccAddressMethod.setVisible(true);
		chkAccBiDirectionalComms.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));
		chkAccType.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));
		chkAccAddressMethod.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));

		chkAccBiDirectionalComms.setVisible(true);
		chkAccType.setVisible(true);
		chkAccAddressMethod.setVisible(true);
	}
	
	// CV Display buttons
	yPos += yConfig + LINE_SPACING;
	
    btnCVs.setXY(x1, yPos);
	btnCVs.setVisible(true);
	
    btnGroup.setXY(x1 + 260, yPos);
	btnGroup.setVisible(true);
	
    btnRaw.setXY(x1 + 260*2, yPos);
	btnRaw.setVisible(true);

	yPos += 50 + LINE_SPACING;
	yCVBegin = yPos;
	
	yPos += LINE_SPACING;
    backgroundImage.setPosition(0, 0, 800, yPos);
	
	scrollableContainer1.doScroll(dx, dy);
	scrollableContainer1.childGeometryChanged();
	scrollableContainer1.invalidate();
	
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

void DCCConfigView::setConfig(uint8_t cv29)
{
	bool isAccessoryDecoder = (cv29 & CV29_ACCESSORY_DECODER) != 0;
	
//	chkMFDirection.setVisible(!isAccessoryDecoder);
//	chkMFFLLocation.setVisible(!isAccessoryDecoder);
//	chkMFPowerSourceConversion.setVisible(!isAccessoryDecoder);
//	chkMFBiDirectionalComms.setVisible(!isAccessoryDecoder);
//	chkMFSpeedTable.setVisible(!isAccessoryDecoder);
//	chkMFTwoByteAddressing.setVisible(!isAccessoryDecoder);
//	chkAccBiDirectionalComms.setVisible(isAccessoryDecoder);
//	chkAccType.setVisible(isAccessoryDecoder);
//	chkAccAddressMethod.setVisible(isAccessoryDecoder);

	chkMFDirection.setSelected(cv29 & CV29_DIRECTION);
	chkMFFLLocation.setSelected(cv29 & CV29_FL_LOCATION);
	chkMFPowerSourceConversion.setSelected(cv29 & CV29_POWER_SOURCE_CONV);
	chkMFBiDirectionalComms.setSelected(cv29 & CV29_BIDIRECT_COMMS);
	chkMFSpeedTable.setSelected(cv29 & CV29_FULL_SPEED_TABLE);
	chkMFTwoByteAddressing.setSelected(cv29 & CV29_TWO_BYTE_ADDRESS );
	chkAccBiDirectionalComms.setSelected(cv29 & CV29_ACC_BIDIRECT_COMMS);
	chkAccType.setSelected(cv29 & CV29_ACC_EXTENDED_DECODER);
	chkAccAddressMethod.setSelected(cv29 & CV29_ACC_EXTENDED_ADDR);
}

void DCCConfigView::loadDecoderDef(const char *filename)
{
	decoderDefinitions.clear();
	decoderDefinitions.loadDecoderDef(filename);
}

void DCCConfigView::clearCVDisplay()
{
	for (auto it = cvDrawables.begin(); it != cvDrawables.end(); it++)
	{
		touchgfx::Drawable *d = *it;
		bool removed = scrollableContainer1.remove(*d);
		assert(removed);
		delete d;	// todo - use smart pointer
	}
	cvDrawables.clear();
	scrollableContainer1.childGeometryChanged();
}


uint16_t DCCConfigView::DrawCVTitle(const char16_t *name, const uint16_t yPos, const uint16_t XOffset, const uint16_t titleWidth )
{
	TextAreaWithOneWildcard *text = new TextAreaWithOneWildcard();
	text->setWideTextAction(touchgfx::WIDE_TEXT_WORDWRAP);
	text->setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	text->setLinespacing(0);	// WARNING linespacing!= requires widget box to be resized.
	text->setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT40PXID));
	text->setWildcard((const Unicode::UnicodeChar*)name);
	text->setPosition(x1+XOffset, yPos, titleWidth, 150);
	text->resizeHeightToCurrentText();
	uint16_t h = text->getTextHeight();
	scrollableContainer1.add(*text);
	cvDrawables.push_back(text);
	
	return h;
}

void DCCConfigView::DisplayCV(EDecoderDataType::EDecoderDataType cvType, uint16_t cvNumber, const char16_t *name, CVDef &cv, uint16_t &yPos)
{
	switch (cvType)
	{
		case EDecoderDataType::Enum:
		case EDecoderDataType::Bitfield:
			break;
		case EDecoderDataType::SpeedTable:
			{
				uint16_t h = DrawCVTitle(name, yPos);
				
				ButtonWithLabel *btn = new ButtonWithLabel();
				btn->setXY(CV_X2, yPos);
				btn->setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_PRESSED_ID));
				btn->setLabelText(touchgfx::TypedText(T_DCCCONFIGSPEEDSTEPS));
				btn->setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
				btn->setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
				//btn->setAction(buttonDeleteClickCallback);
				scrollableContainer1.add(*btn);
				cvDrawables.push_back(btn);
				yPos += h + LINE_SPACING;
			}
			break;
		case EDecoderDataType::User:
			{
				DecoderDef &def = decoderDefinitions.getDecoderDef(); 
				std::shared_ptr<UserType> type = def.types[cv.userType]; 
				
				uint16_t h = DrawCVTitle(name, yPos, 0, type->hasLabeledTypes() ? CV_FULL_LABEL_WIDTH : CV_LABEL_WIDTH );
				if (type->hasLabeledTypes())
				{
					yPos += h + LINE_SPACING;
				}
				
				uint16_t yBoxStart = yPos-20;
				touchgfx::Box * box = new Box();
				box->setPosition(x2-20, yBoxStart, 360+2*20, 100);
				box->setColor(touchgfx::Color::getColorFrom24BitRGB(163, 163, 163));
				box->setVisible(true);
				scrollableContainer1.add(*box);					
				cvDrawables.push_back(box);
					
				for (auto itBF = type->bitFields.begin(); itBF != type->bitFields.end(); itBF++)
				{
					BitField &bf = **itBF;
					if (bf.type == EDecoderDataType::Int)
					{
						uint16_t h = DrawCVTitle(bf.name.c_str(), yPos, CV_SUBTITLE_OFFSET);
				
						touchgfx::ClickListener<touchgfx::TextWithFrame> *edit = new touchgfx::ClickListener<touchgfx::TextWithFrame>();
						edit->setPosition(CV_X2, yPos, 100, 50);
						edit->setClickAction(editTextClickHandlerCallback);
						edit->setFontId(Typography::NUMERIC40PX);
						scrollableContainer1.add(*edit);					
						cvDrawables.push_back(edit);
						yPos += h+ + LINE_SPACING;
						
					}
					else
					{
						for (auto it = bf.bits.begin(); it != bf.bits.end(); it++)
						{
							Bit &b = **it;
							if (b.on.size() > 0 && b.off.size() > 0 )
							{
								uint16_t h = DrawCVTitle(b.name.c_str(), yPos, CV_SUBTITLE_OFFSET);
								
								ComboBox *cbo = new ComboBox;
								cbo->setPosition(CV_X2, yPos, CV_EDIT_WIDTH_MAX, 50);
								//cbo->setSelectionChangedCallback(&cboSpeedStepsSelectionChangedCallback);
								cbo->setNumberOfItems(2);
								cbo->setVisible(true);
								cbo->addComboItem(b.off.c_str(), 0);
								cbo->addComboItem(b.on.c_str(), 1);
								scrollableContainer1.add(*cbo);
								cvDrawables.push_back(cbo);
								yPos += h + LINE_SPACING;
							}
							else
							{
								CheckBox *chk = new CheckBox;							
								chk->setPosition(CV_X2, yPos, 340, 65);
								chk->setText(b.name.c_str());
								chk->setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));
								chk->setVisible(true);
								scrollableContainer1.add(*chk);
								cvDrawables.push_back(chk);
								yPos += 50;
							}
						}					
						if (bf.enums.size() > 0)
						{
							uint16_t h = DrawCVTitle(bf.name.c_str(), yPos, CV_SUBTITLE_OFFSET);
								
							ComboBox *cbo = new ComboBox;
							cbo->setPosition(CV_X2, yPos, CV_EDIT_WIDTH_MAX, 50);
							//cbo->setSelectionChangedCallback(&cboSpeedStepsSelectionChangedCallback);
							cbo->setNumberOfItems(bf.enums.size());
							cbo->setVisible(true);
							for (auto it = bf.enums.begin(); it != bf.enums.end(); it++)
							{
								Enum &e = **it;
								cbo->addComboItem(e.name.c_str(), e.value);
							}
							scrollableContainer1.add(*cbo);
							cvDrawables.push_back(cbo);
							yPos += h + LINE_SPACING;
						}
					}
				}
				box->setHeight(yPos - yBoxStart + 20);
				yPos += 40 + LINE_SPACING;
			}
			break;
		case EDecoderDataType::Address:	// address - only show in raw mode - use common address
		case EDecoderDataType::Int:
			{
				uint16_t h = DrawCVTitle(name, yPos);

				touchgfx::ClickListener<touchgfx::TextWithFrame> *edit = new touchgfx::ClickListener<touchgfx::TextWithFrame>();
				edit->setPosition(CV_X2, yPos, 100, 50);
				edit->setClickAction(editTextClickHandlerCallback);
				edit->setFontId(Typography::NUMERIC40PX);
				scrollableContainer1.add(*edit);					
				cvDrawables.push_back(edit);
				yPos += h+ + LINE_SPACING;
			}
			break;
	}
}

void DCCConfigView::displayCVDisplay()
{
	DecoderDef &def = decoderDefinitions.getDecoderDef(); 
	
	// Radio buttons
	// Buttons for [CV] [Group] [Raw]  (group if there are groups).
	
	int16_t dx = scrollableContainer1.getScrolledX();
	int16_t dy = scrollableContainer1.getScrolledY();
	scrollableContainer1.doScroll(-dx, -dy);
	// Count cvs (max)
	int cvSum = 0;
	for (auto itCV = def.CVs.begin(); itCV != def.CVs.end(); itCV++)
		cvSum += itCV->second->cvcount;
		
	cvNumbers.clear();
	cvNumbers.reserve(cvSum);	
	cvDrawables.reserve(cvSum*4);	
	
	uint16_t yPos = yCVBegin;
	
	if ( btnCVs.getState() )
	{
		for (auto itCV = def.CVs.begin(); itCV != def.CVs.end(); itCV++)
		{
			CVDef &cv = *(itCV->second);
			DisplayCV(cv.type, cv.number, cv.name.c_str(), cv, yPos);
		}
	}
	else if ( btnRaw.getState() )
	{
		// todo - sort by cv number
		for (auto itCV = def.CVs.begin(); itCV != def.CVs.end(); itCV++)
		{
			CVDef &cv = *(itCV->second);
			
			for (int i = 0; i < cv.cvcount; i++)
			{
				int cvNum = cv.number + i;
				
				char16_t buffer[15];
				Unicode::snprintf((Unicode::UnicodeChar *)buffer, countof(buffer), "CV%02d", cvNum);
				cvNumbers.push_back(buffer);
				const char16_t *str = cvNumbers.back().c_str();
				
				DisplayCV(EDecoderDataType::Int, cvNum, str, cv, yPos);
			}
		}
	}
	else if ( btnGroup.getState() )
	{
		for (auto itGroup = def.groups.begin(); itGroup != def.groups.end(); itGroup++)
		{
			Group &g = **itGroup;
			
			TextAreaWithOneWildcard *text = new TextAreaWithOneWildcard();
			text->setXY(x1, yPos);
			text->setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
			text->setLinespacing(0);
			text->setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT40PXID));
			text->setWildcard((const Unicode::UnicodeChar*)g.name.c_str());
			scrollableContainer1.add(*text);
			cvDrawables.push_back(text);
			yPos += 50 + LINE_SPACING;
			
			for (auto itCVNum = g.cvs.begin(); itCVNum != g.cvs.end(); itCVNum++)
			{
				CVDef &cv = *(def.CVs[*itCVNum]);
				DisplayCV(cv.type, cv.number, cv.name.c_str(), cv, yPos);
			}
		}
	}
	
	yPos += LINE_SPACING;
	backgroundImage.setPosition(0, 0, 800, yPos);
	scrollableContainer1.doScroll(dx, dy);
	scrollableContainer1.childGeometryChanged();
	scrollableContainer1.invalidate();
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
		if (strlen(d.getDecoderDefFilename()) == 0)
		{
			textDecoder.setText(u"Other"); 
		}
		else
		{
			strncpy16(decoderDefBuffer, d.getDecoderDefFilename(), countof(decoderDefBuffer));
			textDecoder.setText(decoderDefBuffer); 
			loadDecoderDef(d.getDecoderDefFilename());
		}
		showDecoderSpecificSettings(d.getType() == EDecoderType::Multifunction);
		clearCVDisplay();
		displayCVDisplay();
		setConfig(d.getConfig());
			
		if (d.getType() == EDecoderType::Multifunction)
		{
			cboSpeedSteps.setSelectedItem(d.getLoco().getSpeedSteps());
		}
	}
	else
	{
		textAddress.setText((const Unicode::UnicodeChar *)u"----");
		textName.setText((const Unicode::UnicodeChar *)u"----");
		textDescription.setText((const Unicode::UnicodeChar *)u"");
		setConfig(0);
		HideAllCustomConfigs();
	}
	buttonDelete.setEnabled(hasDecoder);
	
	textAddress.setEnabled(hasDecoder && progTrackEnabled);
	textName.setEnabled(hasDecoder);
	textDescription.setEnabled(hasDecoder);
	textDecoder.setEnabled(hasDecoder);
	
	textAddress.invalidate();
	textName.invalidate();
	textDescription.invalidate();
	textDecoder.invalidate();
}

void DCCConfigView::buttonProgTrackClickHandler(const touchgfx::AbstractButton& src)
{
	audioTask.PlaySound(EAudioSounds::KeyPressTone);
	bool trackEnabled = toggleProgTrack.getState();
	presenter->EnableProgTrack(trackEnabled);
	buttonScanTrack.setEnabled(trackEnabled);
	buttonReadAllCVs.setEnabled(trackEnabled);
	displayDecoder();
}

void DCCConfigView::buttonScanTrackClickHandler(const touchgfx::AbstractButton& src)
{
	audioTask.PlaySound(EAudioSounds::KeyPressTone);
	ShowWaitWindow(u"Scanning Programming Track");	
	state = EState::Scanning;
	presenter->ScanProgrammingTrack();
}

void DCCConfigView::buttonReadAllCVsClickHandler(const touchgfx::AbstractButton& src)
{
	audioTask.PlaySound(EAudioSounds::KeyPressTone);
	ShowWaitWindow(u"Scanning for All CVs", u"", EButtons::Cancel);	
	state = EState::ScanningAllCVs;
	presenter->ScanAllCVsTrack();
}

void DCCConfigView::buttonDeleteClickHandler(const touchgfx::AbstractButton& src)
{
	ShowWaitWindow(u"Delete Decoder?", nullptr, EButtons::OK | EButtons::Cancel);	
	state = EState::DeleteDecoder;
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
	state = EState::Keypad;
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
	state = EState::Keyboard;
	edittingField = field;
}

void DCCConfigView::editTextClickHandler(const TextWithFrame& b, const ClickEvent& evt)
{
	if (state == EState::Editting && selectedDecoderItem >= 0 && evt.getType() == ClickEvent::RELEASED )
	{
		Decoders &d = uiDecodersConfig[selectedDecoderItem];
	    if (&b == static_cast<const TextWithFrame *>(&textAddress) )
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
	    else if (&b == static_cast<const TextWithFrame *>(&textName) )
	    {
			audioTask.PlaySound(EAudioSounds::KeyPressTone);
		    EditText(EField::Name, u"Name", d.getName(), d.getNameMaxLen(), Typography::SANSSERIF40PX, CENTER);
	    }
	    else if (&b == static_cast<const TextWithFrame *>(&textDescription) )
	    {
			audioTask.PlaySound(EAudioSounds::KeyPressTone);
		    EditText(EField::Description, u"Description", d.getDescription(), d.getDescriptionMaxLen(), Typography::SANSSERIF28PX, CENTER);
	    }
	    else if (&b == static_cast<const TextWithFrame *>(&textDecoder) )
	    {
			audioTask.PlaySound(EAudioSounds::KeyPressTone);
		    SelectDecoderDefinition(u"Select Decoder Model");
	    }
	}
}


void DCCConfigView::SelectDecoderDefinition(const char16_t *title)
{
	const int spacing = 15;
	const int windowWidth = 600;
	const int windowHeight = 340;
	const int textwidth = windowWidth - 50;
	selectWindow.setWindowSize(windowWidth, windowHeight);
	selectWindow.setWindowBorderColor(Color::getColorFrom24BitRGB(3, 129, 174));
	
	selectText.setPosition((windowWidth - textwidth)/2, 40, textwidth, 50);

	cboSelectList.setPosition((windowWidth - 360)/2, 135, 360, 50);
	
	int buttonWidth = selectOKButton.getWidth();
	selectOKButton.setXY(windowWidth / 2 - buttonWidth - spacing, 250);
	selectCancelButton.setXY(windowWidth / 2 + spacing, 250);

	selectText.setText(title);
	add(selectWindow);
	selectWindow.setVisible(true);
	selectWindow.invalidate();
	state = EState::SelectDecoderDef;
}

void DCCConfigView::closeKeypadWindowHandler(bool success)
{
	state = EState::Editting;
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
						// todo - this should be MVPt
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
	state = EState::Editting;
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

void DCCConfigView::ScanAllCVsReply(EErrorCode::EErrorCode result, uint16_t cv, uint8_t value)
{
	if (result == EErrorCode::Complete)
	{
		CloseWaitWindow(EErrorCode::Success);
	}
	else
	{
		if (result == EErrorCode::Success)
			Unicode::snprintf((Unicode::UnicodeChar *)scanAllTextBuffer, countof(scanAllTextBuffer), "CV[%d]=%d", cv, value);
		else
			Unicode::snprintf((Unicode::UnicodeChar *)scanAllTextBuffer, countof(scanAllTextBuffer), "CV[%d]=%s!", cv, ErrorCodeText(result) );
		waitSubText.setText(scanAllTextBuffer);
		waitSubText.invalidate();
	}
}

void DCCConfigView::WriteReply(EErrorCode::EErrorCode result)
{
	CloseWaitWindow(result);
	if (result == EErrorCode::Success)
	{
		state = EState::Editting;
	}
}

void DCCConfigView::ScanTrackReply(EErrorCode::EErrorCode result, int16_t address, int16_t config, int16_t extendedAddress, int16_t manufacturer, int16_t version)
{
	printf("ScanTrackReply %d addr=%d config=%d extAddr=%d manufacturer=%d version=%d\n", result, address, config, extendedAddress, manufacturer, version);
	CloseWaitWindow(result);
	if ( state == EState::Scanning && result == EErrorCode::Success )
	{
		int index = uiDecodersConfig.findEncoder(address);
		if (index >= 0)
		{
			// If this matches an existing configuration, load it up.
			selectedDecoderItem = index;
			scrollWheelDecoders.itemChanged(index);
			scrollWheelDecoders.animateToItem(index);
			displayDecoder();
		}
		else
		{
			// Create a new one
			uiDecodersConfig.newDecoder();
			selectedDecoderItem = uiDecodersConfig.Count() - 1;
			Decoders &d = uiDecodersConfig[selectedDecoderItem];
			d.setAddress(address);
			d.setConfig(config);
			scrollWheelDecoders.setNumberOfItems(uiDecodersConfig.Count() + 1);
			scrollWheelDecoders.itemChanged(selectedDecoderItem);
			scrollWheelDecoders.invalidate();
			displayDecoder();
		}
	}
	if (result == EErrorCode::Success)
		state = EState::Editting;
}


void DCCConfigView::SwipePreviousScreen()
{
	application().returnToPreferences();
}

void DCCConfigView::handleGestureEvent(const GestureEvent & evt)
{
	if (state != EState::Editting )
		return;
	
	//printf("DCCConfigView gesture %d %d\n", evt.getType(), evt.getVelocity());
	if(scrollWheelDecoders.getRect().intersect(evt.getX(), evt.getY()))
	{
		scrollWheelDecoders.handleGestureEvent(evt);
	}
	else
	{
		base_class::handleGestureEvent(evt); 
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
		base_class::handleDragEvent(evt); 
	}
}


void DCCConfigView::scrollWheelDecodersUpdateItem(ListItemDecoder& item, int16_t itemIndex)
{
	item.setIndex(itemIndex, itemIndex == selectedDecoderItem);
}


void DCCConfigView::cboSpeedStepsSelectionChangedHandler(ComboBoxBase& cbo, int16_t itemIndex, int16_t id)
{
	Decoders &d = uiDecodersConfig[selectedDecoderItem];
	d.getLoco().setSpeedSteps((ESpeedSteps::ESpeedSteps)id);	
}

void DCCConfigView::ShowWaitWindow(const char16_t *title, const char16_t *subtitle, EButtons buttons )
{
	const int spacing = 15;
	const int windowWidth = 600;
	const int windowHeight = 240;
	const int textwidth = windowWidth - 50;
	waitWindow.setWindowSize(windowWidth, windowHeight);
	waitWindow.setWindowBorderColor(Color::getColorFrom24BitRGB(3, 129, 174));
	
	if (subtitle == nullptr)
	{
		waitText.setPosition((windowWidth - textwidth)/2, 65, textwidth, 50);
		waitSubText.setVisible(false);
	}
	else
	{
		waitText.setPosition((windowWidth - textwidth)/2, 15, textwidth, 50);
		waitSubText.setPosition((windowWidth - textwidth)/2, 85, textwidth, 50);
		waitSubText.setVisible(true);
	}

	waitOKButton.setVisible( ((buttons & EButtons::OK) != 0) ? true : false);
	waitCancelButton.setVisible( ((buttons & EButtons::Cancel) != 0) ? true : false);

	int buttonWidth = waitOKButton.getWidth();
	if ((buttons & (EButtons::Cancel | EButtons::OK)) == (EButtons::Cancel | EButtons::OK))
	{
		// 2 buttons
		waitOKButton.setXY(windowWidth / 2 - buttonWidth - spacing, 150);
		waitCancelButton.setXY(windowWidth / 2 + spacing, 150);
	}
	else if ((buttons & EButtons::Cancel) == EButtons::Cancel)
	{
		waitCancelButton.setXY((windowWidth-buttonWidth)/2, 150);
	}
	else
	{
		waitOKButton.setXY((windowWidth-buttonWidth)/2, 150);
	}

	waitText.setText(title);
	if ( subtitle != nullptr )
		waitSubText.setText(title);
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
		ShowWaitWindow(ErrorCodeText(result), nullptr, EButtons::OK);
	}
}


void DCCConfigView::waitOKButtonClickHandler(const touchgfx::AbstractButton& src)
{
	if (state == EState::DeleteDecoder && selectedDecoderItem > 0 )
	{
		uiDecodersConfig.deleteDecoder(selectedDecoderItem);
		selectedDecoderItem = -1;
		scrollWheelDecoders.setNumberOfItems(uiDecodersConfig.Count() + 1);
		scrollWheelDecoders.invalidate();
	}
	
	CloseWaitWindow(EErrorCode::Success);
	state = EState::Editting;
}


void DCCConfigView::waitCancelButtonClickHandler(const touchgfx::AbstractButton& src)
{
	if (state == EState::ScanningAllCVs)
	{
		presenter->StopScanAllCVsTrack();
	}
	else
	{
		CloseWaitWindow(EErrorCode::Success);
		state = EState::Editting;
	}
}


void DCCConfigView::selectListOKButtonClickHandler(const touchgfx::AbstractButton& src)
{
	if (state == EState::SelectDecoderDef )
	{
		Decoders &d = uiDecodersConfig[selectedDecoderItem];
		int id = cboSelectList.getSelectedId();
		if (id < 0)
			d.setDecoderDefFilename("");
		else
			d.setDecoderDefFilename(decoderDefinitions[id]);
		displayDecoder();
	}
	
	selectWindow.setVisible(false);
	selectWindow.invalidate();
	remove(selectWindow);
	state = EState::Editting;
}


void DCCConfigView::selectListCancelButtonClickHandler(const touchgfx::AbstractButton& src)
{
	selectWindow.setVisible(false);
	selectWindow.invalidate();
	remove(selectWindow);
	state = EState::Editting;
}
