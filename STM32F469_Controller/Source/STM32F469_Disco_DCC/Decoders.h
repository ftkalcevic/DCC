#pragma once

#include "InputEvent.h"

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
	char16_t name[20];
	char16_t description[100];
	uint16_t address;	// How do we address LCC devices
	// model file
	EDecoderType::EDecoderType type;
	char UserIconFile[MAX_PATH];
	char UserBackgroundFile[MAX_PATH];
	union
	{
		LocoSettings loco;
		AccessorySettings accessory;
	};

	Functions func;
};



extern int decoderCount;
extern Decoders decoders[];
extern int activeDecoder;

