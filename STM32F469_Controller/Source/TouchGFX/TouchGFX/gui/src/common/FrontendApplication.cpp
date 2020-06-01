#include <gui/common/FrontendApplication.hpp>
#include <gui/preferences_screen/PreferencesView.hpp>
#include <gui/preferences_screen/PreferencesPresenter.hpp>
#include <touchgfx/transitions/NoTransition.hpp>

#include <new>
#include <gui/common/FrontendHeap.hpp>
#include <touchgfx/transitions/SlideTransition.hpp>
#include <touchgfx/transitions/CoverTransition.hpp>

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap)
{
}

void FrontendApplication::gotoPreferencesScreenSlideTransitionEast()
{
    transitionCallback = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoPreferencesScreenSlideTransitionEastImpl);
    pendingScreenTransitionCallback = &transitionCallback;
}

void FrontendApplication::gotoPreferencesScreenSlideTransitionEastImpl()
{
    touchgfx::makeTransition<PreferencesView, PreferencesPresenter, touchgfx::CoverTransition<EAST>, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}
