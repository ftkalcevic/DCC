#ifndef DCCSETTINGSVIEW_HPP
#define DCCSETTINGSVIEW_HPP

#include <gui_generated/dccsettings_screen/DCCSettingsViewBase.hpp>
#include <gui/dccsettings_screen/DCCSettingsPresenter.hpp>

class DCCSettingsView : public DCCSettingsViewBase
{
public:
    DCCSettingsView();
    virtual ~DCCSettingsView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
	virtual void tripCurrentWheelUpdateItem(scrollItem& item, int16_t itemIndex);
	virtual void toffWheelUpdateItem(scrollItem& item, int16_t itemIndex);
	virtual void slewRateWheelUpdateItem(scrollItem& item, int16_t itemIndex);

	void setSettings( int tripCurrent, int toff, int slewRate );
	void getSettings( int &tripCurrent, int &toff, int &slewRate );
	void setTrackStatus(DCCSettings::HBStatus mainTrack, DCCSettings::HBStatus progTrack);
protected:
	void handleGestureEvent(const GestureEvent & evt);
};

#endif // DCCSETTINGSVIEW_HPP
