#pragma  once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Common.h"
#include "FileSystem.h"
#include "yxml.h"
#include <assert.h>


template<int MAX_ELEMENT_LENGTH, int READ_BUFFER_SIZE=50>
class Config
{
protected:
	struct ElementNames
	{
		const char *elementName;
		int id;
	};
	
	Config()
	{
		dirtyFlag = false;
	}
	bool dirtyFlag;
	int activeElement;
	char buffer[MAX_ELEMENT_LENGTH];
	int bufferIndex;
	
	void dirty() { dirtyFlag = true;}
	virtual const char *getPath() const = 0;
	virtual bool elementStart(const char *element) { return true; }
	virtual bool elementContent(const char *data) 
	{
		if (activeElement != 0)
		{
			while ((*data) != '\0')
			{
				if (bufferIndex < countof(buffer) - 1)	
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
	virtual bool elementEnd() {return true; }
	bool matchElement(const char * element, ElementNames *e, int elements)
	{
		activeElement = 0;
		for ( int i = 0; i < countof(e); i++ )
			if (strcmp(element, e[i].elementName ) == 0)
				activeElement = e[i].id;
		bufferIndex = 0;
		return true;
	}
	
public:	
	bool isDirty() const { return dirtyFlag;}
	void parse()
	{
		activeElement = 0;
	
		uint8_t buffer[READ_BUFFER_SIZE];
		yxml_t xml;
	
		yxml_init(&xml, buffer, READ_BUFFER_SIZE);
	
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
	
		dirtyFlag = false;
	}

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



class DCCConfig: public Config<10>	// longest element is an int 
{
	enum Elements
	{
		None=0,
		TripCurrent,
		Toff,
		SlewRate
	};
	int tripCurrent;
	int toff;
	int slewRate;
protected:
	
	virtual const char *getPath() const { return "/config/DCCConfig.xml";}
	virtual bool elementStart(const char *element) 
	{
		static ElementNames e[] = 
		{ 
			{ "TripCurrent", TripCurrent },
			{ "Toff", Toff },
			{ "SlewRate", SlewRate },
		};
		return matchElement(element, e, countof(e));
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
		toff=1;
		slewRate=6;
	}
	
	int getTripCurrent() const { return tripCurrent;}
	void setTripCurrent(int n) {if (tripCurrent != n) { tripCurrent = n; dirty(); }}
	int getToff() const { return toff;}
	void setToff(int n) { if (toff != n) {toff = n; dirty(); }}
	int getSlewRate() const { return slewRate;}
	void setSlewRate(int n) {if (slewRate != n) { slewRate = n; dirty(); }}
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



class UIConfig: public Config<10>	// longest element is an int 
{
	enum Elements
	{
		None,
		ThrottleMin,
		ThrottleMax,
		ThrottleReverse,
		BrakeMin,
		BrakeMax,
		BrakeReverse,
		DirectionReverse
	};

	int throttleMin;
	int throttleMax;
	bool throttleReverse;
	int brakeMin;
	int brakeMax;
	bool brakeReverse;
	bool directionReverse;
protected:
	virtual const char *getPath() const { return "/config/UIConfig.xml";}
	virtual bool elementStart(const char *element) 
	{
		static ElementNames e[] = 
		{ 
			{ "ThrottleMin", ThrottleMin },
			{ "ThrottleMax", ThrottleMax },
			{ "ThrottleReverse", ThrottleReverse },
			{ "BrakeMin", BrakeMin },
			{ "BrakeMax", BrakeMax },
			{ "BrakeReverse", BrakeReverse },
			{ "DirectionReverse", DirectionReverse }
		};
		return matchElement(element, e, countof(e));
	}
	virtual bool elementEnd() 
	{
		int n = atoi(buffer);
		switch (activeElement)
		{
			case ThrottleMin: if (n >= 0 && n <= 4095) throttleMin = n; break;
			case ThrottleMax: if (n >= 0 && n <= 4095) throttleMax = n; break;
			case ThrottleReverse: throttleReverse = n == 0 ? false : true; break;
			case BrakeMin: if (n >= 0 && n <= 4095) brakeMin = n; break;
			case BrakeMax: if (n >= 0 && n <= 4095) brakeMax = n; break;
			case BrakeReverse: brakeReverse = n == 0 ? false : true; break;
			case DirectionReverse: directionReverse = n == 0 ? false : true; break;
			default:
				break;
		}
		return true;
	}
	
public:
	UIConfig()
	{
		throttleMin = 1773;
		throttleMax = 2551;
		throttleReverse = 0;
		brakeMin = 1664;
		brakeMax = 2477;
		brakeReverse = 0;
		directionReverse = 1;
	}
	
	int getThrottleMin() const { return throttleMin; }
	int getThrottleMax() const { return throttleMax; }
	bool getThrottleReverse() const { return throttleReverse; }
	int getBrakeMin() const { return brakeMin; }
	int getBrakeMax() const { return brakeMax; }
	bool getBrakeReverse() const { return brakeReverse; }
	bool getDirectionReverse() const { return directionReverse; }
	void setThrottleMin(int n) { if ( throttleMin != n ) {throttleMin = n; dirty();}}
	void setThrottleMax(int n) { if ( throttleMax != n ) {throttleMax = n; dirty();}}
	void setThrottleReverse(bool n) { if ( throttleReverse != n ) {throttleReverse = n; dirty();}}
	void setBrakeMin(int n) { if ( brakeMin != n ) {brakeMin = n; dirty();}}
	void setBrakeMax(int n) { if ( brakeMax != n ) {brakeMax = n; dirty();}}
	void setBrakeReverse(bool n) { if ( brakeReverse != n ) {brakeReverse = n; dirty();}}
	void setDirectionReverse(bool n) { if ( directionReverse != n ) {directionReverse = n; dirty();}}
	
	void write()
	{
		ConfigStream stream(getPath());
		stream.WriteStartElement( "UISettings" );
		stream.WriteElement( "ThrottleMin", throttleMin);
		stream.WriteElement( "ThrottleMax", throttleMax);
		stream.WriteElement( "ThrottleReverse", throttleReverse ? 1 : 0);
		stream.WriteElement( "BrakeMin", brakeMin);
		stream.WriteElement( "BrakeMax", brakeMax);
		stream.WriteElement( "BrakeReverse", brakeReverse ? 1 : 0);
		stream.WriteElement( "DirectionReverse", directionReverse ? 1 : 0);
		stream.WriteEndElement( "UISettings" );
		stream.Close();
	}
};

extern UIConfig uiConfig;
