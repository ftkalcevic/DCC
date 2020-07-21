#include <gui/common/BinFileLoader.h>
#include <touchgfx/Color.hpp>
#include <stdio.h>
#include <string.h>
#include "FileSystem.h"
#include "../STM32F469_Disco_DCC/upng.h"


BitmapId BinFileLoader::readBinFile(FIL *f)
{
    // Bin file format - uint16_t width, uint16_t height, uint8_t format=RGB888, data... (BGR BGR from top left).
    f_lseek(f, 0);
	
	const int HEADER_BYTES = 5;
    uint8_t header[HEADER_BYTES];
	
	UINT bytesRead=0;
    f_read(f, header, HEADER_BYTES, &bytesRead);
	if (bytesRead != HEADER_BYTES)
		return BITMAP_INVALID;

    const uint32_t width = header[0] | (header[1] << 8);
    const uint32_t height = header[2] | (header[3] << 8);
    const uint32_t format = header[4];

	if (format != Bitmap::RGB888 && format != Bitmap::ARGB8888)
		return BITMAP_INVALID;
	
	BitmapId bmpId;
	uint16_t bpp;
	if (format == Bitmap::RGB888)
	{
		bmpId = Bitmap::dynamicBitmapCreate(width, height, Bitmap::RGB888);
		bpp = 3;
	}
	else
	{
	    bmpId = Bitmap::dynamicBitmapCreate(width, height, Bitmap::ARGB8888);
		bpp = 4;
	}
    uint8_t* buffer = Bitmap::dynamicBitmapGetAddress(bmpId);
	
	// everything aligns.  Just copy the data into the buffer... 
    UINT bytesToRead = width * height * bpp;
    f_read(f, buffer, bytesToRead, &bytesRead);
	if (bytesRead != bytesToRead)
	{
		Bitmap::dynamicBitmapDelete(bmpId);
		return BITMAP_INVALID;
	}
	return bmpId;
}


BitmapId BinFileLoader::makeBitmap(const char *filename)
{
	BitmapId bmpId = BITMAP_INVALID;
	const int MAX_PATH = 100;
	char path[MAX_PATH];
	FIL f;
	
	// look for .bin file first
	strncpy(path, filename, MAX_PATH); 
	path[MAX_PATH - 1] = 0;
	strncat(path, ".bin", MAX_PATH-strlen(path));
//f_unlink(path);
	if (FileSystem::f_open(&f, path, FA_READ) == FR_OK)
	{
		bmpId = readBinFile(&f);
		f_close(&f);
	}
	else
	{
		// is there a png?
		strncpy(path, filename, MAX_PATH); 
		path[MAX_PATH - 1] = 0;
		strncat(path, ".png", MAX_PATH-strlen(path));
		if (FileSystem::f_open(&f, path, FA_READ) == FR_OK)
		{
			FatFsStream stream(&f);
			upng png(&stream);

			png.header();
	
			uint16_t width = png.get_width();
			uint16_t height = png.get_height();
			upng_format format = png.get_format();
			uint16_t bpp = png.get_bpp();
			uint16_t color_depth = png.get_bitdepth();

			if (color_depth == 8 && (format == UPNG_RGB8 || format == UPNG_RGBA8))
			{
				//Create (24bit) dynamic bitmap of same dimension
				if ( format == UPNG_RGB8 )
					bmpId = Bitmap::dynamicBitmapCreate(width, height + 1, Bitmap::RGB888);
				else
					bmpId = Bitmap::dynamicBitmapCreate(width, height + 1, Bitmap::ARGB8888);
				
				if (bmpId != BITMAP_INVALID)
				{
					uint8_t* const data = Bitmap::dynamicBitmapGetAddress(bmpId);
					if (png.decode(data, (width + 1)*height * bpp) != UPNG_EOK)
					{
						f_close(&f);
						Bitmap::dynamicBitmapDelete(bmpId);
						bmpId = BITMAP_INVALID;
					}
					else
					{
						f_close(&f);

						if (format == UPNG_RGB8)
						{
							// convert RGB to BGR
							uint8_t *ptr = data;
							uint8_t *end = ptr + width*height * 3;
							while (ptr < end)
							{
								uint8_t tmp = *ptr;
								*ptr = *(ptr + 2);
								*(ptr + 2) = tmp;
								ptr += 3;
							}
						}
						else
						{
							// convert RGBA to BGRA
							uint8_t *ptr = data;
							uint8_t *end = ptr + width*height * bpp;
							while (ptr < end)
							{
								uint8_t tmp = *ptr;
								*ptr = *(ptr + 2);
								*(ptr + 2) = tmp;
								ptr += bpp;
							}
						}
						
						// create a new bin file
						strncpy(path, filename, MAX_PATH); 
						path[MAX_PATH - 1] = 0;
						strncat(path, ".bin", MAX_PATH-strlen(path));
					
						bool success = false;
						if (FileSystem::f_open(&f, path, FA_CREATE_NEW | FA_WRITE) == FR_OK)
						{
							char header[5];
							header[0] = width & 0xFF;
							header[1] = (width >> 8) & 0xFF;
							header[2] = height & 0xFF;
							header[3] = (height >> 8) & 0xFF;
							if ( format == UPNG_RGB8 )
								header[4] = (Bitmap::RGB888) & 0xFF;
							else
								header[4] = (Bitmap::ARGB8888) & 0xFF;
					
							UINT bytesToWrite = 5, bytesWritten = 0;
							FRESULT res = f_write(&f, header, bytesToWrite, &bytesWritten); 
							if (bytesToWrite == bytesWritten)
							{
								bytesToWrite = width*height * bpp;
								res = f_write(&f, data, bytesToWrite, &bytesWritten);
								if (bytesToWrite == bytesWritten)
								{
									success = true;	
								}
							}
							f_close(&f);
							if (!success)
							{
								f_unlink(path);	// remove potentially corrupted .bin file 
							}
						}
								
						Bitmap::dynamicBitmapDelete(bmpId);
						bmpId = BITMAP_INVALID;

						if (success)
						{
							if (FileSystem::f_open(&f, path, FA_READ) == FR_OK)
							{
								bmpId = readBinFile(&f);
								f_close(&f);
							}					
						}
					}
				}
			}
		}
	}
	return bmpId;
}

