#include <touchgfx/hal/Types.hpp>
#include <touchgfx/Bitmap.hpp>
#include "fatfs.h"

using namespace touchgfx;

class BinFileLoader
{
	static BitmapId readBinFile(FIL *f);
public:
	static BitmapId makeBitmap(const char *filename);
};
