#ifndef DECODER_HPP
#define DECODER_HPP

#include <gui_generated/containers/DecoderBase.hpp>
#include <touchgfx/widgets/BoxWithBorder.hpp>
#include <touchgfx/containers/Slider.hpp>
#include <touchgfx/widgets/RadioButton.hpp>
#include <touchgfx/widgets/TextAreaWithWildcard.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/ButtonWithLabel.hpp>
#include <touchgfx/widgets/RadioButtonGroup.hpp>
#include <touchgfx/mixins/ClickListener.hpp>
#include "ScalableAspectImage.h"
#include "Decoders.h"


class Decoder : public DecoderBase
{
public:
    Decoder();
    virtual ~Decoder() {}

    virtual void initialize();
	void setIndex(int i);
    void imageClickHandler(const ScalableAspectImage& img, const ClickEvent& e);
	void takeControlClickedCallback(const touchgfx::AbstractButton& src);
	void yieldControlClickedCallback(const touchgfx::AbstractButton& src);

	
protected:
    touchgfx::BoxWithBorder boxDecoderBackground;
    touchgfx::Slider sliderSpeed;
    touchgfx::Slider sliderBrake;
    touchgfx::RadioButton radioButtonRev;
    touchgfx::RadioButton radioButtonFwd;
    touchgfx::TextAreaWithOneWildcard textSpeedValue;
    touchgfx::TextAreaWithOneWildcard textBrakeValue;
    touchgfx::ClickListener< touchgfx::ScalableAspectImage > imageDecoder;
    touchgfx::TextAreaWithOneWildcard textTitle;
    touchgfx::TextAreaWithOneWildcard textDescription;
    touchgfx::TextAreaWithOneWildcard textPosition;
    touchgfx::ButtonWithLabel buttonTakeControl;
    touchgfx::ButtonWithLabel buttonYieldControl;
    touchgfx::RadioButtonGroup<2> radioButtonGroup1;
	
	int index;
	
	void SetWidgetVisibility(bool isMultiFunction, bool controlled);
	void SetupMultiFunction(Decoders &decoder);
	void SetupAccessory(Decoders &decoder);
	void TakeControl(bool control);

protected:
    // Declaring callback type of box and clickEvent
    Callback<Decoder, const ScalableAspectImage&, const ClickEvent&> imageClickedCallback;    
    touchgfx::Callback<Decoder, const touchgfx::AbstractButton&> takeControlClickCallback;
    touchgfx::Callback<Decoder, const touchgfx::AbstractButton&> yieldControlClickCallback;
};

#endif // DECODER_HPP
