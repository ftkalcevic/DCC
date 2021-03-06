/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/preferences_screen/PreferencesViewBase.hpp>
#include "BitmapDatabase.hpp"

PreferencesViewBase::PreferencesViewBase()
{

    touchgfx::CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);

    backgroundImage.setBitmap(touchgfx::Bitmap(BITMAP_DARK_TEXTURES_CARBON_FIBRE_ID));
    backgroundImage.setPosition(0, 0, 800, 480);
    backgroundImage.setOffset(0, 0);

    swipeContainer.setXY(0, 25);
    swipeContainer.setPageIndicatorBitmaps(touchgfx::Bitmap(BITMAP_BLUE_PAGEINDICATOR_DOT_INDICATOR_SMALL_NORMAL_ID), touchgfx::Bitmap(BITMAP_BLUE_PAGEINDICATOR_DOT_INDICATOR_SMALL_HIGHLIGHT_ID));
    swipeContainer.setPageIndicatorXY(393, 404);
    swipeContainer.setSwipeCutoff(50);
    swipeContainer.setEndSwipeElasticWidth(50);

    swipeContainerPage1.setWidth(800);
    swipeContainerPage1.setHeight(430);
    swipeContainer.add(swipeContainerPage1);
    swipeContainer.setSelectedPage(0);

    functionButtons.setXY(0, 455);

    statusBar.setXY(0, 0);

    add(backgroundImage);
    add(swipeContainer);
    add(functionButtons);
    add(statusBar);
}

void PreferencesViewBase::setupScreen()
{
    functionButtons.initialize();
    statusBar.initialize();
}
