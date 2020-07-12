#pragma once

#include "InputEvent.h"
#include <touchgfx/Unicode.hpp>
#include "Common.h"
#include <string.h>


using namespace touchgfx;

const int MAX_PATH = 64;

// Enums are assigned values as these are used in config file.
namespace EDecoderType
{
	enum EDecoderType: uint8_t
	{
		Multifunction=0,
		Accessory=1
	};
}

namespace ESpeedSteps
{
	enum ESpeedSteps: uint8_t
	{
		ss14=0,
		ss28=1,
		ss128=2
	};
}


namespace EFrontLightPosition
{
	enum EFrontLightPosition: uint8_t
	{
		SpeedDirInsBit4=0,		
		FuncGroupOneBit4=1
	};
}

namespace EControlReleaseAction
{
	enum EControlReleaseAction: uint8_t
	{
		Nothing=0,
		Stop=1,
		EStop=2
	};
}

struct LocoSettings 
{
	ESpeedSteps::ESpeedSteps speedSteps;
	EFrontLightPosition::EFrontLightPosition FL;
	EControlReleaseAction::EControlReleaseAction releaseAction;
	bool controlled;
	uint8_t currentSpeed;
	uint8_t currentBraking;
	EDirection::EDirection direction;

	void setSpeedSteps(ESpeedSteps::ESpeedSteps s) { if (speedSteps != s) { speedSteps = s; dirty(); } }
	ESpeedSteps::ESpeedSteps getSpeedSteps() const { return speedSteps; }
	
	bool dirtyFlag;
	void dirty() { dirtyFlag = true;}
};

namespace EAccessoryNetwork
{
	enum EAccessoryNetwork: uint8_t
	{
		DCC = 0,
		LCC = 1
	};
}

namespace EAccessoryType
{
	enum EAccessoryType: uint8_t
	{
		Turnout = 0,
		ThreeWay = 1,
		Turntable = 2,
		Decoupler = 3,
		OnOffswitch = 4
	};
}

namespace EAccessoryImage
{
	enum EAccessoryImage : uint8_t
	{
		StraightTurnout = 0,
		CurvedTurnout = 1,
		Wye = 2,
		ThreeWay = 3,
		Turntable = 4,
		Decoupler = 5,
		OnOffSwitch = 6,
		UserDefined = 255
	};
}
	
namespace EDefaultAccessoryState
{
	enum EDefaultAccessoryState: uint8_t
	{
		Unknown = 0,
		AssumeNormal = 1,
		AssumeReverse = 2,
		MoveToNormal = 3,
		MoveToReverse = 4,
		QueryLCC = 5
	};
}

namespace ESoundEffect
{
	enum ESoundEffect: uint8_t
	{
		Beep = 0,
		Bell = 1,
		UserDefined = 255
	};
}
	
	
struct AccessorySettings
{
	EAccessoryNetwork::EAccessoryNetwork network;
	EAccessoryType::EAccessoryType type;
	EAccessoryImage::EAccessoryImage image;
	char userImage[MAX_PATH];
	EDefaultAccessoryState::EDefaultAccessoryState defaultState;
	uint8_t numberOfStates;	// How to do the turntable?
	ESoundEffect::ESoundEffect soundEffect;
	char userSoundEffect[MAX_PATH];

	uint8_t currentState;
};

namespace EFunctionAction
{
	enum EFunctionAction: uint8_t
	{
		None = 0,
		FrontLight = 1,
		ToggleSwitch = 2,
		// lua script?
	};
}

struct Functions
{
	EFunctionAction::EFunctionAction Fn1, Fn2, Fn3, Fn4, Fn5;
};

struct Decoders
{
private:
	char16_t name[20];
	char16_t description[100];
	uint16_t address;	// How do we address LCC devices?
	uint8_t config;	// cv29
	char decoderDefFilename[20];	// model file
	EDecoderType::EDecoderType type;
	char UserIconFile[MAX_PATH];
	char UserBackgroundFile[MAX_PATH];
	union
	{
		LocoSettings loco;
		AccessorySettings accessory;
	};

