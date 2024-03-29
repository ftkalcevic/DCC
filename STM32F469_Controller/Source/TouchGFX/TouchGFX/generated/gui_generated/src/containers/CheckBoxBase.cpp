/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/containers/CheckBoxBase.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>

CheckBoxBase::CheckBoxBase()
{
    setWidth(338);
    setHeight(65);
    check.setXY(11, 10);
    check.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_CHECK_BUTTONS_CHECK_MARK_INACTIVE_ID), touchgfx::Bitmap(BITMAP_BLUE_CHECK_BUTTONS_CHECK_MARK_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_CHECK_BUTTONS_CHECK_MARK_ACTIVE_ID), touchgfx::Bitmap(BITMAP_BLUE_CHECK_BUTTONS_CHECK_MARK_NORMAL_ID));
    check.setSelected(false);
    check.setDeselectionEnabled(true);

    text.setXY(70, 8);
    text.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    text.setLinespacing(0);
    text.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTLEFT40PXID));

    add(check);
    add(text);
}

void CheckBoxBase::initialize()
{

}
