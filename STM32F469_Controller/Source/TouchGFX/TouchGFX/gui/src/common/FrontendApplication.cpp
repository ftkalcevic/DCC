#include <gui/common/FrontendApplication.hpp>
#include <gui/preferences_screen/PreferencesView.hpp>
#include <gui/preferences_screen/PreferencesPresenter.hpp>
#include <gui/settings_screen/SettingsView.hpp>
#include <gui/settings_screen/SettingsPresenter.hpp>
#include <gui/dccsettings_screen/DCCSettingsView.hpp>
#include <gui/dccsettings_screen/DCCSettingsPresenter.hpp>
#include <gui/about_screen/AboutView.hpp>
#include <gui/about_screen/AboutPresenter.hpp>
#include <gui/decoders_screen/DecodersView.hpp>
#include <gui/decoders_screen/DecodersPresenter.hpp>
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


void FrontendApplication::returnToMainScreen()
{
    transitionCallback = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::returnToMainScreenImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplication::returnToMainScreenImpl()
{
    touchgfx::makeTransition<MainView, MainPresenter, touchgfx::UncoverTransition<SOUTH>, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}



void FrontendApplication::gotoPreferences()
{
    transitionCallback = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoPreferencesImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplication::gotoPreferencesImpl()
{
    touchgfx::makeTransition<PreferencesView, PreferencesPresenter, touchgfx::CoverTransition<NORTH>, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}

void FrontendApplication::returnToPreferences()
{
    transitionCallback = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::returnToPreferencesImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplication::returnToPreferencesImpl()
{
    touchgfx::makeTransition<PreferencesView, PreferencesPresenter, touchgfx::UncoverTransition<SOUTH>, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
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



void FrontendApplication::gotoDCCConfigScreen()
{
    transitionCallback = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoDCCConfigScreenImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplication::gotoDCCConfigScreenImpl()
{
    touchgfx::makeTransition<DCCConfigView, DCCConfigPresenter, touchgfx::CoverTransition<NORTH>, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}


void FrontendApplication::gotoDecodersScreen()
{
    transitionCallback = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoDecodersScreenImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplication::gotoDecodersScreenImpl()
{
    touchgfx::makeTransition<DecodersView, DecodersPresenter, touchgfx::CoverTransition<NORTH>, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}

