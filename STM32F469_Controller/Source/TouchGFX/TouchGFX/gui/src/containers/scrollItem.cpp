#include <gui/containers/scrollItem.hpp>

scrollItem::scrollItem()
{

}

void scrollItem::initialize()
{
    scrollItemBase::initialize();
}


void scrollItem::setText(const char16_t *text)
{
	textArea.setWildcard((const Unicode::UnicodeChar *)text);
}