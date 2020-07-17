#pragma once

#include "Decoders.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace EDecoderDataType
{
	enum EDecoderDataType
	{
		None,
		Enum,
		Bitfield,
		Address,
		SpeedTable,
		Int,
		User
	};
}

class Group
{
public:
	Group()
	{
		cvs.reserve(64);
	}
	std::vector<uint16_t> cvs;
	std::u16string name;
};

class Enum
{
public:
	Enum()
	{
		value = 0;
	}
	uint8_t value;
	std::u16string name;
};

class Bit
{
public:
	Bit()
	{
		bit = 0;
	}
	uint8_t bit;
	std::u16string name;
	std::u16string on;
	std::u16string off;
};

class BitField
{
public:
	BitField()
	{
		type = EDecoderDataType::None;
		bitmask = 0;
		min = 0;
		max = 255;
		activeEnum = nullptr;
		activeBit = nullptr;
	}
	~BitField()
	{
		//for (auto it = enums.begin(); it != enums.end(); it++)
		//	delete *it;
		//for (auto it = bits.begin(); it != bits.end(); it++)
		//	delete *it;
	}
	
	std::u16string name;
	EDecoderDataType::EDecoderDataType type;
	uint8_t bitmask;
	uint8_t min;
	uint8_t max;
	
	std::vector<std::shared_ptr<Enum>> enums;
	std::vector< std::shared_ptr<Bit>> bits;

	std::shared_ptr<Enum> activeEnum;
	std::shared_ptr<Bit> activeBit;
	
	std::shared_ptr<Enum> getEnum()
	{
		if (activeEnum == nullptr)
			activeEnum = std::make_shared<Enum>();
		return activeEnum;
	}
	
	void enumEnd()
	{
		if (activeEnum != nullptr)
		{
			enums.push_back( activeEnum );
			activeEnum = nullptr;
		}
	}
	std::shared_ptr<Bit> getBit()
	{
		if (activeBit == nullptr)
			activeBit = std::make_shared<Bit>();
		return activeBit;
	}
	
	void bitEnd()
	{
		if (activeBit != nullptr)
		{
			bits.push_back( activeBit );
			activeBit = nullptr;
		}
	}

};

class CVDef
{
public:
	CVDef()
	{
		number = 0;
		type = EDecoderDataType::None;
		min = 0;
		max = 255;
		readonly = false;
		cvcount = 1;
	}
	uint16_t number;
	std::u16string name;
	EDecoderDataType::EDecoderDataType type;
	std::string userType;
	uint8_t min;
	uint8_t max;
	bool readonly;
	uint8_t cvcount;
};

class UserType
{
public:
	UserType()
	{
		type = EDecoderDataType::None;
		activeBitField = nullptr;
	}
	~UserType()
	{
		//for (auto it = bitFields.begin(); it != bitFields.end(); it++)
		//	delete *it;
	}
	std::string name;
	EDecoderDataType::EDecoderDataType type;
	std::vector<std::shared_ptr<BitField>> bitFields;
	std::shared_ptr<BitField> activeBitField;
	
	std::shared_ptr<BitField> getBitField()
	{
		if (activeBitField == nullptr)
			activeBitField = std::make_shared<BitField>();
		return activeBitField;
	}
	
	void bitFieldEnd()
	{
		if (activeBitField != nullptr)
		{
			bitFields.push_back( activeBitField );
			activeBitField = nullptr;
		}
	}
	
	bool hasLabeledTypes() const
	{
		// return true if any bit type has a label (on/off type), or more than one enum
		int enumTypes = 0;
		for(auto it = bitFields.cbegin() ; it != bitFields.cend() ; it++)
		{
			BitField &bf = **it;
			if (bf.type == EDecoderDataType::Int)
				return true;
			
			if (bf.enums.size() > 0)
				enumTypes++;
			if (enumTypes > 1)
				return true;
			
			for (auto itB = bf.bits.cbegin(); itB != bf.bits.cend(); itB++)
			{
				Bit &b = **itB;
				if (b.on.size() > 0 && b.off.size() > 0)
					return true;
			}
		}
		return false;
	}

};

class DecoderDef
{
public:
	DecoderDef()
	{
		type = EDecoderType::None;
		activeType = nullptr;
		activeCV = nullptr;
	}
	~DecoderDef()
	{
		//for (auto it = types.begin(); it != types.end(); it++)
		//	delete it->second;
		//for (auto it = CVs.begin(); it != CVs.end(); it++)
		//	delete it->second;
	}
	
	std::u16string manufacturer;
	EDecoderType::EDecoderType type;
	std::map<std::string, std::shared_ptr<UserType>> types;
	std::map<uint16_t, std::shared_ptr<CVDef>> CVs;
	std::vector<std::shared_ptr<Group>> groups;
	
