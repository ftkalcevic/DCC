#pragma once

#include "fatfs.h"

class FileSystem
{
	static FATFS FatFs;
	static bool initialised;
	
	static void Initialise();
	
public:
	static FRESULT f_open(FIL* fp, const TCHAR* path, BYTE mode)
	{
		Initialise();
		return ::f_open(fp, path, mode);
	}
};
