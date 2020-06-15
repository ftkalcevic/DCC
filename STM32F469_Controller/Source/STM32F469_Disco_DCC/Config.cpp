#include "Config.h"
#include <string.h>

#include "FileSystem.h"


ConfigStream::ConfigStream(const char *path)
{
	filePath = path;
	strncpy(tempPath, path, countof(tempPath));
	tempPath[countof(tempPath) - 1] = 0;
	strncat(tempPath, ".new", countof(tempPath)-strlen(tempPath));
	
	FRESULT res = FileSystem::f_open(&file, tempPath, FA_WRITE | FA_CREATE_ALWAYS);
	if ( res == FR_OK)
	{
	}
}

void ConfigStream::WriteStartElement(const char *elem)
{
	UINT bytesWritten;
	f_write(&file, "<", 1, &bytesWritten);
	f_write(&file, elem, strlen(elem), &bytesWritten);
	f_write(&file, ">\n", 2, &bytesWritten);
}

void ConfigStream::WriteElement(const char *elem, int value)
{
	UINT bytesWritten;
	f_write(&file, "<", 1, &bytesWritten);
	f_write(&file, elem, strlen(elem), &bytesWritten);
	f_write(&file, ">", 1, &bytesWritten);
	char buffer[12];
	itoa(value, buffer, 10);
	f_write(&file, buffer, strlen(buffer), &bytesWritten);
	WriteEndElement(elem);
}

void ConfigStream::WriteEndElement(const char *elem)
{
	UINT bytesWritten;
	f_write(&file, "</", 2, &bytesWritten);
	f_write(&file, elem, strlen(elem), &bytesWritten);
	f_write(&file, ">\n", 2, &bytesWritten);
}

void ConfigStream::Close()
{
	FRESULT res = f_close(&file);
	res = f_unlink(filePath);
	res = f_rename(tempPath, filePath);
	res = FR_OK;
}

