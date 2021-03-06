/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/settings_screen/SettingsViewBase.hpp>
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>

SettingsViewBase::SettingsViewBase()
{

    functionButtons.setXY(0, 455);

    statusBar.setXY(0, 0);

    scrollableContainer1.setPosition(0, 25, 800, 430);
    scrollableContainer1.enableHorizontalScroll(false);
    scrollableContainer1.setScrollbarsColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));

    backgroundImage.setBitmap(touchgfx::Bitmap(BITMAP_DARK_TEXTURES_CARBON_FIBRE_ID));
    backgroundImage.setPosition(0, 0, 800, 641);
    backgroundImage.setOffset(0, 0);
    scrollableContainer1.add(backgroundImage);

    textThrottle.setXY(329, 11);
    textThrottle.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textThrottle.setLinespacing(0);
    textThrottle.setTypedText(touchgfx::TypedText(T_SINGLEUSEID30));
    scrollableContainer1.add(textThrottle);

    textBrake.setXY(349, 176);
    textBrake.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textBrake.setLinespacing(0);
    textBrake.setTypedText(touchgfx::TypedText(T_SINGLEUSEID35));
    scrollableContainer1.add(textBrake);

    checkBoxReverseThrottle.setXY(114, 111);
    scrollableContainer1.add(checkBoxReverseThrottle);

    checkBoxReverseBrake.setXY(114, 263);
    scrollableContainer1.add(checkBoxReverseBrake);

    checkBoxReverseDirection.setXY(114, 456);
    scrollableContainer1.add(checkBoxReverseDirection);

    textDirection.setXY(319, 345);
    textDirection.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textDirection.setLinespacing(0);
    textDirection.setTypedText(touchgfx::TypedText(T_SINGLEUSEID34));
    scrollableContainer1.add(textDirection);
    scrollableContainer1.setScrollbarsPermanentlyVisible();
    scrollableContainer1.setScrollbarsVisible(false);

    add(functionButtons);
    add(statusBar);
    add(scrollableContainer1);
}

void SettingsViewBase::setupScreen()
{
    functionButtons.initialize();
    statusBar.initialize();
    checkBoxReverseThrottle.initialize();
    checkBoxReverseBrake.initialize();
    checkBoxReverseDirection.initialize();
}
