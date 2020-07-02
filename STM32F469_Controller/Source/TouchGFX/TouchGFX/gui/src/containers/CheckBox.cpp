#include <gui/containers/CheckBox.hpp>
#include <stdio.h>


CheckBox::CheckBox() :
    CheckCallback(this, &CheckBox::checkHandler)
{
	check.setAction(CheckCallback);
	check.setDeselectedAction(CheckCallback);
	text.setTouchable(true);
}

void CheckBox::initialize()
{
    CheckBoxBase::initialize();
}


void CheckBox::handleClickEvent(const ClickEvent & event)
{
	if (event.getType() == ClickEvent::PRESSED)
		check.setSelected(!check.getSelected());
}
    
void CheckBox::checkHandler(const touchgfx::AbstractButton& src)
{
	//printf("CheckBox::check handler\n");
    if (action && action->isValid())
    {
        action->execute(*this);
    }
}

void CheckBox::getLastChild(int16_t x, int16_t y, Drawable** last)
{
	if (check.getRect().intersect(x, y))
		*last = &check;
	else
		*last = this;
}


void CheckBox::repositionText()
{
	// Keep text and icon aligned in middle of box
	Rect r = getRect();
	Rect c = check.getRect();
	Rect t = text.getRect();
	const Font *f = text.getTypedText().getFont();
    uint16_t fontHeight = f->getMinimumTextHeight();
	
	check.setPosition(c.x, (r.height-c.height)/2, c.width, c.height);
	text.setPosition(t.x, (r.height-fontHeight)/2, t.width, t.height);
	
}