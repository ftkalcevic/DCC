#include <touchgfx/hal/Types.hpp>
#include <touchgfx/Bitmap.hpp>
#ifndef SIMULATOR
#include "fatfs.h"
#endif
using namespace touchgfx;

class BinFileLoader
{
	static BitmapId readBinFile(FIL *f);
public:
	static BitmapId makeBitmap(const char *filename);
};
