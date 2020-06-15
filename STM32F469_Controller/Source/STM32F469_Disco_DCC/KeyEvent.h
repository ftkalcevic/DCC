#pragma once


namespace EKey
{
	enum EKey: uint16_t
	{
		None = 0,
		F1 = 1,
		F2 = 2,
		F3 = 3,
		F4 = 4,
		F5 = 5,
		FWD = 6,
		REV = 7,
		EStop = 8,
		KeyRelease = 0x8000
	};
}


struct KeyEvent
{
	EKey::EKey keyEvent;
	
	EKey::EKey key() const { return (EKey::EKey)(keyEvent & 0x4FFF); }
	bool isPress() const { return (keyEvent & EKey::KeyRelease) == 0; }
	bool isRelease() const { return (keyEvent & EKey::KeyRelease) != 0; }
};
