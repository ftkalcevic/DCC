#include <gui/startupscreen_screen/StartupScreenView.hpp>
#include <touchgfx/Color.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <gui/common/BinFileLoader.h>

StartupScreenView::StartupScreenView()
{
	bmpId = BITMAP_INVALID;

	if (LoadSplashImage())
	{
		
	}
	else
	{
        box1.setPosition(0, 0, 800, 480);
        box1.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));

        textArea1.setXY(130, 194);
        textArea1.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
        textArea1.setLinespacing(0);
        textArea1.setTypedText(touchgfx::TypedText(T_RESOURCEID1));

        add(box1);
        add(textArea1);
	}
}

void StartupScreenView::setupScreen()
{
    StartupScreenViewBase::setupScreen();
}

void StartupScreenView::tearDownScreen()
{
	if (bmpId != BITMAP_INVALID)
	{
		Bitmap::dynamicBitmapDelete(bmpId);
	}
    StartupScreenViewBase::tearDownScreen();
}

bool StartupScreenView::LoadSplashImage()
{
#ifndef SIMULATOR
	bmpId = BinFileLoader::makeBitmap("/Images/Splash"); 
	if (bmpId != BITMAP_INVALID)
	{
		image1.setBitmap(Bitmap(bmpId));
		image1.setXY(0, 0);
		add(image1);		
		return true;
	}
#endif
	return false;

#ifdef PNG
	PNGFileLoader png;
	bmpId = png.makeBitmap("/Images/backgnd.png");
	if (bmpId != BITMAP_INVALID)
	{
		image1.setBitmap(Bitmap(bmpId));

		//Position image and add to View
		//image1.setPosition(0, 0, 800, 480);
		image1.setXY(0, 0);
		add(image1);		
		return true;
	}
	return false;
#endif
	
#if 0	
	bool ret = false;
	FIL file;
	if (f_open(&file, "/Images/Splash.tga", FA_READ) == FR_OK)
	{
        uint16_t width, height, depth;
        TGAFileLoader::getBMP24Dimensions(&file, width, height, depth);
    
        //Create (24bit) dynamic bitmap of same dimension
        bmpId = Bitmap::dynamicBitmapCreate(width, height, Bitmap::RGB888);
		if (bmpId == BITMAP_INVALID)
		{
			ret = false;
		}
		else
		{
            //Load pixels from BMP file to dynamic bitmap
            TGAFileLoader::readBMP24File(Bitmap(bmpId), &file);
			
            //Make Image show the loaded bitmap
            image1.setBitmap(Bitmap(bmpId));

			//Position image and add to View
			//image1.setPosition(0, 0, width, height);
			image1.setXY(0, 0);
            add(image1);		
			ret = true;
		}

		f_close(&file);
	}
	
	return ret;
#endif
}