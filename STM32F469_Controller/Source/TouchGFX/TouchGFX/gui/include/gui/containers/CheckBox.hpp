#ifndef CHECKBOX_HPP
#define CHECKBOX_HPP

#include <gui_generated/containers/CheckBoxBase.hpp>

class CheckBox : public CheckBoxBase
{
public:
    CheckBox();
    virtual ~CheckBox() {}

    virtual void initialize();
	
	void setText(const Unicode::UnicodeChar *txt)
	{
		text.setWildcard(txt);
	}
	void setText(const char16_t *txt)
	{
		this->setText((const Unicode::UnicodeChar *)txt);
	}
	
	virtual void handleClickEvent(const ClickEvent & event);
    virtual void getLastChild(int16_t x, int16_t y, Drawable** last);
    void setAction(GenericCallback< const CheckBox& >& callback)
    {
        action = &callback;
    }
	void setSelected(bool selected) { check.setSelected(selected);}
	bool getSelected() const { return check.getSelected();}
	virtual void setTouchable(bool touch)
	{
		check.setTouchable(touch);
		text.setTouchable(touch);
	}
	
protected:
    GenericCallback< const CheckBox& >* action; ///< The callback to be executed when this AbstractButton is clicked
	
    touchgfx::Callback<CheckBox, const touchgfx::AbstractButton&> CheckCallback;
    void checkHandler(const touchgfx::AbstractButton& src);
	
};

#endif // CHECKBOX_HPP
