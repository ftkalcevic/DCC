#ifndef PLACEHOLDERSCREENPRESENTER_HPP
#define PLACEHOLDERSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class placeHolderScreenView;

class placeHolderScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    placeHolderScreenPresenter(placeHolderScreenView& v);

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

    virtual ~placeHolderScreenPresenter() {};

private:
    placeHolderScreenPresenter();

    placeHolderScreenView& view;
};

#endif // PLACEHOLDERSCREENPRESENTER_HPP
