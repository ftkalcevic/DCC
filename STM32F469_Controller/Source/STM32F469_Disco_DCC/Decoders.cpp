#include <stdio.h>
#include <string.h>
#include "Decoders.h"


EUserImageClass::SUserImageInfo EUserImageClass::UserImageInfo[] = 
{ 
	{ EUserImage::None,				{u"None"},			0 },
	{ EUserImage::UserFile,			{u"File"},			MASK_LOCO | MASK_ACC },
	{ EUserImage::LocoSteamIcon,	{u"Steam Loco"},	MASK_LOCO },
	{ EUserImage::LocoDieselIcon,	{u"Diesel Loco"},	MASK_LOCO },
	{ EUserImage::AccTurnoutIcon,	{u"Turnout"},		MASK_ACC }
};


const char16_t *EUserImageClass::iconName( EUserImage::EUserImage img)
{
	for (int i = 0; i < countof(EUserImageClass::UserImageInfo); i++)
		if (img == UserImageInfo[i].id)
			return UserImageInfo[i].name;
	return u"";
}
