#pragma  once

#include "Config.h"
#include "Decoders.h"
#include <vector>
#include <vector>
#include <touchgfx/Unicode.hpp>

using namespace touchgfx;

class DecodersConfig: public Config<127, 80>	// longest element is a description string, xpath max is about 70
{
	enum Elements
	{
		None,
		Decoder,
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
		};
		return matchElement(element, e, countof(e));
	}
	virtual bool elementEnd() 
	{
		int n = atoi(buffer);
		switch (activeElement)
		{
			case Decoder: break;
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
	
//	int getThrottleMin() const { return throttleMin; }
//	
//	void setThrottleMin(int n) { if ( throttleMin != n ) {throttleMin = n; dirty();}}
	
	void write()
	{
		ConfigStream stream(getPath());
		stream.WriteStartElement( "DecodersConfig" );
		
		stream.WriteStartElement( "Decoders" );
		for (auto it = decoders.begin(); it != decoders.end(); it++)
		{
			Decoders *d = *it;
			stream.WriteStartElement( "Decoder" );
			stream.WriteElement( "Name", d->name );
			stream.WriteElement( "Description", d->description );
			stream.WriteElement( "Address", d->address );
			stream.WriteElement( "Type", d->type );
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
		Unicode::strncpy((Unicode::UnicodeChar *)decoder->name, (Unicode::UnicodeChar *)u"New Decoder", countof(decoder->name));
		decoder->description[0] = 0;
		decoder->address = 0;
		decoder->type = EDecoderType::Multifunction;
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
};

extern DecodersConfig uiDecodersConfig;
