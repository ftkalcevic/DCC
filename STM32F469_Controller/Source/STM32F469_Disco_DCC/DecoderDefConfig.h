#pragma  once

#include "Config.h"
#include "Decoders.h"
#include <vector>
#include <touchgfx/Unicode.hpp>
#include "DecoderDef.h"
#include <algorithm>
#include <memory>
#include <string.h>
#include "Utility.h"

using namespace touchgfx;

const char * const DD_Decoder = "Decoder";
const char * const DD_Details = "Details";
const char * const DD_UserTypes = "UserTypes";
const char * const DD_Type = "Type";
const char * const DD_BitField = "BitField";
const char * const DD_Enum = "Enum";
const char * const DD_Bit = "Bit";
const char * const DD_CVs = "CVs";
const char * const DD_CV = "CV";
const char * const DD_Groups = "Groups";
const char * const DD_Group = "Group";

const char * const DD_Attr_Manufacturer = "manufacturer";
const char * const DD_Attr_Type = "type";
const char * const DD_Attr_Name = "name";
const char * const DD_Attr_Bitmask = "bitmask";
const char * const DD_Attr_Min = "min";
const char * const DD_Attr_Max = "max";
const char * const DD_Attr_Value = "value";
const char * const DD_Attr_Bit = "bit";
const char * const DD_Attr_On = "on";
const char * const DD_Attr_Off = "off";
const char * const DD_Attr_Number = "number";
const char * const DD_Attr_Readonly = "readonly";
const char * const DD_Attr_CVCount = "cvcount";
const char * const DD_Attr_CVS = "cvs";

const char * const Path_Devices = "/config/devices"; 
const char * const Path_Images = "/Images";
const int DECODER_DEF_FILENAME_LEN = 20;

class DecoderDefConfig: public Config<127, 80>	// longest element is a description string, xpath max is about 70
{
	enum Elements
	{
		None,
		Decoder,
		Details,
		UserTypes,
		Type,
		BitField,
		Enum,
		Bit,
		CVs,
		CV,
		Groups,
		Group,
		AttrManufacturer,
		AttrType,
		AttrName,
		AttrBitmask,
		AttrMin,
		AttrMax,
		AttrValue,
		AttrBit,
		AttrOn,
		AttrOff,
		AttrNumber,
		AttrReadonly,
		AttrCVCount,
		AttrCVS
	};

	class DecoderDefFilename
	{
	public:
		char filename[DECODER_DEF_FILENAME_LEN];
	};
	std::vector<DecoderDefFilename> decoderDefFilenames;
	char fullpath[MAX_PATH];
	std::unique_ptr<DecoderDef> decoderDef;
	
protected:
	virtual const char *getPath() const { return fullpath;}
	virtual bool elementStart(const char *element) 
	{
		static ElementNames e[] = 
		{ 
			{ DD_Decoder, Decoder },
			{ DD_Details, Details },
			{ DD_UserTypes, UserTypes },
			{ DD_Type, Type },
			{ DD_BitField, BitField },
			{ DD_Enum, Enum },
			{ DD_Bit, Bit },
			{ DD_CVs, CVs },
			{ DD_CV, CV },
			{ DD_Groups, Groups },
			{ DD_Group, Group },
		};
		if (!matchElement(element, e, countof(e)))
			return false;
		return true;
	}
	virtual bool elementEnd() 
	{
		switch (activeElement)
		{
			case Type:		decoderDef->typeEnd(); break;
			case BitField:	decoderDef->getType()->bitFieldEnd(); break;
			case Enum:      decoderDef->getType()->getBitField()->enumEnd(); break;
			case Bit:       decoderDef->getType()->getBitField()->bitEnd(); break;
			case CV:		decoderDef->cvEnd(); break;
			case Group:		decoderDef->groupEnd(); break;
			default:
				break;
		}
		return true;
	}

