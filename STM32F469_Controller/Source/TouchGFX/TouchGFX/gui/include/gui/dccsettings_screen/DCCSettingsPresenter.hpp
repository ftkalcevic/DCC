#ifndef DCCSETTINGSPRESENTER_HPP
#define DCCSETTINGSPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>
#include "Config.h"


using namespace touchgfx;

class DCCSettingsView;

class DCCSettingsPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    DCCSettingsPresenter(DCCSettingsView& v);

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

    virtual ~DCCSettingsPresenter() {};
	virtual void TrackStatusChanged();

private:
    DCCSettingsPresenter();

    DCCSettingsView& view;
	DCCConfig config;
};

#endif // DCCSETTINGSPRESENTER_HPP
