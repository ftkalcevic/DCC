/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/placeholderscreen_screen/placeHolderScreenViewBase.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Color.hpp>

placeHolderScreenViewBase::placeHolderScreenViewBase()
{

    buttonReadAllCVs.setXY(35, 35);
    buttonReadAllCVs.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_MEDIUM_PRESSED_ID));
    buttonReadAllCVs.setLabelText(touchgfx::TypedText(T_RESOURCEREADALLCVS));
    buttonReadAllCVs.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    buttonReadAllCVs.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));

    slider1.setXY(35, 120);
    slider1.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_MEDIUM_SLIDER2_ROUND_EDGE_BACK_ID), touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_MEDIUM_SLIDER2_ROUND_EDGE_BACK_ID), touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_MEDIUM_INDICATORS_SLIDER2_ROUND_EDGE_NOB_ID));
    slider1.setupHorizontalSlider(2, 6, 0, 0, 284);
    slider1.setValueRange(0, 100);
    slider1.setValue(0);

    slider2.setXY(41, 211);
    slider2.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_SMALL_SLIDER_HORIZONTAL_SMALL_ROUND_EDGE_BACK_ID), touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_SMALL_SLIDER_HORIZONTAL_SMALL_ROUND_EDGE_BACK_ID), touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_SMALL_INDICATORS_SLIDER_HORIZONTAL_SMALL_ROUND_EDGE_KNOB_ID));
    slider2.setupHorizontalSlider(3, 7, 0, 0, 125);
    slider2.setValueRange(0, 100);
    slider2.setValue(0);

    radioButton2.setXY(35, 304);
    radioButton2.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID));
    radioButton2.setSelected(false);
    radioButton2.setDeselectionEnabled(false);

    button1.setXY(325, 35);
    button1.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID));

    sliderBrake.setXY(49, 386);
    sliderBrake.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_LARGE_SLIDER_ROUND_BACK_ID), touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_LARGE_SLIDER_ROUND_FILL_ID), touchgfx::Bitmap(BITMAP_BLUE_SLIDER_HORIZONTAL_LARGE_INDICATORS_SLIDER_ROUND_NOB_ID));
    sliderBrake.setupHorizontalSlider(2, 22, 0, 0, 621);
    sliderBrake.setValueRange(0, 100);
    sliderBrake.setValue(0);

    buttonRev.setXY(235, 334);
    buttonRev.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_ICONS_BACK_ARROW_32_ID), touchgfx::Bitmap(BITMAP_BLUE_ICONS_BACK_ARROW_32_ID));
    buttonRev.setIconXY(22, 15);

    buttonFwd.setXY(411, 334);
    buttonFwd.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_ICONS_NEXT_ARROW_32_ID), touchgfx::Bitmap(BITMAP_BLUE_ICONS_NEXT_ARROW_32_ID));
    buttonFwd.setIconXY(22, 15);

    buttonStop.setXY(321, 334);
    buttonStop.setBitmaps(touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_ID), touchgfx::Bitmap(BITMAP_BLUE_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), touchgfx::Bitmap(BITMAP_BLUE_ICONS_STOP_32_ID), touchgfx::Bitmap(BITMAP_BLUE_ICONS_STOP_32_ID));
    buttonStop.setIconXY(15, 15);

    add(buttonReadAllCVs);
    add(slider1);
    add(slider2);
    add(radioButton2);
    add(button1);
    add(sliderBrake);
    add(buttonRev);
    add(buttonFwd);
    add(buttonStop);
    radioButtonGroup1.add(radioButton2);
}

void placeHolderScreenViewBase::setupScreen()
{

}
