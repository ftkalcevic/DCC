#include "SelectImageDialog.h"
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include "ImageFile.h"
#include <gui/common/BinFileLoader.h>
#include "Decoders.h"

const int windowWidth = 800;
const int windowHeight = 480;
SelectImageDialog::SelectImageDialog(BitmapId backgroundBitmapId, const char16_t *title, EDecoderType::EDecoderType decoderType, EUserImage::EUserImage imageType, const char *imageFile, EImageAttributes::EImageAttributes attr ) :
	bmpIdCurrentImage(BITMAP_INVALID),
    swFiles_updateItemCallback(this, &SelectImageDialog::updateItemCallbackHandler),
    swFilesAnimateToPositionCallback(this, &SelectImageDialog::swFilesAnimateToPositionHandler),
	cboImageListSelectionChangedCallback(this, &SelectImageDialog::cboImageListSelectionChangedHandler),
	okButtonClickCallback(this, &SelectImageDialog::oKButtonClickHandler),
	cancelButtonClickCallback(this, &SelectImageDialog::cancelButtonClickHandler),
	flipFlagChangedCallback(this, &SelectImageDialog::flipFlagChangedHandler )
{
	touchgfx::colortype backgroundColor = Color::getColorFrom24BitRGB(174, 174, 174);
	
	setWidth(windowWidth);
	setHeight(windowHeight);
	setWindowSize(windowWidth, windowHeight);
	setWindowColor(backgroundColor);
	setWindowBorderColor(Color::getColorFrom24BitRGB(3, 129, 174));
	
	const int textwidth = windowWidth - 50;
	txtTitle.setFontId(Typography::SANSSERIF40PX);
	txtTitle.setAlignment(CENTER);
	txtTitle.setBoxColor(backgroundColor);
	txtTitle.setPosition((windowWidth - textwidth)/2, 10, textwidth, 50);
	txtTitle.setText(title);
	add(txtTitle);

	Bitmap bmp(backgroundBitmapId);
    imgImageBackground.setXY(400 + (384-bmp.getWidth())/2, 85 + (214-bmp.getHeight())/2);
	imgImageBackground.setBitmap(touchgfx::Bitmap(backgroundBitmapId));
    add(imgImageBackground);
	
	imgImage.setPosition(imgImageBackground.getX() + 2, imgImageBackground.getY() + 2, 0, 0);
    add(imgImage);
	
	btnOK.setXY(595, 395);
    btnOK.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID));
    btnOK.setLabelText(touchgfx::TypedText(T_WAITBUTTONOKID));
    btnOK.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    btnOK.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	btnOK.setAction(okButtonClickCallback);
	add(btnOK);
		
	btnCancel.setXY(40, 395);
    btnCancel.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID));
    btnCancel.setLabelText(touchgfx::TypedText(T_WAITBUTTONCANCELID));
    btnCancel.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    btnCancel.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
	btnCancel.setAction(cancelButtonClickCallback);
	add(btnCancel);

	chkFlipHoriz.setPosition(400, 314, 180, 65);
	chkFlipHoriz.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT40PXID));
	chkFlipHoriz.setText(u"Horiz");
	chkFlipHoriz.setVisible(true);
	chkFlipHoriz.setAction(flipFlagChangedCallback);
	chkFlipHoriz.setSelected((attr & EImageAttributes::FlipHorizontal) != 0);
	add(chkFlipHoriz);

	chkFlipVert.setPosition(600, 314, 180, 65);
	chkFlipVert.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT40PXID));
	chkFlipVert.setText(u"Vert");
	chkFlipVert.setVisible(true);
	chkFlipVert.setAction(flipFlagChangedCallback);
	chkFlipVert.setSelected((attr & EImageAttributes::FlipVertical) != 0);
	add(chkFlipVert);

	cboImageList.setPosition(20, 84, 360, 50);
	cboImageList.setSelectionChangedCallback(&cboImageListSelectionChangedCallback);
	cboImageList.setNumberOfItems(0);
	uint16_t nItems = 0;
	for (int i = 0; i < EUserImageClass::Count(); i++)
	{
		const EUserImageClass::SUserImageInfo &img = EUserImageClass::UserImage(i);
		if ( img.usage & decoderType )
		{
			cboImageList.addComboItem(img.name, img.id);
			nItems++;
		}
	}
	cboImageList.setSelectedId(imageType);
	cboImageList.setVisible(true);
	add(cboImageList);

    swFiles.setPosition(20, 158, 360, 200);
    swFiles.setHorizontal(false);
    swFiles.setCircular(false);
    swFiles.setEasingEquation(touchgfx::EasingEquations::backEaseOut);
    swFiles.setSwipeAcceleration(10);
    swFiles.setDragAcceleration(10);
    swFiles.setNumberOfItems(imageFiles.imageFiles.size());
    swFiles.setSelectedItemOffset(65);
    swFiles.setDrawableSize(50, 10);
    swFiles.setDrawables(swFilesItems, swFiles_updateItemCallback);
    swFiles.initialize();
	swFiles.setAnimateToCallback(swFilesAnimateToPositionCallback);
    for (int i = 0; i < swFilesItems.getNumberOfDrawables(); i++)
    {
        swFilesItems[i].initialize();
    }
	if (imageFile != nullptr && *imageFile != 0)
	{
		// Find image
		for(int i = 0 ; i < imageFiles.imageFiles.size() ; i++)
			if ( strcasecmp( imageFile, imageFiles.imageFiles[i]->filename.c_str() ) == 0 )
			{
				swFiles.animateToItem(i, 0);	
				break;
			}
	}
	add(swFiles);
		
    tiledImageBottomMask.setBitmap(touchgfx::Bitmap(BITMAP_DECODERSWHEELMASKBOTTOM_ID));
    tiledImageBottomMask.setPosition(20, 285, 360, 73);
    tiledImageBottomMask.setOffset(0, 0);
    add(tiledImageBottomMask);

    tiledImageTopMask.setBitmap(touchgfx::Bitmap(BITMAP_DECODERSWHEELMASKTOP_ID));
    tiledImageTopMask.setPosition(20, 158, 360, 73);
    tiledImageTopMask.setOffset(0, 0);
    add(tiledImageTopMask);
	
	UpdateScrollWheelFiles(imageType);
}


