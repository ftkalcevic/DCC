#include <stdio.h>
#include <string.h>
#include "Decoders.h"
#include "BitmapDatabase.hpp"


EUserImageClass::SUserImageInfo EUserImageClass::UserImageInfo[] = 
{ 
	{ EUserImage::None,				{u"None"},			BITMAP_INVALID,				EDecoderType::None },
	{ EUserImage::UserFile,			{u"File"},			BITMAP_INVALID,				EDecoderType::Multifunction | EDecoderType::Accessory },
	{ EUserImage::LocoSteamIcon,	{u"Steam Loco"},	BITMAP_LOCOICON_ID,			EDecoderType::Multifunction },
	{ EUserImage::LocoDieselIcon,	{u"Diesel Loco"},	BITMAP_DIESELICON_SMALL_ID,	EDecoderType::Multifunction },
	{ EUserImage::LocoElectricIcon,	{u"Electric Loco"},	BITMAP_ELECTRICICON_SMALL_ID,	EDecoderType::Multifunction },
	{ EUserImage::AccTurnoutIcon,	{u"Turnout"},		BITMAP_ACCESSORYICON_ID,	EDecoderType::Accessory }
};


const char16_t *EUserImageClass::iconName( EUserImage::EUserImage img)
{
	for (int i = 0; i < countof(EUserImageClass::UserImageInfo); i++)
		if (img == UserImageInfo[i].id)
			return UserImageInfo[i].name;
	return u"";
}


uint16_t EUserImageClass::Count()
{
	return countof(UserImageInfo);
}

const EUserImageClass::SUserImageInfo &EUserImageClass::UserImage(uint16_t index)
{
	assert(index < Count());
	return UserImageInfo[index];
}


const EUserImageClass::SUserImageInfo &EUserImageClass::UserImage(EUserImage::EUserImage id)
{
	for (int i = 0; i < countof(EUserImageClass::UserImageInfo); i++)
		if (id == UserImageInfo[i].id)
			return UserImageInfo[i];
	assert(false);
	return UserImageInfo[0];
}

