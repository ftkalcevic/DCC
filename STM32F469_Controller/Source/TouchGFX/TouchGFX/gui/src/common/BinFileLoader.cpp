#include <gui/common/BinFileLoader.h>
#include <touchgfx/Color.hpp>
#include <stdio.h>
#include <string.h>
#include "FileSystem.h"
#include "../STM32F469_Disco_DCC/upng.h"


class FatFsStream : public Stream
{
public:
	FatFsStream(FIL *f) : file(f)
	{
	}

	uint16_t ReadBytes(uint8_t *buffer, uint16_t len, uint16_t &bytesread)
	{
		UINT read = 0;
		FRESULT res = f_read(file, buffer, len, &read);
		bytesread = read;
		return res;
	}

	uint32_t GetSize()
	{
		return f_size(file);
	}

	void Seek(uint32_t offset, uint8_t whence)
	{
		switch (whence)
		{
			case SEEK_SET: f_lseek(file, offset); break;
			case SEEK_CUR: f_lseek(file, f_tell(file) + offset); break;
			case SEEK_END: f_lseek(file, f_size(file) - offset); break;
		}
	}
private:
	FIL *file;
};




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

	if (format != Bitmap::RGB888)	
		return BITMAP_INVALID;
	
    BitmapId bmpId = Bitmap::dynamicBitmapCreate(width, height, Bitmap::RGB888);
    uint8_t* buffer = Bitmap::dynamicBitmapGetAddress(bmpId);
	
	// everything aligns.  Just copy the data into the buffer... 
    UINT bytesToRead = width * height * 3;
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
			uint16_t format = png.get_bitdepth();
    
			//Create (24bit) dynamic bitmap of same dimension
			bmpId = Bitmap::dynamicBitmapCreate(width, height + 1, Bitmap::RGB888);
			if (bmpId != BITMAP_INVALID)
			{
				uint8_t* const data = Bitmap::dynamicBitmapGetAddress(bmpId);
				if (png.decode(data, (width + 1)*height * 3) != UPNG_EOK)
				{
					f_close(&f);
					Bitmap::dynamicBitmapDelete(bmpId);
					bmpId = BITMAP_INVALID;
				}
				else
				{
					f_close(&f);

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
						header[4] = (Bitmap::RGB888) & 0xFF;
					
						UINT bytesToWrite = 5, bytesWritten = 0;
						FRESULT res = f_write(&f, header, bytesToWrite, &bytesWritten); 
						if (bytesToWrite == bytesWritten)
						{
							bytesToWrite = width*height * 3;
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
	return bmpId;
}

