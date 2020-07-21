#include "ImageFile.h"
#include "main.h"
#include "DecoderDefConfig.h"
#include "FileSystem.h"
#include "../STM32F469_Disco_DCC/upng.h"



void ImageFiles::Init()
{
	DIR d;
	FRESULT res = f_opendir(&d, Path_Images);
	if (res != FR_OK)
		return;
	
	// Read the filenames of all .png files in the Images directory.
	FILINFO finfo;
	char filename[sizeof(finfo.fname)];
	for(;;)
	{
		res = f_readdir(&d, &finfo);
		if (res == FR_OK)
		{
			if (finfo.fname[0] == 0)
				break;
			
			int len = strlen(finfo.fname);
			if (len > 4 && strcasecmp(finfo.fname + len - 4, ".png") == 0)
			{
				std::shared_ptr<ImageFile> f = std::make_shared<ImageFile>();
				f->filename = finfo.fname;
				f->filename.resize(len - 4);
				strcpy16(f->displayFilename, finfo.fname );
				f->width = 0;
				f->height = 0;
				f->format = 0;

				FIL file;
				std::string path;
				path = Path_Images;
				path += "/";
				path += finfo.fname;
				
				if (FileSystem::f_open(&file, path.c_str(), FA_READ) == FR_OK)
				{
					FatFsStream stream(&file);
					upng png(&stream);

					png.header();
	
					f->width = png.get_width();
					f->height = png.get_height();
					f->format = png.get_bitdepth();
					f_close(&file);
				}
			
				imageFiles.push_back(f);
			}
		}
		else
		{
			break;
		}
	}
	f_closedir(&d);
		
	// Sort alphabetically.
	std::sort( imageFiles.begin(), imageFiles.end(),  
				[](const std::shared_ptr<ImageFile> & a, const std::shared_ptr<ImageFile> & b) -> bool
				{ 
					return strcasecmp( a->filename.c_str(), b->filename.c_str()) < 0; 
				});
}
