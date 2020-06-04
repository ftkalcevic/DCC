/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/main_screen/MainViewBase.hpp>
#include "BitmapDatabase.hpp"

MainViewBase::MainViewBase()
{

    backgroundImage.setBitmap(touchgfx::Bitmap(BITMAP_DARK_TEXTURES_CARBON_FIBRE_ID));
    backgroundImage.setPosition(0, 0, 800, 480);
    backgroundImage.setOffset(0, 0);

    swipeContainer.setXY(0, 25);
    swipeContainer.setPageIndicatorBitmaps(touchgfx::Bitmap(BITMAP_BLUE_PAGEINDICATOR_DOT_INDICATOR_SMALL_NORMAL_ID), touchgfx::Bitmap(BITMAP_BLUE_PAGEINDICATOR_DOT_INDICATOR_SMALL_HIGHLIGHT_ID));
    swipeContainer.setPageIndicatorXY(400, 404);
    swipeContainer.setSwipeCutoff(50);
    swipeContainer.setEndSwipeElasticWidth(50);

    functionButtons.setXY(0, 455);

    statusBar.setXY(0, 0);

    add(backgroundImage);
    add(swipeContainer);
    add(functionButtons);
    add(statusBar);
}

void MainViewBase::setupScreen()
{
    functionButtons.initialize();
    statusBar.initialize();
}
