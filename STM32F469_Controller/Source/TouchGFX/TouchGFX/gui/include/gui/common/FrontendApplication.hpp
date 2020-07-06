#ifndef FRONTENDAPPLICATION_HPP
#define FRONTENDAPPLICATION_HPP

#include <gui_generated/common/FrontendApplicationBase.hpp>

class FrontendHeap;

using namespace touchgfx;

class FrontendApplication : public FrontendApplicationBase
{
public:
    FrontendApplication(Model& m, FrontendHeap& heap);
    virtual ~FrontendApplication() { }

    virtual void handleTickEvent()
    {
        model.tick();
        FrontendApplicationBase::handleTickEvent();
    }
	void returnToMainScreen();
	void gotoPreferences();
	void returnToPreferences();
    void gotoSettingsScreen();
    void gotoDCCSettingsScreen();
    void gotoAboutScreen();
    void gotoDCCConfigScreen();
    void gotoDecodersScreen();
    void gotoAudioConfigScreen();
	
private:
	void returnToMainScreenImpl();
	void gotoPreferencesImpl();
	void returnToPreferencesImpl();
    void gotoSettingsScreenImpl();
    void gotoDCCSettingsScreenImpl();
    void gotoAboutScreenImpl();
    void gotoDCCConfigScreenImpl();
    void gotoDecodersScreenImpl();
    void gotoAudioConfigScreenImpl();
    touchgfx::Callback<FrontendApplication> transitionCallback;
};

#endif // FRONTENDAPPLICATION_HPP
