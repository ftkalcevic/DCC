#include <touchgfx/hal/Types.hpp>
#include <touchgfx/Bitmap.hpp>
#ifndef SIMULATOR
#include "fatfs.h"
#endif
using namespace touchgfx;

class TGAFileLoader
{
public:
	static void getBMP24Dimensions(FIL *fileHandle, uint16_t& width, uint16_t& height, uint16_t &depth);
    static void readBMP24File(Bitmap bitmap, FIL *fileHandle);
private:
    static int readFile(FIL *hdl, uint8_t* buffer, uint32_t length);
    static void seekFile(FIL *hdl, uint32_t offset);
};
