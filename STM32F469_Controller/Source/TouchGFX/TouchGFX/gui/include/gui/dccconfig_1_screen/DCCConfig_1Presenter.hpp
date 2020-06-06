#ifndef DCCCONFIG_1PRESENTER_HPP
#define DCCCONFIG_1PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class DCCConfig_1View;

class DCCConfig_1Presenter : public touchgfx::Presenter, public ModelListener
{
public:
    DCCConfig_1Presenter(DCCConfig_1View& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~DCCConfig_1Presenter() {};

private:
    DCCConfig_1Presenter();

    DCCConfig_1View& view;
};

#endif // DCCCONFIG_1PRESENTER_HPP