	Functions func;
protected:
	bool dirty;
public:
	const char16_t *getName() const { return name;}
	const uint16_t getNameMaxLen() const { return countof(name);}
	const char16_t *getDescription() const { return description;}
	const uint16_t getDescriptionMaxLen() const { return countof(description);}
	const uint8_t getConfig() const { return config;}
	uint16_t getAddress() const { return address; }
	const char *getDecoderDefFilename() const { return decoderDefFilename;}
	EDecoderType::EDecoderType getType() const { return type; }
	LocoSettings &getLoco()
	{
		assert(type == EDecoderType::Multifunction && "Can only getLoco if decoder type is multifunction");
		return loco;
	}
	AccessorySettings &getAcc()
	{
		assert(type == EDecoderType::Accessory && "Can only getAcc if decoder type is accessory");
		return accessory;
	}
	void setName(const char16_t *newName) 
	{ 
		if (Unicode::strncmp((const Unicode::UnicodeChar *)name, (const Unicode::UnicodeChar *)newName, countof(name)) != 0) 
		{ 
			Unicode::strncpy((Unicode::UnicodeChar *)name, (const Unicode::UnicodeChar *)newName, countof(name));
			dirty = true;
		}
	}
	
	void utf8ToChar16(char16_t *dest, const char *src, int destlen)
	{
		int i = 0;
		for (i = 0; i < destlen; i++)		
		{
			uint8_t c = (uint8_t)*src++;
			if (c <= 0x007F)
			{
				dest[i] = c;
			}
			else 
			{
				uint32_t u = '?';
				if ((c & 0b11100000) == 0b11000000)
				{
					uint8_t c2 = (uint8_t)*src++;
					if (c2 != 0)
						u = ((c & 0x1F) << 6) | (c2 & 0x3F);
					else
						break;
				}
				else if ((c & 0b11110000) == 0b11100000)
				{
					uint8_t c2 = (uint8_t)*src++;
					if (c2 != 0)
					{
						uint8_t c3 = (uint8_t)*src++;
						if (c3 != 0)
						{
							if ((c2 & 0b11000000) == 0b10000000 && (c3 & 0b11000000) == 0b10000000)
							{
								u = ((c & 0xF) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
							}
						}
						else
							break;
					}
					else
						break;
				} 
				else if ((c & 0b11111000) == 0b11110000)
				{
					uint8_t c2 = (uint8_t)*src++;
					if (c2 != 0)
					{
						uint8_t c3 = (uint8_t)*src++;
						if (c3 != 0)
						{
							uint8_t c4 = (uint8_t)*src++;
							if (c4 != 0)
							{
								if ((c2 & 0b11000000) == 0b10000000 && (c3 & 0b11000000) == 0b10000000 && (c4 & 0b11000000) == 0b10000000)
								{
									u = ((c & 0xF) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F);
								}
							}
							else
								break;
						}
						else
							break;
					}
					else
						break;
				}
				if (u > 0 && u <= 0xFFFF)
					dest[i] = u;
				else
					dest[i] = '?';
			}	
		}
		if ( i == destlen)
			dest[destlen-1] = 0;
		else
			dest[i] = 0;
	}
	void setName( const char *newName )
	{
		utf8ToChar16(name, newName, countof(name));
		dirty = true;
	}
	void setDescription( const char16_t *newDescription )
	{ 
		if (Unicode::strncmp((const Unicode::UnicodeChar *)description, (const Unicode::UnicodeChar *)newDescription, countof(description)) != 0) 
		{ 
			Unicode::strncpy((Unicode::UnicodeChar *)description, (const Unicode::UnicodeChar *)newDescription, countof(description));
			dirty = true;
		}
	}
	void setDescription( const char *newDescription )
	{
		utf8ToChar16(description, newDescription, countof(description));
		dirty = true;
	}
	
	void setAddress( uint16_t newAddress ) 
	{ 
		if (address != newAddress)
		{
			address = newAddress;
			dirty = true;
		}
	}
	void setType( EDecoderType::EDecoderType newType ) 
	{ 
		if (type != newType)
		{
			type = newType;
			dirty = true;
		}
	}
	void setDecoderDefFilename( const char *newDef )
	{
		if (strcmp(decoderDefFilename, newDef) != 0)
		{
			strncpy(decoderDefFilename, newDef, countof(decoderDefFilename));
			decoderDefFilename[countof(decoderDefFilename)] = 0;
			dirty = true;
		}
	}
	
	void setConfig(uint8_t newConfig) 
	{ 
		if (newConfig != config)
		{
			config = newConfig;
			dirty = true;
		}
	}
};

