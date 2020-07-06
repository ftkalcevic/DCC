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

void ConfigStream::WriteStartElementTag(const char *elem)
{
	UINT bytesWritten;
	f_write(&file, "<", 1, &bytesWritten);
	f_write(&file, elem, strlen(elem), &bytesWritten);
	f_write(&file, ">", 1, &bytesWritten);
}

void ConfigStream::WriteStartElement(const char *elem)
{
	WriteStartElementTag(elem);
	UINT bytesWritten;
	f_write(&file, "\n", 1, &bytesWritten);
}

void ConfigStream::WriteElement(const char *elem, int value)
{
	WriteStartElementTag(elem);
	UINT bytesWritten;
	char buffer[12];
	itoa(value, buffer, 10);
	f_write(&file, buffer, strlen(buffer), &bytesWritten);
	WriteEndElement(elem);
}

void ConfigStream::WriteElement(const char *elem, const char16_t *str)
{
	WriteStartElementTag(elem);
	while (*str != 0)
	{
		UINT bytesWritten;
		switch (*str)
		{
			case '&': f_write(&file, "&amp;", 5, &bytesWritten); break;
			case '<': f_write(&file, "&lt;", 4, &bytesWritten); break;
			case '>': f_write(&file, "&gt;", 4, &bytesWritten); break;
			case '\'': f_write(&file, "&apos;", 6, &bytesWritten); break;
			case '\"': f_write(&file, "&quot;", 6, &bytesWritten); break;
			default:
				if ( *str == '\t' || *str == '\n' || *str == '\r' || (*str >= 32 && *str < 128 ) )
					f_write(&file, str, 1, &bytesWritten);
				else
				{
					char unicode[12];
					snprintf(unicode, countof(unicode), "&#x%04X;", *str);
					f_write(&file, unicode, strlen(unicode), &bytesWritten);
				}
		}
		str++;
	}
	WriteEndElement(elem);
}

void ConfigStream::WriteElement(const char *elem, const char *str)
{
	WriteStartElementTag(elem);
	UINT bytesWritten;
	f_write(&file, str, strlen(str), &bytesWritten);
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

