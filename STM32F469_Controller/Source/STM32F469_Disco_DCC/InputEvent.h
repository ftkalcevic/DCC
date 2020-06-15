#pragma once

namespace EDirection
{
	enum EDirection: uint8_t
	{
		Stopped,
		Forward,
		Reverse
	};
}


struct InputEvent
{
	int throttle;
	int throttleRaw;
	int brake;
	int brakeRaw;
	EDirection::EDirection direction;
};
