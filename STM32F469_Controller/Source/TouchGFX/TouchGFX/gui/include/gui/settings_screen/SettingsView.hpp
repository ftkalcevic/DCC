#ifndef SETTINGSVIEW_HPP
#define SETTINGSVIEW_HPP

#include <gui_generated/settings_screen/SettingsViewBase.hpp>
#include <gui/settings_screen/SettingsPresenter.hpp>
#include "AnalogInputConfigWidget.h"
#include "ToggleButtonIconAndText.h"

class SettingsView : public SettingsViewBase
{
public:
    SettingsView();
    virtual ~SettingsView() {}
	
	ToggleButtonIconAndText toggleCalibrate2;
    ToggleButtonIconAndText buttonRev;
    ToggleButtonIconAndText buttonFwd;
    ToggleButtonIconAndText buttonStop;
		
		
    virtual void setupScreen();
    virtual void tearDownScreen();
	void handleGestureEvent(const GestureEvent & evt);
	void UpdateInputs(int throttle, int brake, int throttleRaw, int brakeRaw, EDirection::EDirection &direction);
    void UpdateDirection(const EDirection::EDirection dir, bool force=false);


protected:
	AnalogInputConfigWidget sliderThrottle;
	AnalogInputConfigWidget sliderBrake;
    EDirection::EDirection lastDirection;
	bool calibrateFirst;

    touchgfx::Callback<SettingsView, const touchgfx::AbstractButton&> buttonCalibrateClickCallback;
    void buttonCalibrateClickHandler(const touchgfx::AbstractButton& src);
    touchgfx::Callback<SettingsView, const CheckBox&> checkBoxCallback;
    void checkBoxHandler(const CheckBox& src);
};

#endif // SETTINGSVIEW_HPP
