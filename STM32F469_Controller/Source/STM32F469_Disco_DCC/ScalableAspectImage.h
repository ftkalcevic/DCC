#pragma once


#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/ScalableImage.hpp>

namespace touchgfx
{
	// Add fixed aspect ratio to ScalableImage
	class ScalableAspectImage : public Image
	{
	public:
//		virtual void setPosition(int16_t x, int16_t y, int16_t width, int16_t height)
//		{
//			int displayWidth = width;
//			int displayHeight = height;
//			
//			int bmpWidth = bitmap.getWidth();
//			int bmpHeight = bitmap.getHeight();
//			
//			float displayRatio = (float)displayWidth / (float)displayHeight;
//			float bmpRatio = (float)bmpWidth / (float)bmpHeight;
//			
//			if (displayRatio > bmpRatio)
//			{
//				// size to display Height
//				int newWidth = displayRatio * height;
//				x += (width - newWidth) / 2;
//				width = newWidth;				
//			}
//			else
//			{
//				// size to display width
//				int newHeight = width / displayRatio;
//				y += (height - newHeight) / 2;
//				height = newHeight;				
//			}
//			Image::setPosition(x, y, width, height);
//		}
		
		// Just center
		virtual void setPosition(int16_t x, int16_t y, int16_t width, int16_t height)
		{
			int displayWidth = width;
			int displayHeight = height;
			
			int bmpWidth = bitmap.getWidth();
			int bmpHeight = bitmap.getHeight();
			
			if (displayWidth > bmpWidth)
			{
				x += (displayWidth - bmpWidth) / 2;
			}
			if ( displayHeight > bmpHeight )
			{
				// size to display width
				y += (displayHeight - bmpHeight) / 2;
			}
			Image::setPosition(x, y, width, height);
		}		
	};
}