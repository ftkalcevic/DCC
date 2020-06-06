#include <gui/settings_screen/SettingsView.hpp>
#include "Common.h"

SettingsView::SettingsView()
{

}

void SettingsView::setupScreen()
{
    SettingsViewBase::setupScreen();
}

void SettingsView::tearDownScreen()
{
    SettingsViewBase::tearDownScreen();
}


void SettingsView::handleGestureEvent(const GestureEvent & evt)
{
	//printf("PreferencesView::%d gesture %d %d\n", count++, evt.getType(), evt.getVelocity());
	if (evt.getType() == GestureEvent::SWIPE_VERTICAL && evt.getVelocity() < -SWIPE_VELOCITY )
	{
		application().gotoPreferencesSouth();
	}
	else
		SettingsViewBase::handleGestureEvent(evt); 
}
