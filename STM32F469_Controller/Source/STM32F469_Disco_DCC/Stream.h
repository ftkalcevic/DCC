#pragma once

class Stream
{
public:
	virtual uint8_t ReadByte()=0;
	virtual uint16_t ReadBytes(uint8_t *buffer, uint16_t len, uint16_t &bytesread)=0;
	virtual uint32_t GetSize()=0;
	virtual void Seek(uint32_t offset, uint8_t whence)=0;
};