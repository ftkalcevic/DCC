#pragma once

#include "InputEvent.h"
#include <touchgfx/Unicode.hpp>
#include "Common.h"
#include <string.h>
#include "Utility.h"
#include <touchgfx/Bitmap.hpp>

using namespace touchgfx;

const int MAX_PATH = 64;

// Enums are assigned values as these are used in config file.
namespace EDecoderType
{
	enum EDecoderType: uint8_t
	{
		None=0,
		Multifunction=1,	// These area also used as a bit mask
		Accessory=2
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

namespace EUserImage
{
	enum EUserImage
	{
		None = 0,
		LocoSteamIcon=1,
		LocoDieselIcon=2,
		LocoElectricIcon=3,
		AccTurnoutIcon=4,
		UserFile=5,
	};
}

const uint8_t MASK_LOCO = 0x01;
const uint8_t MASK_ACC  = 0x02;

class EUserImageClass
{
	EUserImage::EUserImage id;
public:
	EUserImageClass(EUserImage::EUserImage ui)
		: id(ui) {}
	static struct SUserImageInfo
	{
		EUserImage::EUserImage id;
		const char16_t *name;
		BitmapId bmpId;
		uint8_t usage;
	} UserImageInfo[];
	static const char16_t *iconName(EUserImage::EUserImage img);
	static uint16_t Count();
	static const SUserImageInfo& UserImage(uint16_t index);
	static const SUserImageInfo &UserImage(EUserImage::EUserImage);
};

namespace EImageAttributes
{
	// Mask
	enum EImageAttributes: uint8_t
	{
		None = 0,
		FlipHorizontal = 0x01,
		FlipVertical = 0x02,
	};
}

struct DCCImage
{
private:
	EUserImage::EUserImage type;
	char file[MAX_PATH];
	EImageAttributes::EImageAttributes attributes;
	bool dirtyFlag;
	void dirty() { dirtyFlag = true; }
	
public:
	EUserImage::EUserImage getType() const { return type; }
	void setType(EUserImage::EUserImage id)
	{ 
		if (type != id) 
		{
			type = id;
			dirty();
		} 
	}
	
	char *getFile() { return file; }
	void setFile(const char *s)
	{ 
		if (strcmp(file, s) != 0)
		{
			strncpy(file, s, countof(file));
			file[countof(file)-1] = 0;
			dirty();
		}
	}
	
	EImageAttributes::EImageAttributes getAttributes() const { return attributes; }
	void setAttributes(EImageAttributes::EImageAttributes newAttr)
	{
		if (newAttr != attributes)
		{
			attributes = newAttr;
			dirty();
		}
	}
};

struct LocoSettings 
{
private:
	ESpeedSteps::ESpeedSteps speedSteps;
	DCCImage smallImage;
	DCCImage largeImage;
	
	EFrontLightPosition::EFrontLightPosition FL;
	EControlReleaseAction::EControlReleaseAction releaseAction;
	bool controlled;
	uint8_t currentSpeed;
	uint8_t currentBraking;
	EDirection::EDirection direction;
	
	bool dirtyFlag;
	void dirty() { dirtyFlag = true;}
	
public:
	void setSpeedSteps(ESpeedSteps::ESpeedSteps s) { if (speedSteps != s) { speedSteps = s; dirty(); } }
	ESpeedSteps::ESpeedSteps getSpeedSteps() const { return speedSteps; }
	DCCImage &getSmallImage() { return smallImage;}
	DCCImage &getLargeImage() { return largeImage;}
	bool getControlled() const { return controlled; }
	void setControlled(bool c) { controlled = c; }
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
	
const uint8_t MAX_ACCESSORY_DEVICE = 8;
const uint8_t MAX_ACCESSORY_DEVICE_STATES = 3;	

struct AccessoryDevice 
{
private:
	uint8_t numberOfStates;
	struct
	{
		DCCImage smallImage;
		DCCImage largeImage;
	} states[MAX_ACCESSORY_DEVICE_STATES];
	EDefaultAccessoryState::EDefaultAccessoryState defaultState;
	ESoundEffect::ESoundEffect soundEffect;
	char userSoundEffect[MAX_PATH];

	uint8_t currentState;	
public:
	uint8_t getCurrentState() const { return currentState; }
	void setCurrentState(uint8_t s) { if ( s > numberOfStates ) currentState = numberOfStates-1; else currentState = s; }
};

struct AccessorySettings
{
private:
	EAccessoryNetwork::EAccessoryNetwork network;
	EAccessoryType::EAccessoryType type;
	uint8_t numberOfDevices;
	AccessoryDevice devices[MAX_ACCESSORY_DEVICE];
public:
	AccessoryDevice &getDevice(uint8_t index) { assert(index < MAX_ACCESSORY_DEVICE); return devices[index]; }
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
			decoderDefFilename[countof(decoderDefFilename)-1] = 0;
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

