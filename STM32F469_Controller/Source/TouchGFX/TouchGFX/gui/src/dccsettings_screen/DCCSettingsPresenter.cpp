#include <gui/dccsettings_screen/DCCSettingsView.hpp>
#include <gui/dccsettings_screen/DCCSettingsPresenter.hpp>

DCCSettingsPresenter::DCCSettingsPresenter(DCCSettingsView& v)
    : view(v)
{

}

void DCCSettingsPresenter::activate()
{
	view.setSettings( model->getTripCurrent(), model->getToff(), model->getSlewRate() );
	TrackStatusChanged();

}

void DCCSettingsPresenter::deactivate()
{

}


void DCCSettingsPresenter::TrackStatusChanged()
{
	view.setTrackStatus(model->getMainTrackStatus(), model->getProgTrackStatus());
}
