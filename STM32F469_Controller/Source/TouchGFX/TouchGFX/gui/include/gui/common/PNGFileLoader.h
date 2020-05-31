#include <touchgfx/hal/Types.hpp>
#include <touchgfx/Bitmap.hpp>
#include "fatfs.h"
#include "../STM32F469_Disco_DCC/upng.h"

using namespace touchgfx;

//class FatFsStream : public Stream
//{
//public:
//	FatFsStream(const char *file);
//	virtual uint16_t ReadBytes(uint8_t *buffer, uint16_t len, uint16_t &bytesread);
//	virtual uint32_t GetSize();
//	virtual void Seek(uint32_t offset, uint8_t whence);
//private:
//	FIL file;
//	bool open;
//};

class PNGFileLoader
{
public:
	BitmapId makeBitmap(const char *filename);

};
