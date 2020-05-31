#ifndef STARTUPSCREENVIEW_HPP
#define STARTUPSCREENVIEW_HPP

#include <gui_generated/startupscreen_screen/StartupScreenViewBase.hpp>
#include <gui/startupscreen_screen/StartupScreenPresenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/Image.hpp>

class StartupScreenView : public StartupScreenViewBase
{
public:
    StartupScreenView();
    virtual ~StartupScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
    touchgfx::Box box1;
    touchgfx::TextArea textArea1;
    touchgfx::Image image1;
	
	bool LoadSplashImage();
	BitmapId bmpId;
};

#endif // STARTUPSCREENVIEW_HPP
