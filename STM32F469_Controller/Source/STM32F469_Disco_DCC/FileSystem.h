#pragma once

#include "fatfs.h"
#include "Stream.h"
#include <stdio.h>


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


class FatFsStream : public Stream
{
public:
	FatFsStream(FIL *f) : file(f)
	{
	}

	uint16_t ReadBytes(uint8_t *buffer, uint16_t len, uint16_t &bytesread)
	{
		UINT read = 0;
		FRESULT res = f_read(file, buffer, len, &read);
		bytesread = read;
		return res;
	}

	uint32_t GetSize()
	{
		return f_size(file);
	}

	void Seek(uint32_t offset, uint8_t whence)
	{
		switch (whence)
		{
			case SEEK_SET: f_lseek(file, offset); break;
			case SEEK_CUR: f_lseek(file, f_tell(file) + offset); break;
			case SEEK_END: f_lseek(file, f_size(file) - offset); break;
		}
	}
private:
	FIL *file;
};

