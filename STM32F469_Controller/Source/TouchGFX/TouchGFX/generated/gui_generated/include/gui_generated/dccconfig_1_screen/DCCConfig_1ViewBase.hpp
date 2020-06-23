/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef DCCCONFIG_1VIEWBASE_HPP
#define DCCCONFIG_1VIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/dccconfig_1_screen/DCCConfig_1Presenter.hpp>
#include <touchgfx/widgets/TiledImage.hpp>
#include <gui/containers/StatusBar.hpp>
#include <gui/containers/FunctionButtons.hpp>

class DCCConfig_1ViewBase : public touchgfx::View<DCCConfig_1Presenter>
{
public:
    DCCConfig_1ViewBase();
    virtual ~DCCConfig_1ViewBase() {}
    virtual void setupScreen();

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::TiledImage backgroundImage;
    StatusBar statusBar;
    FunctionButtons functionButtons;

private:

};

#endif // DCCCONFIG_1VIEWBASE_HPP