#pragma  once

#include "Config.h"
#include "Decoders.h"
#include <vector>
#include <touchgfx/Unicode.hpp>
#include <algorithm>

using namespace touchgfx;

//const char * const DC_DecodersConfig = "DecodersConfig";
//const char * const DC_Decoder = "Decoder";
//const char * const DC_Decoders = "Decoders";
//const char * const DC_DecoderName = "Name";
//const char * const DC_DecoderDesc = "Description";
//const char * const DC_DecoderAddr = "Address";
//const char * const DC_DecoderType = "Type";
//const char * const DC_DecoderCV29 = "CV29";
//const char * const DC_DecoderLocoSpeedSteps = "LocoSS";

const char * const Path_Devices = "/config/devices"; 
const int MAX_FILENAME_LEN = 20;

class DecoderDefConfig: public Config<127, 80>	// longest element is a description string, xpath max is about 70
{
	enum Elements
	{
		None,
		Decoder,
		DecoderName,
		DecoderDescription,
		DecoderAddress,
		DecoderType,
		DecoderCV29,
		DecoderLocoSpeedSteps
	};

	class DecoderDefFilename
	{
	public:
		char filename[MAX_FILENAME_LEN];
	};
	std::vector<DecoderDefFilename> decoderDefFilenames;
	
protected:
	virtual const char *getPath() const { return "/config/DecodersConfig.xml";}
	virtual bool elementStart(const char *element) 
	{
//		static ElementNames e[] = 
//		{ 
//			{ DC_Decoder, Decoder },
//			{ DC_DecoderName, DecoderName },
//			{ DC_DecoderDesc, DecoderDescription },
//			{ DC_DecoderAddr, DecoderAddress },
//			{ DC_DecoderType, DecoderType },
//			{ DC_DecoderCV29, DecoderCV29 },
//			{ DC_DecoderLocoSpeedSteps, DecoderLocoSpeedSteps },
//		};
//		if (!matchElement(element, e, countof(e)))
//			return false;
//		
//		if ( activeElement == Decoder )
//			newDecoder();
		return true;
	}
	virtual bool elementEnd() 
	{
		switch (activeElement)
		{
//			case DecoderName:			decoders.back()->setName(contentBuffer); break;
//			case DecoderDescription:	decoders.back()->setDescription(contentBuffer); break;
//			case DecoderAddress:		decoders.back()->setAddress(atoi(contentBuffer)); break;
//			case DecoderType:			decoders.back()->setType((EDecoderType::EDecoderType)atoi(contentBuffer)); break;
//			case DecoderCV29:			decoders.back()->setConfig(atoi(contentBuffer)); break;
//			case DecoderLocoSpeedSteps:	if ( decoders.back()->getType() == EDecoderType::Multifunction ) decoders.back()->getLoco().setSpeedSteps((ESpeedSteps::ESpeedSteps)atoi(contentBuffer)); break;
			default:
				break;
		}
		return true;
	}

	void addDecoderDef(const char *name)
	{
		decoderDefFilenames.resize(decoderDefFilenames.size() + 1);
		strncpy(decoderDefFilenames.back().filename, name, MAX_FILENAME_LEN);
		decoderDefFilenames.back().filename[MAX_FILENAME_LEN - 1] = 0;
	}
	
public:
	DecoderDefConfig()
	{
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
				if (len > 4 && strcmp(finfo.fname + len - 4, ".xml") == 0)
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
		
		std::sort( decoderDefFilenames.begin(), decoderDefFilenames.end(),  
				   [](const DecoderDefFilename & a, const DecoderDefFilename & b) -> bool
					{ 
						return strcmp( a.filename, b.filename) < 0; 
					});
	}
};

extern DecoderDefConfig decoderDefinitions;
