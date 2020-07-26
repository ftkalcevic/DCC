#include "BitmapManager.h"
#include <gui/common/BinFileLoader.h>


std::map<std::string, touchgfx::BitmapId, BitmapManager::cmpNoCase> BitmapManager::bmps;


touchgfx::BitmapId BitmapManager::FindBitmap(const char *file)
{
	auto it = bmps.find(file);
	if (it != bmps.end())
		return it->second;
	
	std::string path = "/Images/";
	path += file;
	BitmapId id = BinFileLoader::makeBitmap(path.c_str());
	if (id != BITMAP_INVALID)
		bmps[file] = id;
	return id;
}


void BitmapManager::Clear()
{
	for (auto it = bmps.begin(); it != bmps.end(); it++)
		Bitmap::dynamicBitmapDelete(it->second);

	bmps.clear();
}
