#include <gui/containers/Decoder.hpp>
#include <touchgfx/Color.hpp>
#include <stdio.h>

#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include "DecodersConfig.h"
#include "BitmapManager.h"

Decoder::Decoder()
	: index(-1)
    , imageClickedCallback(this, &Decoder::imageClickHandler)
	, takeControlClickCallback(this, &Decoder::takeControlClickedCallback)
	, yieldControlClickCallback(this, &Decoder::yieldControlClickedCallback)
{
	// Build the contents
    boxDecoderBackground.setPosition(0, 0, 420, 420);
    boxDecoderBackground.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    boxDecoderBackground.setBorderColor(touchgfx::Color::getColorFrom24BitRGB(255, 0, 0));
    boxDecoderBackground.setBorderSize(0);

    sliderSpeed.setXY(41, 265);
    sliderSpeed.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_MEDIUM_SLIDER2_ROUND_EDGE_BACK_ID), touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_MEDIUM_SLIDER2_ROUND_EDGE_BACK_ID), touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_MEDIUM_INDICATORS_SLIDER2_ROUND_EDGE_NOB_ID));
    sliderSpeed.setupHorizontalSlider(2, 6, 0, 0, 284);
    sliderSpeed.setValueRange(0, 100);
    sliderSpeed.setValue(0);

    sliderBrake.setXY(200, 309);
    sliderBrake.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_SMALL_SLIDER_HORIZONTAL_SMALL_ROUND_EDGE_BACK_ID), touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_SMALL_SLIDER_HORIZONTAL_SMALL_ROUND_EDGE_BACK_ID), touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_SMALL_INDICATORS_SLIDER_HORIZONTAL_SMALL_ROUND_EDGE_KNOB_ID));
    sliderBrake.setupHorizontalSlider(3, 7, 0, 0, 125);
    sliderBrake.setValueRange(0, 100);
    sliderBrake.setValue(0);

    radioButtonRev.setXY(11, 349);
    radioButtonRev.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID));
    radioButtonRev.setSelected(false);
    radioButtonRev.setDeselectionEnabled(false);

    radioButtonFwd.setXY(112, 349);
    radioButtonFwd.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID));
    radioButtonFwd.setSelected(false);
    radioButtonFwd.setDeselectionEnabled(false);

    textSpeedValue.setXY(210, 269);
    textSpeedValue.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textSpeedValue.setLinespacing(0);
    textSpeedValue.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTCENTER20PXID));

    textBrakeValue.setXY(297, 314);
    textBrakeValue.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textBrakeValue.setLinespacing(0);
    textBrakeValue.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTCENTER20PXID));

    imageDecoder.setXY(170, 54);
	//imageDecoder.setScalingAlgorithm(ScalableImage::NEAREST_NEIGHBOR);
    imageDecoder.setClickAction(imageClickedCallback);
	
    textTitle.setXY(206, 9);
    textTitle.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textTitle.setLinespacing(0);
    textTitle.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTCENTERED40PXID));

    textDescription.setXY(189, 210);
    textDescription.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textDescription.setLinespacing(0);
    textDescription.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTCENTER20PXID));

    textPosition.setXY(206, 237);
    textPosition.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textPosition.setLinespacing(0);
    textPosition.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTCENTER20PXID));

    buttonTakeControl.setXY(240, 349);
    buttonTakeControl.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID));
    buttonTakeControl.setLabelText(touchgfx::TypedText(T_RESOURCEID3));
    buttonTakeControl.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    buttonTakeControl.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    buttonTakeControl.setAction(takeControlClickCallback);
	
    buttonYieldControl.setXY(240, 349);
    buttonYieldControl.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID));
    buttonYieldControl.setLabelText(touchgfx::TypedText(T_RESOURCEID2));
    buttonYieldControl.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    buttonYieldControl.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    buttonYieldControl.setAction(yieldControlClickCallback);

    add(boxDecoderBackground);
    add(sliderSpeed);
    add(sliderBrake);
    add(radioButtonRev);
    add(radioButtonFwd);
    add(textSpeedValue);
    add(textBrakeValue);
    add(imageDecoder);
    add(textTitle);
    add(textDescription);
    add(textPosition);
    add(buttonTakeControl);
    add(buttonYieldControl);
    radioButtonGroup1.add(radioButtonRev);
    radioButtonGroup1.add(radioButtonFwd);
}

void Decoder::initialize()
{
    DecoderBase::initialize();
}

