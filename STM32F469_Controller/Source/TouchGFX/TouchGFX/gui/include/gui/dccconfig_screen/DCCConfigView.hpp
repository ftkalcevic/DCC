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
#include <gui/containers/CheckBox.hpp>
#include <touchgfx/widgets/TextWithFrame.hpp>
#include <touchgfx/Containers/ModalBoxWindow.hpp>

class DCCConfigView : public DCCConfigViewBase
{
public:
	enum EButtons
	{
		None   = 0x00,
		OK	   = 0x01,
		Cancel = 0x02
	};

	
	DCCConfigView();
	virtual ~DCCConfigView() {}
	virtual void setupScreen();
	virtual void tearDownScreen();
	
	void ScanTrackReply(EErrorCode::EErrorCode result, int16_t address, int16_t config, int16_t extendedAddress, int16_t manufacturer, int16_t version);
	void ScanAllCVsReply(EErrorCode::EErrorCode result, uint16_t cv, uint8_t value);
	void WriteReply(EErrorCode::EErrorCode result);
	
	touchgfx::ClickListener<touchgfx::Box> boxDecoder;
	touchgfx::Box boxConfig;
	touchgfx::ClickListener<touchgfx::TextWithFrame> textAddress;
	touchgfx::ClickListener<touchgfx::TextWithFrame> textName;
	touchgfx::ClickListener<touchgfx::TextWithFrame> textDescription;
	touchgfx::TextArea textAreaLabelAddress;
	touchgfx::TextArea textAreaLabelName;
	touchgfx::TextArea textAreaLabelDescription;
	touchgfx::TextArea textAreaLabelDecoder;
	touchgfx::TextArea textAreaLabelConfig;
	CheckBox chkMFDirection;
	CheckBox chkMFFLLocation;
	CheckBox chkMFPowerSourceConversion;
	CheckBox chkMFBiDirectionalComms;
	CheckBox chkMFSpeedTable;
	CheckBox chkMFTwoByteAddressing;
	CheckBox chkAccBiDirectionalComms;
	CheckBox chkAccType;
	CheckBox chkAccAddressMethod;
	touchgfx::TextArea textAreaLabelAllCVs;
	NumericKeypad numericKeypad;
	FullKeyboard keyboard;
	int16_t selectStartX;
	int16_t selectStartY;
	int selectedDecoderItem;
	TickType_t selectStartTime;
	ModalBoxWindow waitWindow;
	touchgfx::TextWithFrame waitText;
	touchgfx::TextWithFrame waitSubText;
	touchgfx::ButtonWithLabelAndEnable waitOKButton;
	touchgfx::ButtonWithLabelAndEnable waitCancelButton;
	
protected:
	touchgfx::ButtonWithLabelAndEnable buttonScanTrack;
	touchgfx::ButtonWithLabelAndEnable buttonReadAllCVs;
	touchgfx::ButtonWithLabelAndEnable buttonDelete;
	
	enum EField
	{
		Address,
		Name,
		Description
	} edittingField;
	enum EState
	{
		Editting,
		Scanning,
		ScanningAllCVs,
		Keypad,
		Keyboard,
		DeleteDecoder
	} state;

	Unicode::UnicodeChar addressTextBuffer[5];	// max 4 digits plus terminator
	Unicode::UnicodeChar scanAllTextBuffer[50];

	void EditNumeric(EField field, const char16_t *title, uint16_t value, int min, int max);
	void EditText(EField field, const char16_t *title, const char16_t *text, uint16_t maxLen, FontId font, Alignment align);
	virtual void scrollWheelDecodersUpdateItem(ListItemDecoder& item, int16_t itemIndex);
	void displayDecoder();
		
	touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> buttonProgTrackClickCallback;
	void buttonProgTrackClickHandler(const touchgfx::AbstractButton& src);
	touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> buttonScanTrackClickCallback;
	void buttonScanTrackClickHandler(const touchgfx::AbstractButton& src);
	touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> buttonReadAllCVsClickCallback;
	void buttonReadAllCVsClickHandler(const touchgfx::AbstractButton& src);
	touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> buttonDeleteClickCallback;
	void buttonDeleteClickHandler(const touchgfx::AbstractButton& src);
	virtual void handleGestureEvent(const GestureEvent & evt);
	virtual void handleDragEvent(const DragEvent& evt);
	touchgfx::Callback<DCCConfigView, const TextWithFrame&, const ClickEvent& > editTextClickHandlerCallback;
	void editTextClickHandler(const TextWithFrame& b, const ClickEvent& evt);
	touchgfx::Callback<DCCConfigView, bool> closeKeypadWindowCallback;
	void closeKeypadWindowHandler(bool success);
	touchgfx::Callback<DCCConfigView, bool> closeKeyboardWindowCallback;
	void closeKeyboardWindowHandler(bool success);
	
	touchgfx::Callback<DCCConfigView, const ScrollWheel&, const ClickEvent& > scrollWheelDecodersClickCallback;
	void scrollWheelDecodersClickHandler(const ScrollWheel& b, const ClickEvent& evt);
	touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> waitOKButtonClickCallback;
	void waitOKButtonClickHandler(const touchgfx::AbstractButton& src);
	touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> waitCancelButtonClickCallback;
	void waitCancelButtonClickHandler(const touchgfx::AbstractButton& src);

	bool isProgTrackEnabled() const 
	{
		return toggleProgTrack.getState();
	}
	
	void ShowWaitWindow(const char16_t *title, const char16_t *subtitle = nullptr, EButtons buttons = EButtons::None);
	void CloseWaitWindow(EErrorCode::EErrorCode result);
	void setConfig(uint8_t cv29);
};

inline DCCConfigView::EButtons operator | (DCCConfigView::EButtons a, DCCConfigView::EButtons b)
{
	return static_cast<DCCConfigView::EButtons>(static_cast<int>(a) | static_cast<int>(b));
}
	
inline DCCConfigView::EButtons operator & (DCCConfigView::EButtons a, DCCConfigView::EButtons b)
{
	return static_cast<DCCConfigView::EButtons>(static_cast<int>(a) & static_cast<int>(b));
}
	



#endif // DCCCONFIGVIEW_HPP
