#include <gui/containers/ListItemDecoder.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include "DecodersConfig.h"
#include <touchgfx/Color.hpp>

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
		textArea.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID));
		textArea.setWildcard((const Unicode::UnicodeChar *)uiDecodersConfig[index].getName());
		
	}
	else
	{
		textArea.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTCENTERED40PXID));
		textArea.setWildcard((const Unicode::UnicodeChar *)u"<New>");
	}
}
