#pragma once
class DCC
{
public:
	void Run();
	bool ProcessBit(uint8_t bit, bool error = false);
	void ProcessPacket(uint8_t const * buffer, uint8_t len);	
};