	virtual bool attributeStart(const char *element) 
	{
		static ElementNames e[] = 
		{ 
			{ DD_Attr_Manufacturer, AttrManufacturer },
			{ DD_Attr_Type, AttrType },
			{ DD_Attr_Name, AttrName },
			{ DD_Attr_Bitmask, AttrBitmask },
			{ DD_Attr_Min, AttrMin },
			{ DD_Attr_Max, AttrMax },
			{ DD_Attr_Value, AttrValue },
			{ DD_Attr_Bit, AttrBit },
			{ DD_Attr_On, AttrOn },
			{ DD_Attr_Off, AttrOff },
			{ DD_Attr_Number, AttrNumber },
			{ DD_Attr_Readonly, AttrReadonly },
			{ DD_Attr_CVCount, AttrCVCount },
			{ DD_Attr_CVS, AttrCVS },
			
		};
		if (!matchAttribute(element, e, countof(e)))
			return false;
		
		return true;
	}
	virtual bool attributeEnd() 
	{
		switch (activeElement)
		{
			case Details:
				switch (activeAttribute)
				{
					case AttrManufacturer:		
						strcpy16(decoderDef->manufacturer, contentBuffer);
						break;
					case AttrType:
						decoderDef->type = decoderDef->parseDecoderType(contentBuffer);
						break;
					default:
						break;
				}
				break;
			case Type:
				switch (activeAttribute)
				{
					case AttrName:
						decoderDef->getType()->name = contentBuffer;
						break;
					case AttrType:
						decoderDef->getType()->type = decoderDef->parseDataType(contentBuffer);
						break;
				}
				break;
			case BitField:
				switch (activeAttribute)
				{
					case AttrName: 
						strcpy16(decoderDef->getType()->getBitField()->name, contentBuffer);
						break;
					case AttrType:
						decoderDef->getType()->getBitField()->type = decoderDef->parseDataType(contentBuffer);
						break;
					case AttrBitmask:
						decoderDef->getType()->getBitField()->bitmask = atoi(contentBuffer);
						break;
					case AttrMin:
						decoderDef->getType()->getBitField()->min = atoi(contentBuffer);
						break;
					case AttrMax:
						decoderDef->getType()->getBitField()->max = atoi(contentBuffer);
						break;
				}
				break;
			case Enum:
				switch (activeAttribute)
				{
					case AttrValue:	
						decoderDef->getType()->getBitField()->getEnum()->value = atoi(contentBuffer);
						break;
					case AttrName:
						strcpy16(decoderDef->getType()->getBitField()->getEnum()->name, contentBuffer);
						break;
				}
				break;
			case Bit:
				switch (activeAttribute)
				{
					case AttrBit:	
						decoderDef->getType()->getBitField()->getBit()->bit = atoi(contentBuffer);
						break;
					case AttrName:
						strcpy16(decoderDef->getType()->getBitField()->getBit()->name, contentBuffer);
						break;
					case AttrOn:
						strcpy16(decoderDef->getType()->getBitField()->getBit()->on, contentBuffer);
						break;
					case AttrOff:
						strcpy16(decoderDef->getType()->getBitField()->getBit()->off, contentBuffer);
						break;
				}
				break;
			case CV:
				switch (activeAttribute)
				{
					case AttrNumber:
						decoderDef->getCV()->number = atoi(contentBuffer);
						break;
					case AttrName:		
						strcpy16(decoderDef->getCV()->name, contentBuffer);
						break;
					case AttrType:
						decoderDef->parseCVDataType(contentBuffer, decoderDef->getCV() );
						break;
					case AttrMin:		
						decoderDef->getCV()->min = atoi(contentBuffer);
						break;
					case AttrMax:		
						decoderDef->getCV()->max = atoi(contentBuffer);
						break;
					case AttrReadonly:		
						decoderDef->getCV()->readonly = decoderDef->parseBool(contentBuffer);
						break;
					case AttrCVCount:		
						decoderDef->getCV()->cvcount = atoi(contentBuffer);
						break;
					default:
						break;
				}
				break;
			case Group:
				switch (activeAttribute)
				{
					case AttrName:	
						strcpy16(decoderDef->getGroup()->name, contentBuffer);
						break;
					case AttrCVS:
						decoderDef->parseCVList(contentBuffer, decoderDef->getGroup() );
						break;
				}
				break;
			default:
				break;
		}
		return true;
	}

	void addDecoderDef(const char *name)
	{
		decoderDefFilenames.resize(decoderDefFilenames.size() + 1);
		strncpy(decoderDefFilenames.back().filename, name, DECODER_DEF_FILENAME_LEN);
		decoderDefFilenames.back().filename[DECODER_DEF_FILENAME_LEN - 1] = 0;
	}
	
public:
	DecoderDefConfig()
	{
		decoderDef = nullptr;
		fullpath[0] = 0;
		decoderDefFilenames.reserve(64);
	}
	
	const char * operator[](int index) const
	{
		assert(index < decoderDefFilenames.size() && "Index out of range");
		if (index < decoderDefFilenames.size())
		{
			return decoderDefFilenames[index].filename;
		}
		return "";
	}
	
	uint16_t Count() const 
	{
		return decoderDefFilenames.size();
	}
	

	void init()
	{
		DIR d;
		FRESULT res = f_opendir(&d, Path_Devices);
		if (res != FR_OK)
			return;
	
		// Read the filenames of all .xml files in the decoders directory.  Only remember the name (no extension).
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
				if (len > 4 && strcasecmp(finfo.fname + len - 4, ".xml") == 0)
				{
					finfo.fname[len - 4] = 0;
					addDecoderDef(finfo.fname);
				}
			}
			else
			{
				break;
			}
		}
		f_closedir(&d);
		
		// Sort alphabetically.
		std::sort( decoderDefFilenames.begin(), decoderDefFilenames.end(),  
				   [](const DecoderDefFilename & a, const DecoderDefFilename & b) -> bool
					{ 
						return strcasecmp( a.filename, b.filename) < 0; 
					});
	}

	void clear()
	{
		decoderDef.reset(nullptr);
	}
	
	DecoderDef &getDecoderDef() const 
	{ 
		assert(decoderDef != nullptr && "getDecoderDef not yet loaded");
		return *decoderDef; 
	}
	
	void loadDecoderDef(const char *filename)
	{
		strcpy(fullpath, Path_Devices);
		strncat(fullpath, "/", MAX_PATH - strlen(fullpath));
		strncat(fullpath, filename, MAX_PATH - strlen(fullpath));
		strncat(fullpath, ".xml", MAX_PATH - strlen(fullpath));

		decoderDef.reset(new DecoderDef);

		parse();
	}
};

extern DecoderDefConfig decoderDefinitions;
