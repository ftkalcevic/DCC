#ifndef SCROLLWHEELTEXT_HPP
#define SCROLLWHEELTEXT_HPP

#include <gui_generated/containers/ScrollWheelTextBase.hpp>

class ScrollWheelText : public ScrollWheelTextBase
{
public:
    ScrollWheelText();
    virtual ~ScrollWheelText() {}

    virtual void initialize();
	
	void setText(const char16_t *text)
	{
		textDisplay.setWildcard((const Unicode::UnicodeChar *)text);
	}
	void setText(const Unicode::UnicodeChar *text)
	{
		textDisplay.setWildcard(text);
	}
protected:
};

#endif // SCROLLWHEELTEXT_HPP
