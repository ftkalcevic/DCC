#include <gui/common/TGAFileLoader.h>
#include <touchgfx/Color.hpp>

extern FATFS FatFs;


int TGAFileLoader::readFile(FIL *hdl, uint8_t*  buffer, uint32_t length)
{
	UINT  r = 0;
	f_read(hdl, buffer, length, &r);
    if (r != length)
    {
        return 1;
    }
    return 0;
}

void TGAFileLoader::seekFile(FIL *hdl, uint32_t offset)
{
	f_lseek(hdl, offset);
}

void TGAFileLoader::getBMP24Dimensions(FIL *fileHandle, uint16_t& width, uint16_t& height, uint16_t &depth)
{
	const int HEADER_BYTES = 18;
    uint8_t data[HEADER_BYTES];
    seekFile(fileHandle, 0);
    readFile(fileHandle, data, HEADER_BYTES); //read first part of header.

    width = data[12] | (data[13] << 8);
    height = data[14] | (data[15] << 8);
	depth = data[16];
}

void TGAFileLoader::readBMP24File(Bitmap bitmap, FIL * fileHandle)
{
	const int HEADER_BYTES = 18;
    uint8_t data[HEADER_BYTES];
    seekFile(fileHandle, 0);
    readFile(fileHandle, data, HEADER_BYTES); //read first part of header.

    uint16_t width = data[12] | (data[13] << 8);
    uint16_t height = data[14] | (data[15] << 8);
	uint16_t depth = data[16];

    //get dynamic bitmap boundaries
    const uint32_t buffer_width = bitmap.getWidth();
    const uint32_t buffer_height = bitmap.getHeight();

    const Bitmap::BitmapFormat format = bitmap.getFormat();
    uint8_t* const  buffer8  = Bitmap::dynamicBitmapGetAddress(bitmap.getId());
    uint16_t* const buffer16 = (uint16_t*)buffer8;

	if (buffer_width == width && bitmap.getFormat() == Bitmap::RGB888 && depth == 24)
	{
		// everything aligns.  Just copy the data into the buffer.
		readFile(fileHandle, buffer8, buffer_width * buffer_height * ((depth+7)/8) );
	}
////	else
//	{
//        for (uint32_t y = 0; y < height; y++)
//        {
//            for (uint32_t x = 0; x < width; x++)
//            {
//                if (x % 10 == 0) //read data every 10 pixels = 30 bytes
//                {
//                    if (x + 10 <= width) //read 10
//                    {
//                        readFile(fileHandle, data, 10 * 3); //10 pixels
//                    }
//                    else
//                    {
//                        readFile(fileHandle, data, (width - x) * 3 + rowpadding); //rest of line
//                    }
//                }
//                //insert pixel, if within dynamic bitmap boundaries
//                if (x < buffer_width && ((height - y - 1) < buffer_height))
//                {
//                    switch (format)
//                    {
//                    case Bitmap::RGB565:
//                        buffer16[x + (height - y - 1) * buffer_width] =
//                            touchgfx::Color::getColorFrom24BitRGB(data[(x % 10) * 3 + 2], data[(x % 10) * 3 + 1], data[(x % 10) * 3]);
//                        break;
//                    case Bitmap::RGB888:
//                        {
//                            //24 bit framebuffer
//                            const uint32_t inx = 3 * (x + (height - y - 1) * buffer_width);
//                            buffer8[inx + 0] = data[(x % 10) * 3 + 2];
//                            buffer8[inx + 1] = data[(x % 10) * 3 + 1];
//                            buffer8[inx + 2] = data[(x % 10) * 3 + 0];
//                            break;
//                        }
//                    case Bitmap::ARGB8888:
//                        {
//                            //24 bit framebuffer
//                            const uint32_t inx = 4 * (x + (height - y - 1) * buffer_width);
//                            buffer8[inx + 0] = data[(x % 10) * 3 + 0];
//                            buffer8[inx + 1] = data[(x % 10) * 3 + 1];
//                            buffer8[inx + 2] = data[(x % 10) * 3 + 2];
//                            buffer8[inx + 3] = 255; //solid
//                            break;
//                        }
//                    default:
//                        assert(!"Unsupported bitmap format in TGAFileLoader!");
//                    }
//                }
//            }
//	    }
//    }
}
