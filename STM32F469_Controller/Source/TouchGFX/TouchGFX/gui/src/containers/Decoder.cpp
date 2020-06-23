#include <gui/containers/Decoder.hpp>
#include <touchgfx/Color.hpp>
#include <stdio.h>

#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include "Decoders.h"


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
    textSpeedValue.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTIDCENTER));

    textBrakeValue.setXY(297, 314);
    textBrakeValue.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textBrakeValue.setLinespacing(0);
    textBrakeValue.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTIDCENTER));

    imageDecoder.setXY(170, 54);
	//imageDecoder.setScalingAlgorithm(ScalableImage::NEAREST_NEIGHBOR);
    imageDecoder.setClickAction(imageClickedCallback);
	
    textTitle.setXY(206, 9);
    textTitle.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textTitle.setLinespacing(0);
    textTitle.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTIDMEDIUMCENTERED));

    textDescription.setXY(189, 210);
    textDescription.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textDescription.setLinespacing(0);
    textDescription.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTIDCENTER));

    textPosition.setXY(206, 237);
    textPosition.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textPosition.setLinespacing(0);
    textPosition.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTIDCENTER));

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
	
	if (index < 0 || index >= decoderCount)
		return;

	Decoders &d = decoders[index];
	
	bool isMultiFunction = d.type == EDecoderType::Multifunction;
	SetWidgetVisibility(isMultiFunction, isMultiFunction ? d.loco.controlled : false);

	if (*d.name)
	{
		textTitle.setWildcard((const Unicode::UnicodeChar*)d.name);
	}
	else
		textTitle.setVisible(false);
	if (*d.description)
		textDescription.setWildcard((const Unicode::UnicodeChar*)d.description);
	else
		textDescription.setVisible(false);	
	
	if (isMultiFunction)
		SetupMultiFunction(d);
	else
		SetupAccessory(d);

}


void Decoder::SetupMultiFunction(Decoders &decoder)
{
    boxDecoderBackground.setBorderSize(decoder.loco.controlled ? 10 : 0);
	imageDecoder.setBitmap(touchgfx::Bitmap(BITMAP_LOCOICON_ID));
	imageDecoder.setPosition(20, 15, 380, 210);
}

void Decoder::SetupAccessory(Decoders &decoder)
{
	if ( decoder.accessory.currentState == 0 )
	    imageDecoder.setBitmap(touchgfx::Bitmap(BITMAP_TURNOUT_STRAIGHT_NORMAL_ID));
	else
	    imageDecoder.setBitmap(touchgfx::Bitmap(BITMAP_TURNOUT_STRAIGHT_REVERSE_ID));
	imageDecoder.setPosition(20, 15, 380, 210);
}


void Decoder::imageClickHandler(const ScalableAspectImage& img, const ClickEvent& e)
{
	Decoders &d = decoders[index];
	if (d.type == EDecoderType::Accessory && e.getType() == ClickEvent::PRESSED )
	{
		d.accessory.currentState = (d.accessory.currentState + 1) & 1;
		SetupAccessory(d);
		imageDecoder.invalidate();
	}
}

void Decoder::TakeControl(bool control)
{
	Decoders &d = decoders[index];
	if (d.type == EDecoderType::Multifunction )
	{
		//presenter.LocoTakeControl(d.address, control);
		// TODO Call method to take control - possible yield others (multi-control)
		// Send message.
		//      picked up by AppMain to start sending dcc messages
		//          maybe disable other locos
		//      update the function keys
		//      
		d.loco.controlled = control;
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

	
