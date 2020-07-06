#ifndef AUDIOCONFIGVIEW_HPP
#define AUDIOCONFIGVIEW_HPP

#include <gui_generated/audioconfig_screen/AudioConfigViewBase.hpp>
#include <gui/audioconfig_screen/AudioConfigPresenter.hpp>

class AudioConfigView : public AudioConfigViewBase
{
public:
    AudioConfigView();
    virtual ~AudioConfigView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
	virtual void handleGestureEvent(const GestureEvent & evt);
protected:
    touchgfx::Callback<AudioConfigView, const CheckBox&> checkBoxMuteCallback;
    void checkBoxMuteHandler(const CheckBox& src);
};

#endif // AUDIOCONFIGVIEW_HPP
