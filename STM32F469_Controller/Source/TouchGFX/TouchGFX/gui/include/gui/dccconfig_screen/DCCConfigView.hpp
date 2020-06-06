#ifndef DCCCONFIGVIEW_HPP
#define DCCCONFIGVIEW_HPP

#include <gui_generated/dccconfig_screen/DCCConfigViewBase.hpp>
#include <gui/dccconfig_screen/DCCConfigPresenter.hpp>
#include <gui/common/ButtonWithLabelAndEnable.hpp>

class DCCConfigView : public DCCConfigViewBase
{
public:
    DCCConfigView();
    virtual ~DCCConfigView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
	touchgfx::ButtonWithLabelAndEnable buttonScanTrack;
    touchgfx::ButtonWithLabelAndEnable buttonReadAllCVs;

	
    touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> buttonProgTrackClickCallback;
    void buttonProgTrackClickHandler(const touchgfx::AbstractButton& src);
    touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> buttonScanTrackClickCallback;
    void buttonScanTrackClickHandler(const touchgfx::AbstractButton& src);
    touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> buttonReadAllCVsClickCallback;
    void buttonReadAllCVsClickHandler(const touchgfx::AbstractButton& src);
	void handleGestureEvent(const GestureEvent & evt);
		
};

#endif // DCCCONFIGVIEW_HPP
