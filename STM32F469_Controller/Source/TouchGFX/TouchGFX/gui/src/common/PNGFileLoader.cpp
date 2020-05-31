#include <gui/common/PNGFileLoader.h>
#include <touchgfx/Color.hpp>
#include <stdio.h>
extern FATFS FatFs;

//FatFsStream::FatFsStream(const char *filename)
//{
//	if (f_open(&file, filename, FA_READ) == FR_OK)
//		open = true;
//	else
//		open = false;
//}
//
//uint16_t FatFsStream::ReadBytes(uint8_t *buffer, uint16_t len, uint16_t &bytesread)
//{
//	UINT read = 0;
//	f_read(&file, buffer, len, &read);
//	bytesread = read;
//}
//
//uint32_t FatFsStream::GetSize()
//{
//	return f_size(&file);
//}
//
//void FatFsStream::Seek(uint32_t offset, uint8_t whence)
//{
//	switch (whence)
//	{
//		case SEEK_SET: f_lseek(&file, offset); break;
//		case SEEK_CUR: f_lseek(&file, f_tell(&file) + offset); break;
//		case SEEK_END: f_lseek(&file, f_size(&file) - offset); break;
//	}
//}



BitmapId PNGFileLoader::makeBitmap(const char *filename)
{
	FatFsStream stream(filename);
	upng png(&stream);
	
	png.header();
	
	uint16_t width = png.get_width();
	uint16_t height = png.get_height();
	uint16_t depth = png.get_bitdepth();
    
    //Create (24bit) dynamic bitmap of same dimension
    BitmapId bmpId = Bitmap::dynamicBitmapCreate(width, height+1, Bitmap::RGB888);
	if (bmpId != BITMAP_INVALID)
	{
		uint8_t* const data = Bitmap::dynamicBitmapGetAddress(bmpId);
		if (png.decode(data, (width + 1)*height * 3) != UPNG_EOK)
		{
			Bitmap::dynamicBitmapDelete(bmpId);
			bmpId = BITMAP_INVALID;
		}
	}
	return bmpId;
}

