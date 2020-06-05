#pragma  once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Common.h"
#include "FileSystem.h"

class Config
{
protected:
	Config()
	{
		dirtyFlag = false;
	}
	bool dirtyFlag;
	void dirty() { dirtyFlag = true;}
	virtual const char *getPath() const = 0;
	virtual bool elementStart(const char *element) { return true; }
	virtual bool elementContent(const char *data) {return true; }
	virtual bool elementEnd() {return true; }
	
public:	
	bool isDirty() const { return dirtyFlag;}
	void parse(); 
};



class ConfigStream
{
	const char *filePath;
	char tempPath[128];
	FIL file;
public:
	ConfigStream(const char *path);
	void WriteStartElement(const char *elem);
	void WriteElement(const char *elem, int value);
	void WriteEndElement(const char *elem);
	void Close();
};

class DCCConfig: public Config
{
	enum Elements
	{
		None,
		TripCurrent,
		Toff,
		SlewRate
	};
	int tripCurrent;
	int toff;
	int slewRate;
	Elements activeElement;
	char buffer[10];
	int bufferIndex;
protected:
	virtual const char *getPath() const { return "/config/DCCConfig.xml";}
	virtual bool elementStart(const char *element) 
	{
		if (strcmp(element, "TripCurrent") == 0)
			activeElement = TripCurrent;
		else if (strcmp(element, "Toff") == 0)
			activeElement = Toff;
		else if (strcmp(element, "SlewRate") == 0)
			activeElement = SlewRate;	
		else
			activeElement = None;
		bufferIndex = 0;
		return true;
	}
	virtual bool elementContent(const char *data) 
	{
		if (activeElement != None)
		{
			while ((*data) != '\0')
			{
				if (bufferIndex < (sizeof(buffer) / sizeof(buffer[0])) - 1)	
				{
					buffer[bufferIndex++] = *data++;
					buffer[bufferIndex] = 0;
				}
				else 
					break;
			}
		}
		return true;		
	}
	virtual bool elementEnd() 
	{
		int n = atoi(buffer);
		switch (activeElement)
		{
			case DCCConfig::TripCurrent: if ( n >= 0 && n <= 3 ) tripCurrent = n; break;
			case DCCConfig::Toff: if ( n >= 0 && n <= 3 ) toff = n; break;
			case DCCConfig::SlewRate: if ( n >= 0 && n <= 7 ) slewRate = n; break;
			default:
				break;
		}
		return true;
	}
	
public:
	DCCConfig()
	{
		tripCurrent=0;
		toff=0;
		slewRate=0;
	}
	
	int getTripCurrent() const { return tripCurrent;}
	void setTripCurrent(int n) {if (tripCurrent != n) { tripCurrent = n; dirty(); }}
	int getToff() const { return toff;}
	void setToff(int n) { if (toff != n) {toff = n; dirty(); }}
	int getSlewRate() const { return slewRate;}
	void setSlewRate(int n) {if (slewRate != n) { slewRate = n; dirty(); }}
	void parse()
	{
		activeElement = None;
		Config::parse();
		dirtyFlag = false;
	}
	void write()
	{
		ConfigStream stream(getPath());
		stream.WriteStartElement( "DCCSettings" );
		stream.WriteElement( "TripCurrent", tripCurrent );
		stream.WriteElement( "Toff", toff );
		stream.WriteElement( "SlewRate", slewRate );
		stream.WriteEndElement( "DCCSettings" );
		stream.Close();
	}
};

