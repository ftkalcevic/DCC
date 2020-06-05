#include <gui/dccsettings_screen/DCCSettingsView.hpp>
#include <gui/dccsettings_screen/DCCSettingsPresenter.hpp>

DCCSettingsPresenter::DCCSettingsPresenter(DCCSettingsView& v)
    : view(v)
{

}

void DCCSettingsPresenter::activate()
{
	config.parse();
	view.setSettings( config.getTripCurrent(), config.getToff(), config.getSlewRate() );
	TrackStatusChanged();
}

void DCCSettingsPresenter::deactivate()
{
	// TODO this could go in dcc.cpp, or at least let it know to reload settings.
	int tripCurrent, toff, slewRate;
	view.getSettings(tripCurrent, toff, slewRate);
	config.setTripCurrent(tripCurrent);
	config.setToff(toff);
	config.setTripCurrent(tripCurrent);
	if (config.isDirty())
		config.write();
}


void DCCSettingsPresenter::TrackStatusChanged()
{
	view.setTrackStatus(model->getMainTrackStatus(), model->getProgTrackStatus());
}
