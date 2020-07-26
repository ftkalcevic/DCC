/**
  ******************************************************************************
  * This file is part of the TouchGFX 4.13.0 distribution.
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/hal/HAL.hpp>


namespace touchgfx
{
BitmapId  Image::bitmapRGB888Id = BITMAP_INVALID;
BitmapId  Image::bitmapARGB8888Id = BITMAP_INVALID;

void Image::setBitmap(const Bitmap& bmp)
{
    bitmap = bmp;
    // When setting bitmap, adjust size of this widget to match.
    setWidth(bitmap.getWidth());
    setHeight(bitmap.getHeight());

    // This bool is no longer used, but maintained for backwards compat.
    hasTransparentPixels = bitmap.hasTransparentPixels();
}

BitmapId Image::GetLineBitmap(Bitmap::BitmapFormat fmt) 
{
	switch (fmt)
	{
		case Bitmap::RGB888:
			if (bitmapRGB888Id == BITMAP_INVALID)
				bitmapRGB888Id = Bitmap::dynamicBitmapCreate(HAL::DISPLAY_WIDTH, 1, Bitmap::RGB888);
			return bitmapRGB888Id;

		case Bitmap::ARGB8888:
			if (bitmapARGB8888Id == BITMAP_INVALID)
				bitmapARGB8888Id = Bitmap::dynamicBitmapCreate(HAL::DISPLAY_WIDTH, 1, Bitmap::ARGB8888);
			return bitmapARGB8888Id;
			
		default:
			return BITMAP_INVALID;
	}
}

// bitmap - the bitmap
// widgetRect - bounding rectangle of the bitmap (WxH)
// xpos, ypos - absolute position
// dirtyBitmapArea - dirty part of widgetRect
// flipHorizontal, flipVertical - flip
// alpha
void Image::DrawBitmap(const Bitmap &bitmap, Rect widgetRect, int16_t xPos, int16_t yPos, Rect dirtyBitmapArea, bool flipHorizontal, bool flipVertical, uint8_t alpha)
{
    if (!dirtyBitmapArea.isEmpty())
    {
	    if (flipHorizontal || flipVertical)
	    {
			int bmpWidth = bitmap.getWidth();
			int bmpHeight = bitmap.getHeight();
			int width = dirtyBitmapArea.width;
			if (width > widgetRect.width)
				width = widgetRect.width;
			int height = dirtyBitmapArea.height;
			if (height > widgetRect.height)
				height = widgetRect.height;
		    
		    uint16_t bytesPerPixel;
		    uint16_t yOutStart, yOutEnd, xOutStart, xOutEnd;
		    int16_t yOutDirection, xOutDirection;
		    if (flipVertical)
		    {
			    yOutStart = dirtyBitmapArea.bottom()-1;
			    yOutEnd = dirtyBitmapArea.y;
			    yOutDirection = -1;
		    }
		    else
		    {
			    yOutStart = dirtyBitmapArea.y;
			    yOutEnd = dirtyBitmapArea.bottom()-1;
			    yOutDirection = 1;
		    }
		    if (flipHorizontal)
		    {
			    xOutStart = bmpWidth - dirtyBitmapArea.right();
			    xOutEnd = bmpWidth - dirtyBitmapArea.x;
			    xOutDirection = -1;
		    }
		    else
		    {
			    xOutStart = dirtyBitmapArea.x;
			    xOutEnd = dirtyBitmapArea.right()-1;
			    xOutDirection = 1;
		    }
		    
			// build up 1 row at a time and paint it.
			    
			if (!flipHorizontal)	// No horizontal (x) flip, we can just use the data straight from the bitmap
			{
				int y = yOutStart;
				for(;;)
				{
					Rect srcRect(xOutStart, bmpHeight - y - 1, width, 1);
					HAL::lcd().drawPartialBitmap(bitmap, xPos, yPos - (bmpHeight-1-2*y), srcRect);
					if (y == yOutEnd)
						break;
			    
					y += yOutDirection;
				}
			}
		    else	// Horizontal (x) flip, we need to move pixels (x) -> (width-x-1)
			{
				BitmapId bmpLineId;
				if (bitmap.getFormat() == Bitmap::RGB888)
				{
					bytesPerPixel = 3;
					bmpLineId = Image::GetLineBitmap(Bitmap::RGB888);
				}
				else if (bitmap.getFormat() == Bitmap::ARGB8888)
				{
					bytesPerPixel = 4;
					bmpLineId = Image::GetLineBitmap(Bitmap::ARGB8888);
				}
				else
					{ assert(false); }
				
				Bitmap bmpLine(bmpLineId);
				Rect rSolid = bmpLine.getSolidRect();
				Rect rBmp = bmpLine.getRect();
				uint16_t lineWidth = bmpLine.getWidth();
				uint8_t *destBmp = bmpLine.dynamicBitmapGetAddress(bmpLineId);
				const uint8_t *srcBmp = bitmap.getData();
			    
				Rect srcRect(0, 0, width, 1);
				int y = yOutStart;
				for(;;)
				{
					uint8_t *dest = destBmp;
					const uint8_t *src = srcBmp + (xOutEnd + y*bmpWidth)*bytesPerPixel;
					int x = xOutEnd;
					if (bytesPerPixel == 4)
					{
						// optimisation for 4 bytes per pixel
						for(;;)
						{
//							if ( y <= MIN(yOutStart,yOutEnd) + (yOutStart+yOutEnd)/10 )
//								*((uint32_t *)dest) = 0xFF0000FF;
//							else
								*((uint32_t *)dest) = *((const uint32_t *)src);
							if (x == xOutStart)
								break;
							x += xOutDirection;
							dest += 4;
							src -= 4;
						}
					}
					else
					{
						for(;;)
						{
							for (int i = 0; i < bytesPerPixel; i++)
								*dest++ = *src++;
					    
							if (x == xOutStart)
								break;
							x += xOutDirection;
							src -= 2*bytesPerPixel;
						}
					}
				    
					if (flipVertical)
					{
						HAL::lcd().drawPartialBitmap(bmpLine, xPos + dirtyBitmapArea.x, yPos + (bmpHeight-y-1), srcRect, alpha);
					}
					else
					{
						HAL::lcd().drawPartialBitmap(bmpLine,xPos + dirtyBitmapArea.x, yPos+y, srcRect, alpha);
					}
			    
					if (y == yOutEnd)
						break;
			    
					y += yOutDirection;
				}
		    }
	    }
	    else
	    {
            HAL::lcd().drawPartialBitmap(bitmap, xPos, yPos, dirtyBitmapArea, alpha);
	    }
    }
}
	
void Image::draw(const Rect& invalidatedArea) const
{
    Rect meAbs;
    translateRectToAbsolute(meAbs); //To find our x and y coords in absolute.

    Rect dirtyBitmapArea = bitmap.getRect() & invalidatedArea;

	if (!dirtyBitmapArea.isEmpty())
		DrawBitmap(bitmap, getRect(), meAbs.x, meAbs.y, dirtyBitmapArea, flipHorizontal, flipVertical, alpha);
	
//    Rect meAbs;
//    translateRectToAbsolute(meAbs); //To find our x and y coords in absolute.
//
//    // Calculate intersection between image, bitmap rect and invalidated area.
//    Rect rt1 = getRect();
//	Rect rt2 = bitmap.getRect();
//	Rect rt3 = invalidatedArea;
//    Rect dirtyBitmapArea = bitmap.getRect() & invalidatedArea;
//
//    if (!dirtyBitmapArea.isEmpty())
//    {
//	    if (flipHorizontal || flipVertical)
//	    {
//			int bmpWidth = bitmap.getWidth();
//			int bmpHeight = bitmap.getHeight();
//			int width = dirtyBitmapArea.width;
//			if (width > getWidth())
//				width = getWidth();
//			int height = dirtyBitmapArea.height;
//			if (height > getHeight())
//				height = getHeight();
//		    
//		    uint16_t bytesPerPixel;
//		    uint16_t yOutStart, yOutEnd, xOutStart, xOutEnd;
//		    int16_t yOutDirection, xOutDirection;
//		    if (flipVertical)
//		    {
//			    yOutStart = dirtyBitmapArea.bottom()-1;
//			    yOutEnd = dirtyBitmapArea.y;
//			    yOutDirection = -1;
//		    }
//		    else
//		    {
//			    yOutStart = dirtyBitmapArea.y;
//			    yOutEnd = dirtyBitmapArea.bottom()-1;
//			    yOutDirection = 1;
//		    }
//		    if (flipHorizontal)
//		    {
//			    xOutStart = bmpWidth - dirtyBitmapArea.right();
//			    xOutEnd = bmpWidth - dirtyBitmapArea.x;
//			    xOutDirection = -1;
//		    }
//		    else
//		    {
//			    xOutStart = dirtyBitmapArea.x;
//			    xOutEnd = dirtyBitmapArea.right()-1;
//			    xOutDirection = 1;
//		    }
//		    
//			// build up 1 row at a time and paint it.
//			    
//			if (!flipHorizontal)	// No horizontal (x) flip, we can just use the data straight from the bitmap
//			{
//				int y = yOutStart;
//				for(;;)
//				{
//					Rect srcRect(xOutStart, bmpHeight - y - 1, width, 1);
//					HAL::lcd().drawPartialBitmap(bitmap,meAbs.x, meAbs.y - (bmpHeight-1-2*y), srcRect);
//					if (y == yOutEnd)
//						break;
//			    
//					y += yOutDirection;
//				}
//			}
//		    else	// Horizontal (x) flip, we need to move pixels (x) -> (width-x-1)
//			{
//				BitmapId bmpLineId;
//				if (bitmap.getFormat() == Bitmap::RGB888)
//				{
//					bytesPerPixel = 3;
//					bmpLineId = GetLineBitmap(Bitmap::RGB888);
//				}
//				else if (bitmap.getFormat() == Bitmap::ARGB8888)
//				{
//					bytesPerPixel = 4;
//					bmpLineId = GetLineBitmap(Bitmap::ARGB8888);
//				}
//				else
//					{ assert(false); }
//				
//				Bitmap bmpLine(bmpLineId);
//				Rect rSolid = bmpLine.getSolidRect();
//				Rect rBmp = bmpLine.getRect();
//				uint16_t lineWidth = bmpLine.getWidth();
//				uint8_t *destBmp = bmpLine.dynamicBitmapGetAddress(bmpLineId);
//				const uint8_t *srcBmp = bitmap.getData();
//			    
//				Rect srcRect(0, 0, width, 1);
//				int y = yOutStart;
//				for(;;)
//				{
//					uint8_t *dest = destBmp;
//					const uint8_t *src = srcBmp + (xOutEnd + y*bmpWidth)*bytesPerPixel;
//					int x = xOutEnd;
//					if (bytesPerPixel == 4)
//					{
//						// optimisation for 4 bytes per pixel
//						for(;;)
//						{
////							if ( y <= (yOutStart+yOutEnd)/2 )
////								*((uint32_t *)dest) = 0xFF0000FF;
////							else
//								*((uint32_t *)dest) = *((const uint32_t *)src);
//							if (x == xOutStart)
//								break;
//							x += xOutDirection;
//							dest += 4;
//							src -= 4;
//						}
//					}
//					else
//					{
//						for(;;)
//						{
//							for (int i = 0; i < bytesPerPixel; i++)
//								*dest++ = *src++;
//					    
//							if (x == xOutStart)
//								break;
//							x += xOutDirection;
//							src -= 2*bytesPerPixel;
//						}
//					}
//				    
//					if (flipVertical)
//					{
//						HAL::lcd().drawPartialBitmap(bmpLine, meAbs.x, meAbs.y + (bmpHeight-y-1), srcRect);
//					}
//					else
//					{
//						HAL::lcd().drawPartialBitmap(bmpLine,meAbs.x+dirtyBitmapArea.x, meAbs.y+y, srcRect);
//					}
//			    
//					if (y == yOutEnd)
//						break;
//			    
//					y += yOutDirection;
//				}
//		    }
//	    }
//	    else
//	    {
//            HAL::lcd().drawPartialBitmap(bitmap, meAbs.x, meAbs.y, dirtyBitmapArea, alpha);
//	    }
//    }
}

Rect Image::getSolidRect() const
{
    // If alpha is less than solid, we have an empty solid rect.
    if (alpha < 255)
    {
        return Rect(0, 0, 0, 0);
    }
    // Return solid rect from bitmap (precalculated).
    return bitmap.getSolidRect();
}
} // namespace touchgfx
