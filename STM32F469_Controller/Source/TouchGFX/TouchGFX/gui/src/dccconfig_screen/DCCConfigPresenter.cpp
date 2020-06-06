#include <gui/dccconfig_screen/DCCConfigView.hpp>
#include <gui/dccconfig_screen/DCCConfigPresenter.hpp>

DCCConfigPresenter::DCCConfigPresenter(DCCConfigView& v)
    : view(v)
{

}

void DCCConfigPresenter::activate()
{

}

void DCCConfigPresenter::deactivate()
{
	EnableProgTrack(false);
}

void DCCConfigPresenter::EnableProgTrack(bool enable)
{
	model->EnableProgTrack(enable);
}

void DCCConfigPresenter::ScanProgrammingTrack()
{
	model->ScanProgrammingTrack();
}
