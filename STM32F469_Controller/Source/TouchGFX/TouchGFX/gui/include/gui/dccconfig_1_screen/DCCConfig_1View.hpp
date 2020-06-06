#ifndef DCCCONFIG_1VIEW_HPP
#define DCCCONFIG_1VIEW_HPP

#include <gui_generated/dccconfig_1_screen/DCCConfig_1ViewBase.hpp>
#include <gui/dccconfig_1_screen/DCCConfig_1Presenter.hpp>

class DCCConfig_1View : public DCCConfig_1ViewBase
{
public:
    DCCConfig_1View();
    virtual ~DCCConfig_1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // DCCCONFIG_1VIEW_HPP
