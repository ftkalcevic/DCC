#ifndef COMBOITEM_HPP
#define COMBOITEM_HPP

#include <gui_generated/containers/ComboItemBase.hpp>

class ComboItem : public ComboItemBase
{
public:
    ComboItem();
    virtual ~ComboItem() {}

    virtual void initialize();
	
    void setText( const char16_t *t ) { text.setWildcard((const Unicode::UnicodeChar*)t); }
	
protected:
};

#endif // COMBOITEM_HPP