	std::shared_ptr<UserType> activeType;
	std::shared_ptr<CVDef> activeCV;
	std::shared_ptr<Group> activeGroup;
	
	std::shared_ptr<UserType> getType()
	{
		if (activeType == nullptr)
		{
			activeType = std::make_shared<UserType>();
			if (activeCV != nullptr)
			{
				// If this is a CV, create a new custom user type
				char buf[12];
				activeType->name = "\xA7""CV";	// This can't be typed in.
				activeType->name += itoa(activeCV->number, buf, 10);
				activeType->type = activeCV->type;
				types[activeType->name] = activeType;	// We don't get an "endType" on a CV, so we add it to the types list now - normally we postpone adding because we don't have the name yet.
				activeCV->type = EDecoderDataType::User;
				activeCV->userType = activeType->name;
			}
		}
		return activeType;
	}
	
	void typeEnd()
	{
		if (activeType != nullptr)
		{
			activeType->bitFieldEnd();
			types[activeType->name] = activeType;
			activeType = nullptr;
		}
	}
	
	std::shared_ptr<CVDef> getCV()
	{
		if (activeCV == nullptr)
			activeCV = std::make_shared<CVDef>();
		return activeCV;
	}
	
	void cvEnd()
	{
		if (activeCV != nullptr)
		{
			CVs[activeCV->number] = activeCV;
			activeCV = nullptr;
		}
		if (activeType != nullptr)
		{
			activeType->bitFieldEnd();
			activeType = nullptr;
		}
	}
	
	std::shared_ptr<Group> getGroup()
	{
		if (activeGroup == nullptr)
			activeGroup = std::make_shared<Group>();
		return activeGroup;
	}
	
	void groupEnd()
	{
		if (activeGroup != nullptr)
		{
			groups.push_back( activeGroup );
			activeGroup = nullptr;
		}
	}
	
	static EDecoderType::EDecoderType parseDecoderType(const char *str)
	{
		if (strcasecmp(str, "Multifunction") == 0)
			return EDecoderType::Multifunction;
		else if (strcasecmp(str, "Accessory") == 0)
			return EDecoderType::Accessory;
		else
			return EDecoderType::None;
	}
	
	static EDecoderDataType::EDecoderDataType parseDataType(const char *str)
	{
		if (strcasecmp(str, "enum") == 0)
			return EDecoderDataType::Enum;
		else if (strcasecmp(str, "bitfield") == 0)
			return EDecoderDataType::Bitfield;
		else if (strcasecmp(str, "address") == 0)
			return EDecoderDataType::Address;
		else if (strcasecmp(str, "speedTable") == 0)
			return EDecoderDataType::SpeedTable;
		else if (strcasecmp(str, "int") == 0)
			return EDecoderDataType::Int;
		else
			return EDecoderDataType::None;
	}
	
	void parseCVDataType(const char *str, std::shared_ptr<CVDef> cv)
	{
		cv->type = parseDataType(str);
		if (cv->type == EDecoderDataType::None)
		{
			if (types.find(str) != types.end())
			{
				cv->type = EDecoderDataType::User;
				cv->userType = str;
			}
		}
	}

	static bool parseBool(const char *str)
	{
		if (strcasecmp(str, "true") == 0)
			return true;
		else if (strcasecmp(str, "1") == 0)
			return true;
		else if (strcasecmp(str, "false") == 0)
			return true;
		else if (strcasecmp(str, "0") == 0)
			return true;
		else
			return false;
	}
	
	void parseCVList(const char *str, std::shared_ptr<Group> group )
	{
		uint16_t cv=0;
		for(;;)
		{
			if (*str >= '0' && *str <= '9')
				cv = (cv * 10) + (*str - '0');
			else if ( cv > 0 )	// comma separated list, but we'll take anything non-numeric to terminate a number
			{
				group->cvs.push_back(cv);
				cv = 0;
			}
			if (!*str)	// string terminator - stop
				break;
			str++;
		}
	}
};





/*



Decoder
	- u16string		manufacturer
	- EDecoderType type
	- map<u16string,UserType> userTypes
		- UserType
			- ECVDataType	- bitfield, 
			- vector<field>
				- field
					- EType - Bitfield/Bit
						- BitField
							- uint8_t mask
							- u16string name
							- EType int/enum
								- int
									- min
									- max
								- enum
									- map<int,u16string> validValues
						- Bit
							- u16string name
							- uint8_t bit (0-7)
							- u16string onDesc
							- u16string offDesc
	- map<uint16_t,CV> 
		-CV
			- uint16_t cv
			- u16string name
			- EType type enum, bitfield, address, speedtable,int, user
			- u16string userType
			- uint8_t min, max
			- bool readonly
			- uint16_t cvcount
			- if bitfield/enum
				- recurse, or add as a new UserType.
			(how to handle range?)
	- vector<Group>
		- Group
			- name
			- vector<int> cvs
		
 */