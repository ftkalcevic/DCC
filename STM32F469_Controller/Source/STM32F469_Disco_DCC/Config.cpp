#include "Config.h"
#include <string.h>
#include <assert.h>
#include "yxml.h"

#include "FileSystem.h"


void Config::parse()
{
	const int BUFFER_SIZE = 50;
	uint8_t buffer[BUFFER_SIZE];
	yxml_t xml;
	
	yxml_init(&xml, buffer, BUFFER_SIZE);
	
	FIL file;
	if (FileSystem::f_open(&file, getPath(), FA_READ) == FR_OK)
	{
		bool run = true;
		while ( run )
		{
			uint8_t readBuffer[50];
			UINT bytesRead;
			if (f_read(&file, readBuffer, sizeof(readBuffer), &bytesRead) == FR_OK)
			{
				if (bytesRead == 0)
				{
					// Done
					break;
				}
				
				for (int i = 0; i < bytesRead && run; i++)
				{
					char c = (char)readBuffer[i];
					yxml_ret_t r = yxml_parse(&xml, c);
					if (r < 0)
					{
						// error
						break;
					}
					switch (r)
					{
						case YXML_OK:				/* Character consumed, no new token present   */
							break;
						case YXML_ELEMSTART:		/* Start of an element:   '<Tag ..'           */
							if ( !elementStart(xml.elem) ) run = false;
							break;
						case YXML_CONTENT:			/* Element content                            */
							if ( !elementContent(xml.data) ) run = false;
							break;
						case YXML_ELEMEND:			/* End of an element:     '.. />' or '</Tag>' */
							if ( !elementEnd() ) run = false;
							break;
						case YXML_ATTRSTART:		/* Attribute:             'Name=..'           */
						case YXML_ATTRVAL:			/* Attribute value                            */
						case YXML_ATTREND:			/* End of attribute       '.."'               */
						case YXML_PISTART:			/* Start of a processing instruction          */
						case YXML_PICONTENT:		/* Content of a PI                            */
						case YXML_PIEND:			/* End of a processing instruction            */						
							break;
						case YXML_EEOF:				/* Unexpected EOF                             */
						case YXML_EREF:				/* Invalid character or entity reference (&whatever;) */
						case YXML_ECLOSE:			/* Close tag does not match open tag (<Tag> .. </OtherTag>) */
						case YXML_ESTACK:			/* Stack overflow (too deeply nested tags or too long element/attribute name) */
						case YXML_ESYN:				/* Syntax error (unexpected byte)             */
						default:
							assert(false && "Error condition in yxml");
							break;
					}
				}
			}
		}
		
		f_close(&file);
	}
}


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

