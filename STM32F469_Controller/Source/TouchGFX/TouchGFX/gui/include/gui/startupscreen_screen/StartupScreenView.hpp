#ifndef STARTUPSCREENVIEW_HPP
#define STARTUPSCREENVIEW_HPP

#include <gui_generated/startupscreen_screen/StartupScreenViewBase.hpp>
#include <gui/startupscreen_screen/StartupScreenPresenter.hpp>

class StartupScreenView : public StartupScreenViewBase
{
public:
    StartupScreenView();
    virtual ~StartupScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // STARTUPSCREENVIEW_HPP
