#ifndef PLACEHOLDERSCREENVIEW_HPP
#define PLACEHOLDERSCREENVIEW_HPP

#include <gui_generated/placeholderscreen_screen/placeHolderScreenViewBase.hpp>
#include <gui/placeholderscreen_screen/placeHolderScreenPresenter.hpp>

class placeHolderScreenView : public placeHolderScreenViewBase
{
public:
    placeHolderScreenView();
    virtual ~placeHolderScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // PLACEHOLDERSCREENVIEW_HPP
