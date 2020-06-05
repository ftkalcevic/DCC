#include <gui/common/FrontendApplication.hpp>
#include <gui/preferences_screen/PreferencesView.hpp>
#include <gui/preferences_screen/PreferencesPresenter.hpp>
#include <gui/settings_screen/SettingsView.hpp>
#include <gui/settings_screen/SettingsPresenter.hpp>
#include <gui/dccsettings_screen/DCCSettingsView.hpp>
#include <gui/dccsettings_screen/DCCSettingsPresenter.hpp>
#include <gui/about_screen/AboutView.hpp>
#include <gui/about_screen/AboutPresenter.hpp>
#include <touchgfx/transitions/NoTransition.hpp>

#include <new>
#include <gui/common/FrontendHeap.hpp>
#include <touchgfx/transitions/SlideTransition.hpp>
#include <touchgfx/transitions/CoverTransition.hpp>
#include <touchgfx/transitions/UncoverTransition.hpp>

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap)
{
}

void FrontendApplication::gotoPreferencesNorth()
{
    transitionCallback = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoPreferencesNorthImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplication::gotoPreferencesNorthImpl()
{
    touchgfx::makeTransition<PreferencesView, PreferencesPresenter, touchgfx::CoverTransition<NORTH>, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}

void FrontendApplication::gotoPreferencesSouth()
{
    transitionCallback = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoPreferencesSouthImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplication::gotoPreferencesSouthImpl()
{
    touchgfx::makeTransition<PreferencesView, PreferencesPresenter, touchgfx::CoverTransition<SOUTH>, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}

void FrontendApplication::gotoSettingsScreen()
{
    transitionCallback = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoSettingsScreenImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplication::gotoSettingsScreenImpl()
{
    touchgfx::makeTransition<SettingsView, SettingsPresenter, touchgfx::CoverTransition<NORTH>, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}


void FrontendApplication::gotoDCCSettingsScreen()
{
    transitionCallback = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoDCCSettingsScreenImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplication::gotoDCCSettingsScreenImpl()
{
    touchgfx::makeTransition<DCCSettingsView, DCCSettingsPresenter, touchgfx::CoverTransition<NORTH>, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}


void FrontendApplication::gotoAboutScreen()
{
    transitionCallback = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoAboutScreenImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplication::gotoAboutScreenImpl()
{
    touchgfx::makeTransition<AboutView, AboutPresenter, touchgfx::CoverTransition<NORTH>, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}
