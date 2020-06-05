#ifndef ABOUTPRESENTER_HPP
#define ABOUTPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class AboutView;

class AboutPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    AboutPresenter(AboutView& v);

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

    virtual ~AboutPresenter() {};

private:
    AboutPresenter();

    AboutView& view;
};

#endif // ABOUTPRESENTER_HPP