void Decoder::SetWidgetVisibility(bool isMultiFunction, bool controlled)
{
    boxDecoderBackground.setVisible(true);
	boxDecoderBackground.setBorderSize(0);
    sliderSpeed.setVisible(isMultiFunction);
    sliderBrake.setVisible(isMultiFunction);
    radioButtonRev.setVisible(isMultiFunction);
    radioButtonFwd.setVisible(isMultiFunction);
    textSpeedValue.setVisible(isMultiFunction);
    textBrakeValue.setVisible(isMultiFunction);
    imageDecoder.setVisible(true);
    textTitle.setVisible(true);
    textDescription.setVisible(true);
    textPosition.setVisible(!isMultiFunction);;
    buttonTakeControl.setVisible(isMultiFunction && !controlled);;
    buttonYieldControl.setVisible(isMultiFunction && controlled);;
}


void Decoder::setIndex(int newIndex) 
{ 
	index = newIndex;
	
	if (index < 0 || index >= uiDecodersConfig.Count())
		return;

	Decoders &d = uiDecodersConfig[index];
	
	bool isMultiFunction = d.getType() == EDecoderType::Multifunction;
	SetWidgetVisibility(isMultiFunction, isMultiFunction ? d.getLoco().controlled : false);

	if (*d.getName())
	{
		textTitle.setWildcard((const Unicode::UnicodeChar*)d.getName());
	}
	else
		textTitle.setVisible(false);
	if (*d.getDescription())
		textDescription.setWildcard((const Unicode::UnicodeChar*)d.getDescription());
	else
		textDescription.setVisible(false);	
	
	if (isMultiFunction)
		SetupMultiFunction(d);
	else
		SetupAccessory(d);

}


void Decoder::SetupMultiFunction(Decoders &decoder)
{
    boxDecoderBackground.setBorderSize(decoder.getLoco().controlled ? 10 : 0);
	BitmapId bmpId;
	if (decoder.getLargeImageType() == EUserImage::UserFile)
	{
		bmpId = BitmapManager::FindBitmap(decoder.getLargeImageFile());
	}
	else
	{
		bmpId = EUserImageClass::UserImage(decoder.getLargeImageType()).bmpId;
	}
	imageDecoder.setBitmap(touchgfx::Bitmap(bmpId));
	imageDecoder.setFlipHorizontal(decoder.getLargeImageAttributes() & EImageAttributes::FlipHorizontal);
	imageDecoder.setFlipVertical(decoder.getLargeImageAttributes() & EImageAttributes::FlipVertical);
	imageDecoder.setPosition(20, 15, 380, 210);
}

void Decoder::SetupAccessory(Decoders &decoder)
{
	if ( decoder.getAcc().currentState == 0 )
	    imageDecoder.setBitmap(touchgfx::Bitmap(BITMAP_TURNOUT_STRAIGHT_NORMAL_ID));
	else
	    imageDecoder.setBitmap(touchgfx::Bitmap(BITMAP_TURNOUT_STRAIGHT_REVERSE_ID));
	imageDecoder.setPosition(20, 15, 380, 210);
}


void Decoder::imageClickHandler(const ScalableAspectImage& img, const ClickEvent& e)
{
	Decoders &d = uiDecodersConfig[index];
	if (d.getType() == EDecoderType::Accessory && e.getType() == ClickEvent::PRESSED )
	{
		d.getAcc().currentState = (d.getAcc().currentState + 1) & 1;
		SetupAccessory(d);
		imageDecoder.invalidate();
	}
}

void Decoder::TakeControl(bool control)
{
	Decoders &d = uiDecodersConfig[index];
	if (d.getType() == EDecoderType::Multifunction )
	{
		//presenter.LocoTakeControl(d.address, control);
		// TODO Call method to take control - possible yield others (multi-control)
		// Send message.
		//      picked up by AppMain to start sending dcc messages
		//          maybe disable other locos
		//      update the function keys
		//      
		d.getLoco().controlled = control;
		boxDecoderBackground.setBorderSize(control ? 10 : 0);
        buttonTakeControl.setVisible(!control);;
        buttonYieldControl.setVisible(control);;
		invalidate();
	}
}


void Decoder::takeControlClickedCallback(const touchgfx::AbstractButton& src)
{
	//	TakeControl(true);
	emitTakeControl(index,true);
}

void Decoder::yieldControlClickedCallback(const touchgfx::AbstractButton& src)
{
//	TakeControl(false);
	emitTakeControl(index,false);
}

