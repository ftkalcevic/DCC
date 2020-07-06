#pragma  once

#include "Config.h"
#include "Decoders.h"
#include <vector>
#include <touchgfx/Unicode.hpp>

using namespace touchgfx;

class DecodersConfig: public Config<127, 80>	// longest element is a description string, xpath max is about 70
{
	enum Elements
	{
		None,
		Decoder,
		DecoderName,
		DecoderDescription,
		DecoderType,
		DecoderCV29		
	};

	std::vector<Decoders *> decoders;
	int activeDecoder;
	
	
protected:
	virtual const char *getPath() const { return "/config/DecodersConfig.xml";}
	virtual bool elementStart(const char *element) 
	{
		static ElementNames e[] = 
		{ 
			{ "Decoder", Decoder },
			{ "Name", DecoderName },
			{ "Description", DecoderDescription },
			{ "Type", DecoderType },
			{ "CV29", DecoderCV29 },
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
			case DecoderName:			decoders.back()->setName(buffer); break;
			case DecoderDescription:	decoders.back()->setDescription(buffer); break;
			case DecoderType:			decoders.back()->setType((EDecoderType::EDecoderType)atoi(buffer)); break;
			case DecoderCV29:			decoders.back()->setConfig(atoi(buffer)); break;
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
		stream.WriteStartElement( "DecodersConfig" );
		
		stream.WriteStartElement( "Decoders" );
		for (auto it = decoders.begin(); it != decoders.end(); it++)
		{
			Decoders *d = *it;
			stream.WriteStartElement( "Decoder" );
			stream.WriteElement( "Name", d->getName() );
			stream.WriteElement( "Description", d->getDescription() );
			stream.WriteElement( "Address", d->getAddress() );
			stream.WriteElement( "Type", d->getType() );
			stream.WriteElement( "CV29", d->getConfig() );
			stream.WriteEndElement( "Decoder" );
		}
		stream.WriteEndElement( "Decoders" );
		
		stream.WriteEndElement( "DecodersConfig" );
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
	
};

extern DecodersConfig uiDecodersConfig;
