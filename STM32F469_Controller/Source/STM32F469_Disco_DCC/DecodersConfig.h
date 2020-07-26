#pragma  once

#include "Config.h"
#include "Decoders.h"
#include <vector>
#include <touchgfx/Unicode.hpp>

using namespace touchgfx;

const char * const DC_DecodersConfig = "DecodersConfig";
const char * const DC_Decoder = "Decoder";
const char * const DC_Decoders = "Decoders";
const char * const DC_DecoderName = "Name";
const char * const DC_DecoderDesc = "Description";
const char * const DC_DecoderAddr = "Address";
const char * const DC_DecoderType = "Type";
const char * const DC_DecoderCV29 = "CV29";
const char * const DC_DecoderLocoSpeedSteps = "LocoSS";
const char * const DC_DecoderDefFile = "DecoderDef";
const char * const DC_DecoderSmallImageType = "SmallImageType";
const char * const DC_DecoderSmallImageFile = "SmallImageFile";
const char * const DC_DecoderSmallImageAttr = "SmallImageAttr";
const char * const DC_DecoderLargeImageType = "LargeImageType";
const char * const DC_DecoderLargeImageFile = "LargeImageFile";
const char * const DC_DecoderLargeImageAttr = "LargeImageAttr";

class DecodersConfig: public Config<127, 80>	// longest element is a description string, xpath max is about 70
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
		DecoderLocoSpeedSteps,
		DecoderDefinitionFile,
		DecoderSmallImageType,
		DecoderSmallImageFile,
		DecoderSmallImageAttr,
		DecoderLargeImageType,
		DecoderLargeImageFile,
		DecoderLargeImageAttr
	};

	std::vector<Decoders *> decoders;
	int activeDecoder;
	
	
protected:
	virtual const char *getPath() const { return "/config/DecodersConfig.xml";}
	virtual bool elementStart(const char *element) 
	{
		static ElementNames e[] = 
		{ 
			{ DC_Decoder, Decoder },
			{ DC_DecoderName, DecoderName },
			{ DC_DecoderDesc, DecoderDescription },
			{ DC_DecoderAddr, DecoderAddress },
			{ DC_DecoderType, DecoderType },
			{ DC_DecoderCV29, DecoderCV29 },
			{ DC_DecoderLocoSpeedSteps, DecoderLocoSpeedSteps },
			{ DC_DecoderDefFile, DecoderDefinitionFile },
			{ DC_DecoderSmallImageType, DecoderSmallImageType },
			{ DC_DecoderSmallImageFile,	DecoderSmallImageFile },
			{ DC_DecoderSmallImageAttr,	DecoderSmallImageAttr },
			{ DC_DecoderLargeImageType,	DecoderLargeImageType },
			{ DC_DecoderLargeImageFile,	DecoderLargeImageFile },
			{ DC_DecoderLargeImageAttr,	DecoderLargeImageAttr }
		};
		if (!matchElement(element, e, countof(e)))
			return false;
		
		if ( activeElement == Decoder )
			newDecoder();
		return true;
	}
	virtual bool elementEnd() 
	{
		switch (activeElement)
		{
			case DecoderName:			decoders.back()->setName(contentBuffer); break;
			case DecoderDescription:	decoders.back()->setDescription(contentBuffer); break;
			case DecoderAddress:		decoders.back()->setAddress(atoi(contentBuffer)); break;
			case DecoderType:			decoders.back()->setType((EDecoderType::EDecoderType)atoi(contentBuffer)); break;
			case DecoderCV29:			decoders.back()->setConfig(atoi(contentBuffer)); break;
			case DecoderLocoSpeedSteps:	if ( decoders.back()->getType() == EDecoderType::Multifunction ) decoders.back()->getLoco().setSpeedSteps((ESpeedSteps::ESpeedSteps)atoi(contentBuffer)); break;
			case DecoderDefinitionFile:	decoders.back()->setDecoderDefFilename(contentBuffer); break;
			case DecoderSmallImageType: decoders.back()->setSmallImageType((EUserImage::EUserImage)atoi(contentBuffer)); break;
			case DecoderSmallImageFile: decoders.back()->setSmallImageFile(contentBuffer); break;
			case DecoderSmallImageAttr: decoders.back()->setSmallImageAttributes((EImageAttributes::EImageAttributes)atoi(contentBuffer)); break;
			case DecoderLargeImageType: decoders.back()->setLargeImageType((EUserImage::EUserImage)atoi(contentBuffer)); break;
			case DecoderLargeImageFile:	decoders.back()->setLargeImageFile(contentBuffer); break;
			case DecoderLargeImageAttr: decoders.back()->setLargeImageAttributes((EImageAttributes::EImageAttributes)atoi(contentBuffer)); break;
			default:
				break;
		}
		return true;
	}

	void addDecoder(Decoders *decoder)
	{
		decoders.push_back(decoder);
	}
	
