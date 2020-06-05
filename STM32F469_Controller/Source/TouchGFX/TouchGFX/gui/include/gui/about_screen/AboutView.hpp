#ifndef ABOUTVIEW_HPP
#define ABOUTVIEW_HPP

#include <gui_generated/about_screen/AboutViewBase.hpp>
#include <gui/about_screen/AboutPresenter.hpp>

class AboutView : public AboutViewBase
{
public:
    AboutView();
    virtual ~AboutView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
	virtual void handleGestureEvent(const GestureEvent & evt);
protected:
};

#endif // ABOUTVIEW_HPP
