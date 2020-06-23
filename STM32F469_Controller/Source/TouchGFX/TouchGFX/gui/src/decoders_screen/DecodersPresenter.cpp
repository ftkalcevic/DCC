#include <gui/decoders_screen/DecodersView.hpp>
#include <gui/decoders_screen/DecodersPresenter.hpp>

DecodersPresenter::DecodersPresenter(DecodersView& v)
    : view(v)
{
	
}

void DecodersPresenter::activate()
{

}

void DecodersPresenter::deactivate()
{

}

void DecodersPresenter::TakeControl(int decoderIndex, bool control)
{
	model->TakeControl(decoderIndex,control);
	view.invalidate();
}

