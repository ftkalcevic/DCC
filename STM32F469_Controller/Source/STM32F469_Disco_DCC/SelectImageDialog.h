#pragma once

#include <gui/common/FrontendApplication.hpp>
#include <fonts/ApplicationFontProvider.hpp>
#include <touchgfx/Containers/ModalBoxWindow.hpp>
#include <touchgfx/widgets/TextWithFrame.hpp>
#include <gui/common/ButtonWithLabelAndEnable.hpp>
#include <gui/containers/CheckBox.hpp>
#include <gui/containers/ComboBox.hpp>
#include <touchgfx/widgets/Image.hpp>
#include "ScalableAspectImage.h"
#include <touchgfx/mixins/ClickListener.hpp>
#include <gui/containers/FileItem.hpp>
#include "Decoders.h"



class SelectImageDialog: public ModalBoxWindow
{
public:
	SelectImageDialog(BitmapId backgroundBitmapId, const char16_t *title, EDecoderType::EDecoderType decoderType, EUserImage::EUserImage imageType, const char *imageFile );
	~SelectImageDialog();
	
	void setButtonPressedCallback( GenericCallback<SelectImageDialog&, bool> *callback ) { buttonCallback = callback; }
	EUserImage::EUserImage getUserImage() const;
	const char *getUserFile() const;
	
protected:
	touchgfx::TextWithFrame txtTitle;
	touchgfx::ButtonWithLabelAndEnable btnOK;
	touchgfx::ButtonWithLabelAndEnable btnCancel;
	CheckBox chkFlipHoriz;
	CheckBox chkFlipVert;
	ComboBox cboImageList;
	touchgfx::Image imgImageBackground;
	touchgfx::ScalableAspectImage imgImage;

    touchgfx::ClickListener< touchgfx::ScrollWheel > swFiles;
    touchgfx::DrawableListItems<FileItem, 5> swFilesItems;
    touchgfx::TiledImage tiledImageBottomMask;
    touchgfx::TiledImage tiledImageTopMask;	
	
	BitmapId bmpIdCurrentImage;
private:
	void ShowBitmap(BitmapId id);
	void notify(bool ok);
	void UpdateScrollWheelFiles(EUserImage::EUserImage id);
		
    touchgfx::Callback<SelectImageDialog, touchgfx::DrawableListItemsInterface*, int16_t, int16_t> swFiles_updateItemCallback;
    void updateItemCallbackHandler(touchgfx::DrawableListItemsInterface* items, int16_t containerIndex, int16_t itemIndex);
	
    Callback<SelectImageDialog, int16_t> swFilesAnimateToPositionCallback;
    void swFilesAnimateToPositionHandler(int16_t index);
	
    touchgfx::Callback<SelectImageDialog, ComboBoxBase&, int16_t, int16_t> cboImageListSelectionChangedCallback;
    void cboImageListSelectionChangedHandler(ComboBoxBase&, int16_t, int16_t);
	
	touchgfx::Callback<SelectImageDialog, const touchgfx::AbstractButton&> okButtonClickCallback;
	void oKButtonClickHandler(const touchgfx::AbstractButton& src);

	touchgfx::Callback<SelectImageDialog, const touchgfx::AbstractButton&> cancelButtonClickCallback;
	void cancelButtonClickHandler(const touchgfx::AbstractButton& src);
	
    GenericCallback<SelectImageDialog&, bool> * buttonCallback;
};