SelectImageDialog::~SelectImageDialog()
{
	if (bmpIdCurrentImage)
	{
		Bitmap::dynamicBitmapDelete(bmpIdCurrentImage);
		bmpIdCurrentImage = BITMAP_INVALID;
	}
}


void SelectImageDialog::updateItemCallbackHandler(touchgfx::DrawableListItemsInterface* items, int16_t containerIndex, int16_t itemIndex)
{
    if (items == &swFilesItems)
    {
        touchgfx::Drawable* d = items->getDrawable(containerIndex);
        FileItem* cc = (FileItem*)d;
	    cc->setText(imageFiles.imageFiles[itemIndex]->displayFilename.c_str());
        //scrollWheel1UpdateItem(*cc, itemIndex);
    }
}


void SelectImageDialog::swFilesAnimateToPositionHandler(int16_t index)
{
	std::string path = "/Images/"; 
	path += imageFiles.imageFiles[index]->filename.c_str();
	if (bmpIdCurrentImage)
	{
		Bitmap::dynamicBitmapDelete(bmpIdCurrentImage);
		bmpIdCurrentImage = BITMAP_INVALID;
	}
		
	bmpIdCurrentImage = BinFileLoader::makeBitmap(path.c_str() );
	if (bmpIdCurrentImage != BITMAP_INVALID)
	{
		ShowBitmap(bmpIdCurrentImage);
	}
}

void SelectImageDialog::UpdateScrollWheelFiles(EUserImage::EUserImage id)
{
	if (id == EUserImage::UserFile)
	{
		swFiles.setVisible(true);
		swFiles.invalidate();
	}
	else
	{
		swFiles.setVisible(false);
		ShowBitmap(EUserImageClass::UserImage(id).bmpId);
		swFiles.invalidate();
	}
}

void SelectImageDialog::cboImageListSelectionChangedHandler(ComboBoxBase& cbo, int16_t itemIndex, int16_t _id)
{
	EUserImage::EUserImage id = (EUserImage::EUserImage)_id;
	UpdateScrollWheelFiles(id);
}

void SelectImageDialog::ShowBitmap(BitmapId id)
{
	Bitmap bmp(id);
	imgImage.setBitmap(bmp);
	if (imgImage.getWidth() < imgImageBackground.getWidth() - 4)
	{
		imgImage.setX(imgImageBackground.getX() +  2 + (imgImageBackground.getWidth() - imgImage.getWidth()) / 2);
	}
	else
	{
		imgImage.setX( imgImageBackground.getX() +  2 );
		imgImage.setWidth(imgImageBackground.getWidth() - 4);
	}
	if (imgImage.getHeight() < imgImageBackground.getHeight() - 4)
	{
		imgImage.setY(imgImageBackground.getY() + 2 + (imgImageBackground.getHeight() - imgImage.getHeight()) / 2);
	}
	else
	{
		imgImage.setY( imgImageBackground.getY() + 2 );
		imgImage.setHeight(imgImageBackground.getHeight() - 4);
	}
	imgImageBackground.invalidate();
}

void SelectImageDialog::notify(bool ok)
{
    if (buttonCallback && buttonCallback->isValid())
    {
        buttonCallback->execute(*this, ok);
    }
}

void SelectImageDialog::oKButtonClickHandler(const touchgfx::AbstractButton& src)
{
	notify(true);
}


void SelectImageDialog::cancelButtonClickHandler(const touchgfx::AbstractButton& src)
{
	//notify(false);
	Rect r(imgImage.getWidth()/2, imgImage.getHeight()/2, imgImage.getWidth()/2, imgImage.getHeight()/2);
	imgImage.invalidateRect(r);
}


EUserImage::EUserImage SelectImageDialog::getUserImage() const
{
	return (EUserImage::EUserImage)cboImageList.getSelectedId();
}


const char *SelectImageDialog::getUserFile() const
{
	int index = swFiles.getSelectedItem();
	if (index >= 0 && index < imageFiles.imageFiles.size())
		return imageFiles.imageFiles[index]->filename.c_str();
	return "";
}


void SelectImageDialog::flipFlagChangedHandler(const CheckBox& src)
{
	imgImage.setFlipHorizontal(chkFlipHoriz.getSelected());
	imgImage.setFlipVertical(chkFlipVert.getSelected());
	//imgImageBackground.invalidate();
	imgImage.invalidate();
}

EImageAttributes::EImageAttributes SelectImageDialog::getImageAttributes() const
{
	return (EImageAttributes::EImageAttributes)((chkFlipVert.getSelected() ? EImageAttributes::FlipVertical : 0) | (chkFlipHoriz.getSelected() ? EImageAttributes::FlipHorizontal : 0));
}
