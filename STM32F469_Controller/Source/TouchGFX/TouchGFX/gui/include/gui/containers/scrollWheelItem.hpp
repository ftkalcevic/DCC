#ifndef SCROLLWHEELITEM_HPP
#define SCROLLWHEELITEM_HPP

#include <gui_generated/containers/scrollWheelItemBase.hpp>

class scrollWheelItem : public scrollWheelItemBase
{
public:
    scrollWheelItem();
    virtual ~scrollWheelItem() {}

    virtual void initialize();
	
    void setText(const char16_t *text)
    {
	    textArea.setWildcard((const Unicode::UnicodeChar *)text);
    }	
protected:
};

#endif // SCROLLWHEELITEM_HPP