public:
	DecodersConfig()
	{
		decoders.reserve(128);
		activeDecoder = -1;
	}
	
	void write()
	{
		ConfigStream stream(getPath());
		stream.WriteStartElement( DC_DecodersConfig );
		
		stream.WriteStartElement( DC_Decoders );
		for (auto it = decoders.begin(); it != decoders.end(); it++)
		{
			Decoders *d = *it;
			stream.WriteStartElement( DC_Decoder );
			stream.WriteElement( DC_DecoderName, d->getName() );
			stream.WriteElement( DC_DecoderDesc, d->getDescription() );
			stream.WriteElement( DC_DecoderAddr, d->getAddress() );
			stream.WriteElement( DC_DecoderType, d->getType() );
			stream.WriteElement( DC_DecoderDefFile, d->getDecoderDefFilename() );
			stream.WriteElement( DC_DecoderCV29, d->getConfig() );
			stream.WriteElement( DC_DecoderSmallImageType, d->getSmallImageType() );
			stream.WriteElement( DC_DecoderSmallImageFile, d->getSmallImageFile() );	
			stream.WriteElement( DC_DecoderSmallImageAttr, d->getSmallImageAttributes() );	
			stream.WriteElement( DC_DecoderLargeImageType, d->getLargeImageType() );
			stream.WriteElement( DC_DecoderLargeImageFile, d->getLargeImageFile() );
			stream.WriteElement( DC_DecoderLargeImageAttr, d->getLargeImageAttributes() );
			if (d->getType() == EDecoderType::Multifunction)
			{
				stream.WriteElement( DC_DecoderLocoSpeedSteps, d->getLoco().getSpeedSteps() );
			}
			stream.WriteEndElement( DC_Decoder );
		}
		stream.WriteEndElement( DC_Decoders );
		
		stream.WriteEndElement( DC_DecodersConfig );
		stream.Close();
	}

	Decoders & operator[](int index) const
	{
		assert(index < decoders.size() && "Index out of range");
		if (index < decoders.size())
		{
			return *(decoders[index]);
		}
		static Decoders d;
		return d;
	}
	
	uint16_t Count() const 
	{
		return decoders.size();
	}
	
	int newDecoder()
	{
		Decoders *decoder = new Decoders();
		memset(decoder, 0, sizeof(Decoders));
		decoders.push_back(decoder);
		decoder->setName(u"New Decoder");
		decoder->setDescription(u"");
		decoder->setAddress(0);
		decoder->setType(EDecoderType::Multifunction);
		decoder->setSmallImageType(EUserImage::LocoSteamIcon);
		decoder->setLargeImageType(EUserImage::LocoSteamIcon);
		return decoders.size()-1;
	}
	
	int getActiveDecoder() const
	{
		return activeDecoder;
	}
	
	void setActiveDecoder(int decoder)
	{
		activeDecoder = decoder;
	}
	
	int16_t findEncoder(int16_t address)
	{
		for (int i = 0; i < decoders.size(); i++)
			if (decoders[i]->getAddress() == address)
				return i;
		return -1;
	}
	
	void deleteDecoder(int item)
	{
		if (item >= 0 && item < decoders.size())
		{
			decoders.erase(decoders.begin() + item);
		}
	}
	
};

extern DecodersConfig uiDecodersConfig;
