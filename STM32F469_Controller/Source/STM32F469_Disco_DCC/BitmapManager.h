#pragma once

#include <touchgfx/Bitmap.hpp>
#include <map>
#include <string.h>
#include <string>

class BitmapManager
{
    struct cmpNoCase {
        bool operator()(const std::string& a, const std::string& b) const {
	        return strcasecmp(a.c_str(), b.c_str()) < 0;
        }
    };
	static std::map<std::string, touchgfx::BitmapId, BitmapManager::cmpNoCase> bmps;
public:
	static touchgfx::BitmapId FindBitmap(const char *file);
	void Clear();
};

