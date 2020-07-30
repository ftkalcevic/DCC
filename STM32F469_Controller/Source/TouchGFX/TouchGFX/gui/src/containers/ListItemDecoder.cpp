#include <gui/containers/ListItemDecoder.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>
#include "DecodersConfig.h"
#include "BitmapDatabase.hpp"

ListItemDecoder::ListItemDecoder()
{
	index = -1;
}

void ListItemDecoder::initialize()
{
    ListItemDecoderBase::initialize();
}

void ListItemDecoder::setIndex(int i, bool selected)
{ 
	index = i;
	
	boxBackground.setBorderSize(selected ? 6 : 0);

	if (index < uiDecodersConfig.Count())
	{
		Decoders &d = uiDecodersConfig[index];
		textArea.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));
		textArea.setWildcard((const Unicode::UnicodeChar *)d.getName());
		if (d.getType() == EDecoderType::Multifunction)
			image.setBitmap(Bitmap(BITMAP_MULTIFUNCTIONACCESSORYICON_ID));
		else
			image.setBitmap(Bitmap(BITMAP_ACCESSORYDECODERICON_ID));
	}
	else
	{
		textArea.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTCENTERED40PXID));
		textArea.setWildcard((const Unicode::UnicodeChar *)u"<New>");
	}
}
