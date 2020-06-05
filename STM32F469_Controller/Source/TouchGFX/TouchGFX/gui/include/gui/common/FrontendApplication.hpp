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
	void gotoPreferencesNorth();
	void gotoPreferencesSouth();
    void gotoSettingsScreen();
    void gotoDCCSettingsScreen();
    void gotoAboutScreen();
	
private:
	void gotoPreferencesNorthImpl();
	void gotoPreferencesSouthImpl();
    void gotoSettingsScreenImpl();
    void gotoDCCSettingsScreenImpl();
    void gotoAboutScreenImpl();
    touchgfx::Callback<FrontendApplication> transitionCallback;
};

#endif // FRONTENDAPPLICATION_HPP
