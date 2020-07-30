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
#include <gui/containers/ComboBox.hpp>
#include "DecoderDefConfig.h"
#include "ToggleButtonIconAndText.h"
#include <gui/common/TouchEventHandler.h>
#include "SelectImageDialog.h"



static const int16_t x1 = 17;
static const int16_t x2 = 417;
static const int16_t LINE_SPACING = 45;
static const int16_t CV_LABEL_WIDTH = 360;
static const int16_t CV_FULL_LABEL_WIDTH = 800-2*x1;
static const int16_t CV_EDIT_WIDTH_MAX = 360;
static const int16_t CV_X2 = 417;
static const int16_t CV_SUBTITLE_OFFSET = 50;




class DCCConfigView : public TouchEventHandler<DCCConfigViewBase>
{
	using base_class = TouchEventHandler<DCCConfigViewBase>;
	
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
	
	touchgfx::Box boxConfig;
	touchgfx::ClickListener<touchgfx::TextWithFrame> textAddress;
	touchgfx::ClickListener<touchgfx::TextWithFrame> textName;
	touchgfx::ClickListener<touchgfx::TextWithFrame> textDescription;
	touchgfx::ClickListener<touchgfx::TextWithFrame> textDecoder;
	touchgfx::ClickListener<touchgfx::TextWithFrame> textSmallImage;
	touchgfx::ClickListener<touchgfx::TextWithFrame> textLargeImage;
	touchgfx::TextArea textAreaLabelAddress;
	touchgfx::TextArea textAreaLabelName;
	touchgfx::TextArea textAreaLabelDescription;
	touchgfx::TextArea textAreaLabelDecoder;
	touchgfx::TextArea textAreaLabelConfig;
	touchgfx::TextArea textAreaLabelSmallImage;
	touchgfx::TextArea textAreaLabelLargeImage;
	CheckBox chkMFDirection;
	CheckBox chkMFFLLocation;
	CheckBox chkMFPowerSourceConversion;
	CheckBox chkMFBiDirectionalComms;
	CheckBox chkMFSpeedTable;
	CheckBox chkMFTwoByteAddressing;
	CheckBox chkAccBiDirectionalComms;
	CheckBox chkAccType;
	CheckBox chkAccAddressMethod;
	ToggleButtonIconAndText btnCVs;
	ToggleButtonIconAndText btnGroup;
	ToggleButtonIconAndText btnRaw;
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
	
	touchgfx::TextArea textAreaLabelSpeedSteps;
    ComboBox cboSpeedSteps;
    touchgfx::Callback<DCCConfigView, ComboBoxBase&, int16_t, int16_t> cboSpeedStepsSelectionChangedCallback;
    void cboSpeedStepsSelectionChangedHandler(ComboBoxBase&, int16_t, int16_t);

	ModalBoxWindow selectWindow;
	touchgfx::TextWithFrame selectText;
	touchgfx::ButtonWithLabelAndEnable selectOKButton;
	touchgfx::ButtonWithLabelAndEnable selectCancelButton;
	ComboBox cboSelectList;
	
protected:
	int16_t decoderSpecificYStartPos;
	int16_t yCVBegin;

	touchgfx::ButtonWithLabelAndEnable buttonScanTrack;
	touchgfx::ButtonWithLabelAndEnable buttonReadAllCVs;
	touchgfx::ButtonWithLabelAndEnable buttonDelete;
	
	enum EField
	{
		Address,
		Name,
		Description,
		SmallImage,
		LargeImage
	} edittingField;
	enum EState
	{
		Editting,
		Scanning,
		ScanningAllCVs,
		Keypad,
		Keyboard,
		DeleteDecoder,
		SelectDecoderDef,
		SelectLargeIcon,
		SelectSmallIcon
	} state;

	std::u16string addressTextBuffer;	// max 4 digits plus terminator
	std::u16string decoderDefBuffer;
	std::u16string scanAllTextBuffer;
	std::u16string smallImageTextBuffer;
	std::u16string largeImageTextBuffer;
	std::vector<std::u16string> cvNumbers;
	std::vector<touchgfx::Drawable*> cvDrawables;
	
	virtual void handleGestureEvent(const GestureEvent & evt);
	virtual void handleDragEvent(const DragEvent& evt);
	virtual void SwipePreviousScreen();

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
	touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> selectListOKButtonClickCallback;
	void selectListOKButtonClickHandler(const touchgfx::AbstractButton& src);
	touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> selectListCancelButtonClickCallback;
	void selectListCancelButtonClickHandler(const touchgfx::AbstractButton& src);

    touchgfx::Callback<DCCConfigView, const touchgfx::AbstractButton&> buttonCVDisplayClickCallback;
    void buttonCVDisplayClickHandler(const touchgfx::AbstractButton& src);

    touchgfx::Callback<DCCConfigView, SelectImageDialog&, bool> selectImageDialogCloseCallback;
    void selectImageDialogCloseHandler(SelectImageDialog&, bool);
	std::shared_ptr<SelectImageDialog> selectFileDialog;
		
	bool isProgTrackEnabled() const 
	{
		return toggleProgTrack.getState();
	}
	
	void ShowWaitWindow(const char16_t *title, const char16_t *subtitle = nullptr, EButtons buttons = EButtons::None);
	void CloseWaitWindow(EErrorCode::EErrorCode result);
	void setConfig(uint8_t cv29);
	void showDecoderSpecificSettings(bool loco);
	void HideAllCustomConfigs();
	void SelectDecoderDefinition(const char16_t *title);
	void loadDecoderDef(const char *filename);
	void clearCVDisplay();
	void displayCVDisplay();
	void DisplayCV(EDecoderDataType::EDecoderDataType cvType, uint16_t cvNumber, const char16_t *name, CVDef &cv, uint16_t &yPos);
	uint16_t DrawCVTitle(const char16_t *name, const uint16_t yPos, const uint16_t XOffset = 0, const uint16_t titleWidth = CV_LABEL_WIDTH);
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
