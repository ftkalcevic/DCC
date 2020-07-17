#include <gui/audioconfig_screen/AudioConfigView.hpp>
#include "AudioTask.h"


AudioConfigView::AudioConfigView() :
	checkBoxMuteCallback(this, &AudioConfigView::checkBoxMuteHandler )
{
	bool isMute = audioTask.isMute();
	checkBoxMute.setAction(checkBoxMuteCallback);
	checkBoxMute.setText(u"Mute");
	checkBoxMute.setTouchable(true);
	checkBoxMute.setSelected( isMute );
}

void AudioConfigView::setupScreen()
{
    AudioConfigViewBase::setupScreen();
}

void AudioConfigView::tearDownScreen()
{
    AudioConfigViewBase::tearDownScreen();
}

void AudioConfigView::handleGestureEvent(const GestureEvent & evt)
{
	if (evt.getType() == GestureEvent::SWIPE_VERTICAL && evt.getVelocity() < -EXITSCREEN_SWIPE_VELOCITY )
	{
		application().returnToPreferences();
	}
	else
		AudioConfigViewBase::handleGestureEvent(evt); 
}


void AudioConfigView::checkBoxMuteHandler(const CheckBox& src)
{
	bool isMute = src.getSelected();
	audioTask.Mute(isMute);
	audioTask.PlaySound(EAudioSounds::KeyPressTone);
}
