#ifndef DECODERSVIEW_HPP
#define DECODERSVIEW_HPP

#include <gui_generated/decoders_screen/DecodersViewBase.hpp>
#include <gui/decoders_screen/DecodersPresenter.hpp>

class DecodersView : public DecodersViewBase
{
public:
    DecodersView();
    virtual ~DecodersView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
    virtual void scrollWheelDecodersUpdateItem(Decoder& item, int16_t itemIndex);
    void handleGestureEvent(const GestureEvent& evt);
};

#endif // DECODERSVIEW_HPP
