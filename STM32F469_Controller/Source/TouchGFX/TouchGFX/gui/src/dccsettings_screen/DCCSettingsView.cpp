#include <gui/dccsettings_screen/DCCSettingsView.hpp>
#include <touchgfx/Color.hpp>

DCCSettingsView::DCCSettingsView()
{
	tripCurrentWheel.setNumberOfItems(4);
	toffWheel.setNumberOfItems(4);
	slewRateWheel.setNumberOfItems(8);
}

void DCCSettingsView::setupScreen()
{
    DCCSettingsViewBase::setupScreen();
}

void DCCSettingsView::tearDownScreen()
{
    DCCSettingsViewBase::tearDownScreen();
}

void DCCSettingsView::tripCurrentWheelUpdateItem(scrollWheelItem& item, int16_t itemIndex)
{
	const char16_t *text=u"";
	switch (itemIndex)
	{
		case 0: text = u"4A"; break;
		case 1: text = u"5.4A"; break;
		case 2: text = u"6.5A"; break;
		case 3: text = u"7A"; break;
	}
	item.setText(text);
}


void DCCSettingsView::toffWheelUpdateItem(scrollWheelItem& item, int16_t itemIndex)
{
	const char16_t *text=u"";
	switch (itemIndex)
	{
		case 0: text = u"20us"; break;
		case 1: text = u"40us"; break;
		case 2: text = u"60us"; break;
		case 3: text = u"80us"; break;
	}
	item.setText(text);}

void DCCSettingsView::slewRateWheelUpdateItem(scrollWheelItem& item, int16_t itemIndex)
{
	const char16_t *text=u"";
	switch (itemIndex)
	{
		case 0: text = u"53.2 V/us"; break;
		case 1: text = u"34 V/us"; break;
		case 2: text = u"18.3 V/us"; break;
		case 3: text = u"13 V/us"; break;
		case 4: text = u"10.8 V/us"; break;
		case 5: text = u"7.9 V/us"; break;
		case 6: text = u"5.3 V/us"; break;
		case 7: text = u"2.6 V/us"; break;
	}
	item.setText(text);
}


void DCCSettingsView::handleGestureEvent(const GestureEvent & evt)
{
	//printf("PreferencesView::%d gesture %d %d\n", count++, evt.getType(), evt.getVelocity());
	if (evt.getType() == GestureEvent::SWIPE_VERTICAL && evt.getVelocity() < -SWIPE_VELOCITY)
	{
		application().gotoPreferencesSouth();
	}
	else
		DCCSettingsViewBase::handleGestureEvent(evt); 
}



void DCCSettingsView::setSettings(int tripCurrent, int toff, int slewRate)
{
	tripCurrentWheel.animateToItem(tripCurrent,0);
	toffWheel.animateToItem(toff,0);
	slewRateWheel.animateToItem(slewRate,0);
}

void DCCSettingsView::getSettings(int &tripCurrent, int &toff, int &slewRate)
{
	tripCurrent = tripCurrentWheel.getSelectedItem();
	toff = toffWheel.getSelectedItem();
	slewRate = slewRateWheel.getSelectedItem();
}

void DCCSettingsView::setTrackStatus(DCCSettings::HBStatus mainTrack, DCCSettings::HBStatus progTrack)
{
	colortype red = touchgfx::Color::getColorFrom24BitRGB(255, 0, 0);
	colortype amber = touchgfx::Color::getColorFrom24BitRGB(255, 191, 0);
	colortype green = touchgfx::Color::getColorFrom24BitRGB(0, 255, 0);
		
    trackFaultBox.setColor(mainTrack.fault ? red : green);
    progFaultBox.setColor(progTrack.fault ? red : green);
    trackOpenLoadBox.setColor(mainTrack.openLoad ? red : green);
    progOpenLoadBox.setColor(progTrack.openLoad ? red : green);
    trackOverCurrentBox.setColor(mainTrack.overCurrent == DCCSettings::Good ? green : mainTrack.overCurrent == DCCSettings::Warning ? amber : red);
    progOverCurrentBox.setColor(progTrack.overCurrent == DCCSettings::Good ? green : progTrack.overCurrent == DCCSettings::Warning ? amber : red);
    progOverTempBox.setColor(mainTrack.overTemp == DCCSettings::Good ? green : mainTrack.overTemp == DCCSettings::Warning ? amber : red);
    trackOverTempBox.setColor(progTrack.overTemp == DCCSettings::Good ? green : progTrack.overTemp == DCCSettings::Warning ? amber : red);
	
	currentDisplay.Update(mainTrack.current, progTrack.current);
}
