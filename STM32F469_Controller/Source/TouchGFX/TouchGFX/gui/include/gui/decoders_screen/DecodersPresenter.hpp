#ifndef DECODERSPRESENTER_HPP
#define DECODERSPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class DecodersView;

class DecodersPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    DecodersPresenter(DecodersView& v);

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

    virtual ~DecodersPresenter() {};

private:
    DecodersPresenter();

    DecodersView& view;
};

#endif // DECODERSPRESENTER_HPP
