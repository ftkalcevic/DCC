#ifndef DCCCONFIGPRESENTER_HPP
#define DCCCONFIGPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class DCCConfigView;

class DCCConfigPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    DCCConfigPresenter(DCCConfigView& v);

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

    virtual ~DCCConfigPresenter() {};

	void EnableProgTrack(bool enable);
	void ScanProgrammingTrack();
	
private:
    DCCConfigPresenter();

    DCCConfigView& view;
};

#endif // DCCCONFIGPRESENTER_HPP
