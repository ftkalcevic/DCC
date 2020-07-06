#include <gui/settings_screen/SettingsView.hpp>
#include "Common.h"
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include "Config.h"
#include "AudioTask.h"


SettingsView::SettingsView() :
    buttonCalibrateClickCallback(this, &SettingsView::buttonCalibrateClickHandler),
	checkBoxCallback(this, &SettingsView::checkBoxHandler )
{
	toggleCalibrate2.setAction(buttonCalibrateClickCallback);
	
    sliderThrottle.setXY(60, 61);
    sliderThrottle.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_LARGE_SLIDER_ROUND_BACK_ID), touchgfx::Bitmap(BITMAP_INDICATORMIN_ID), touchgfx::Bitmap(BITMAP_INDICATOR_ID));
    sliderThrottle.setupHorizontalSlider(2, 22, 0, 0, 621);
    sliderThrottle.setValueRange(uiConfig.getThrottleMin(), uiConfig.getThrottleMax());
    scrollableContainer1.add(sliderThrottle);
	
	checkBoxReverseThrottle.setSelected(uiConfig.getThrottleReverse());
	checkBoxReverseThrottle.setAction(checkBoxCallback);
	checkBoxReverseThrottle.setText(u"Reverse");
	checkBoxReverseThrottle.setTouchable(false);
	
    sliderBrake.setXY(56, 223);
    sliderBrake.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_LARGE_SLIDER_ROUND_BACK_ID), touchgfx::Bitmap(BITMAP_INDICATORMIN_ID), touchgfx::Bitmap(BITMAP_INDICATOR_ID));
    sliderBrake.setupHorizontalSlider(2, 22, 0, 0, 621);
    sliderBrake.setValueRange(uiConfig.getBrakeMin(), uiConfig.getBrakeMax());
    scrollableContainer1.add(sliderBrake);
	
	checkBoxReverseBrake.setSelected(uiConfig.getBrakeReverse());
	checkBoxReverseBrake.setAction(checkBoxCallback);
	checkBoxReverseBrake.setText(u"Reverse");
	checkBoxReverseBrake.setTouchable(false);
	
	checkBoxReverseDirection.setText(u"Reverse");
	checkBoxReverseDirection.setAction(checkBoxCallback);
	checkBoxReverseDirection.setSelected(uiConfig.getDirectionReverse());
	checkBoxReverseDirection.setTouchable(false);
	
    toggleCalibrate2.setXY(117, 544);
    toggleCalibrate2.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_PRESSED_ID));
	toggleCalibrate2.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT40PXID));
	toggleCalibrate2.setText(u"Calibrate");
	toggleCalibrate2.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	toggleCalibrate2.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    scrollableContainer1.add(toggleCalibrate2);
	
    buttonRev.setXY(320, 405);
    buttonRev.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_ICONS_BACK_ARROW_32_ID), touchgfx::Bitmap(BITMAP_BLUE_ICONS_BACK_ARROW_32_ID));
	buttonRev.setTouchable(false);
    scrollableContainer1.add(buttonRev);

    buttonStop.setXY(406, 405);
    buttonStop.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_ICONS_STOP_32_ID), touchgfx::Bitmap(BITMAP_BLUE_ICONS_STOP_32_ID));
    buttonStop.setTouchable(false);
    scrollableContainer1.add(buttonStop);
	
    buttonFwd.setXY(496, 405);
    buttonFwd.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_ICONS_NEXT_ARROW_32_ID), touchgfx::Bitmap(BITMAP_BLUE_ICONS_NEXT_ARROW_32_ID));
    buttonFwd.setTouchable(false);
    scrollableContainer1.add(buttonFwd);

}

void SettingsView::setupScreen()
{
	UpdateDirection(EDirection::Stopped, true);
    SettingsViewBase::setupScreen();
}

void SettingsView::tearDownScreen()
{
    SettingsViewBase::tearDownScreen();
	if (uiConfig.isDirty())
		uiConfig.write();
}


void SettingsView::handleGestureEvent(const GestureEvent & evt)
{
	//printf("PreferencesView::%d gesture %d %d\n", count++, evt.getType(), evt.getVelocity());
	if (evt.getType() == GestureEvent::SWIPE_VERTICAL && evt.getVelocity() < -SWIPE_VELOCITY )
	{
		application().returnToPreferences();
	}
	else
		SettingsViewBase::handleGestureEvent(evt); 
}

void SettingsView::UpdateInputs(int throttle, int brake, int throttleRaw, int brakeRaw, EDirection::EDirection &direction)
{
	if ( throttleRaw != sliderThrottle.getValue() )
		sliderThrottle.setValue(throttleRaw);
	if ( brakeRaw != sliderBrake.getValue() )
		sliderBrake.setValue(brakeRaw);
	UpdateDirection(direction);
	bool calibrate = toggleCalibrate2.getState();
	if (calibrate)
	{
		if (throttleRaw < uiConfig.getThrottleMin())
			uiConfig.setThrottleMin(throttleRaw);
		if (throttleRaw > uiConfig.getThrottleMax())
			uiConfig.setThrottleMax(throttleRaw);
		if (brakeRaw < uiConfig.getBrakeMin())
			uiConfig.setBrakeMin(brakeRaw);
		if (brakeRaw > uiConfig.getBrakeMax())
			uiConfig.setBrakeMax(brakeRaw);
		
	}
}

void SettingsView::UpdateDirection(const EDirection::EDirection dir, bool force)
{
	if (force || lastDirection != dir)
	{
		buttonRev.forceState(dir == EDirection::Reverse ?  false: true );
		buttonStop.forceState(dir == EDirection::Stopped ? false: true );
		buttonFwd.forceState(dir == EDirection::Forward ?  false: true );
		buttonRev.invalidate();
		buttonStop.invalidate();
		buttonFwd.invalidate();
		lastDirection = dir;
	}
}

void SettingsView::buttonCalibrateClickHandler(const touchgfx::AbstractButton& src)
{
	audioTask.PlaySound(EAudioSounds::KeyPressTone);
	bool calibrate = toggleCalibrate2.getState();
	sliderThrottle.setAllowUpdate(calibrate);
	sliderBrake.setAllowUpdate(calibrate);
	checkBoxReverseThrottle.setTouchable(calibrate);
	checkBoxReverseBrake.setTouchable(calibrate);
	checkBoxReverseDirection.setTouchable(calibrate);
	if (calibrate)
	{
		sliderThrottle.setReverse(checkBoxReverseThrottle.getSelected());
		sliderBrake.setReverse(checkBoxReverseBrake.getSelected());
		sliderThrottle.setValueRange(sliderThrottle.getValue(), sliderThrottle.getValue());
		sliderBrake.setValueRange(sliderBrake.getValue(), sliderBrake.getValue());
	}
}


void SettingsView::checkBoxHandler(const CheckBox& src)
{
	audioTask.PlaySound(EAudioSounds::KeyPressTone);
	bool calibrate = toggleCalibrate2.getState();
	if (calibrate)
	{
		sliderThrottle.setReverse(checkBoxReverseThrottle.getSelected());
		sliderBrake.setReverse(checkBoxReverseBrake.getSelected());
	}
}



