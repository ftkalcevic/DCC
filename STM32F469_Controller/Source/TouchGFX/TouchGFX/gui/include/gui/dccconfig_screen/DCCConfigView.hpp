#ifndef DCCCONFIGVIEW_HPP
#define DCCCONFIGVIEW_HPP

#include <gui_generated/dccconfig_screen/DCCConfigViewBase.hpp>
#include <gui/dccconfig_screen/DCCConfigPresenter.hpp>
#include <gui/common/ButtonWithLabelAndEnable.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/mixins/ClickListener.hpp>
#include "gui/common/FullKeyboard.hpp"
#include "gui/common/NumericKeypad.hpp"

class DCCConfigView : public DCCConfigViewBase
{
public:
    DCCConfigView();
    virtual ~DCCConfigView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
	
	void ScanTrackReply(int16_t address, int16_t config, int16_t extendedAddress, int16_t manufacturer, int16_t version);
	
    touchgfx::ClickListener<touchgfx::Box> boxDecoder;
    touchgfx::ClickListener<touchgfx::Box> boxAddress;
    touchgfx::ClickListener<touchgfx::Box> boxName;
    touchgfx::ClickListener<touchgfx::Box> boxDescription;
    touchgfx::Box boxConfig;
    touchgfx::TextAreaWithOneWildcard textAddress;
    touchgfx::TextAreaWithOneWildcard textName;
    touchgfx::TextAreaWithOneWildcard textDescription;
    touchgfx::TextArea textAreaLabelAddress;
    touchgfx::TextArea textAreaLabelName;
    touchgfx::TextArea textAreaLabelDescription;
    touchgfx::TextArea textAreaLabelDecoder;
    touchgfx::TextArea textAreaLabelConfig;
    touchgfx::TextArea textAreaLabelAllCVs;
    NumericKeypad numericKeypad;
    FullKeyboard keyboard;

protected:
	touchgfx::ButtonWithLabelAndEnable buttonScanTrack;
    touchgfx::ButtonWithLabelAndEnable buttonReadAllCVs;
	
	enum EState
	{
		Editting,
		Scanning,
        Keypad,
        Keyboard
	} state;

	
    touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> buttonProgTrackClickCallback;
    void buttonProgTrackClickHandler(const touchgfx::AbstractButton& src);
    touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> buttonScanTrackClickCallback;
    void buttonScanTrackClickHandler(const touchgfx::AbstractButton& src);
    touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> buttonReadAllCVsClickCallback;
    void buttonReadAllCVsClickHandler(const touchgfx::AbstractButton& src);
	void handleGestureEvent(const GestureEvent & evt);
    touchgfx::Callback<DCCConfigView, const Box&, const ClickEvent& > editTextClickHandlerCallback;
    void editTextClickHandler(const Box& b, const ClickEvent& evt);
    touchgfx::Callback<DCCConfigView, bool> closeKeypadWindowCallback;
    void closeKeypadWindowHandler(bool success);
    touchgfx::Callback<DCCConfigView, bool> closeKeyboardWindowCallback;
    void closeKeyboardWindowHandler(bool success);
};

#endif // DCCCONFIGVIEW_HPP
