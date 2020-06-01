#ifndef LISTITEM_HPP
#define LISTITEM_HPP

#include <gui_generated/containers/ListItemBase.hpp>

class ListItem : public ListItemBase
{
public:
    ListItem();
    virtual ~ListItem() {}

    virtual void initialize();
	
	void setText(const Unicode::UnicodeChar* value)
	{
		textArea1.setWildcard(value);
	}

protected:
	
};

#endif // LISTITEM_HPP
