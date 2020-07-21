#pragma once

#include <vector>
#include <memory>
#include <assert.h>

class ImageFile
{
public:
	std::string filename;
	std::u16string displayFilename;
	uint16_t width;
	uint16_t height;
	uint16_t format;
};

class ImageFiles
{
public:
	std::vector<std::shared_ptr<ImageFile>> imageFiles;
	void Init();
};

extern ImageFiles imageFiles;
