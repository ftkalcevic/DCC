#ifndef PREFERENCESPRESENTER_HPP
#define PREFERENCESPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class PreferencesView;

class PreferencesPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    PreferencesPresenter(PreferencesView& v);

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

    virtual ~PreferencesPresenter() {};

private:
    PreferencesPresenter();

    PreferencesView& view;
};

#endif // PREFERENCESPRESENTER_HPP
