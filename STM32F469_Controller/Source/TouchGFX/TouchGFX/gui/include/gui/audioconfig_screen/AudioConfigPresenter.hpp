#ifndef AUDIOCONFIGPRESENTER_HPP
#define AUDIOCONFIGPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class AudioConfigView;

class AudioConfigPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    AudioConfigPresenter(AudioConfigView& v);

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

    virtual ~AudioConfigPresenter() {};

private:
    AudioConfigPresenter();

    AudioConfigView& view;
};

#endif // AUDIOCONFIGPRESENTER_HPP
