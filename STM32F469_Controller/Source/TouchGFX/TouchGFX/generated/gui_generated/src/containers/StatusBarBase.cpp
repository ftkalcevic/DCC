/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/containers/StatusBarBase.hpp>
#include <touchgfx/Color.hpp>
#include <texts/TextKeysAndLanguages.hpp>

StatusBarBase::StatusBarBase()
{
    setWidth(800);
    setHeight(25);
    box1.setPosition(0, 0, 800, 25);
    box1.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));

    textAreaStatusBar.setXY(0, 2);
    textAreaStatusBar.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textAreaStatusBar.setLinespacing(0);
    textAreaStatusBar.setTypedText(touchgfx::TypedText(T_SINGLEUSEID13));

    add(box1);
    add(textAreaStatusBar);
}

void StatusBarBase::initialize()
{

}
