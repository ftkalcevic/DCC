/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/containers/FunctionButtonsBase.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>

FunctionButtonsBase::FunctionButtonsBase()
{
    setWidth(800);
    setHeight(25);
    image1.setXY(0, 0);
    image1.setBitmap(touchgfx::Bitmap(BITMAP_FNBACKGROUND_ID));

    image2.setXY(160, 0);
    image2.setBitmap(touchgfx::Bitmap(BITMAP_FNBACKGROUND_ID));

    image3.setXY(320, 0);
    image3.setBitmap(touchgfx::Bitmap(BITMAP_FNBACKGROUND_ID));

    image4.setXY(480, 0);
    image4.setBitmap(touchgfx::Bitmap(BITMAP_FNBACKGROUND_ID));

    image5.setXY(640, 0);
    image5.setBitmap(touchgfx::Bitmap(BITMAP_FNBACKGROUND_ID));

    textArea1.setPosition(0, 0, 160, 25);
    textArea1.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textArea1.setLinespacing(0);
    textArea1.setWildcard(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID).getText());
    textArea1.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTCENTER20PXID));

    textArea2.setPosition(160, 0, 160, 25);
    textArea2.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textArea2.setLinespacing(0);
    textArea2.setWildcard(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID).getText());
    textArea2.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTCENTER20PXID));

    textArea3.setPosition(320, 0, 160, 25);
    textArea3.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textArea3.setLinespacing(0);
    textArea3.setWildcard(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID).getText());
    textArea3.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTCENTER20PXID));

    textArea4.setPosition(480, 0, 160, 25);
    textArea4.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textArea4.setLinespacing(0);
    textArea4.setWildcard(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID).getText());
    textArea4.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTCENTER20PXID));

    textArea5.setPosition(640, 0, 160, 25);
    textArea5.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textArea5.setLinespacing(0);
    textArea5.setWildcard(touchgfx::TypedText(T_WILDCARDTEXTLEFT20PXID).getText());
    textArea5.setTypedText(touchgfx::TypedText(T_WILDCARDTEXTCENTER20PXID));

    add(image1);
    add(image2);
    add(image3);
    add(image4);
    add(image5);
    add(textArea1);
    add(textArea2);
    add(textArea3);
    add(textArea4);
    add(textArea5);
}

void FunctionButtonsBase::initialize()
{

}
