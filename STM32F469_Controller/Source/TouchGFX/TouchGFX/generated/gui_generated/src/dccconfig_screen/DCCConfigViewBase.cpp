/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/dccconfig_screen/DCCConfigViewBase.hpp>
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"

DCCConfigViewBase::DCCConfigViewBase() :
    updateItemCallback(this, &DCCConfigViewBase::updateItemCallbackHandler)
{

    touchgfx::CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);

    statusBar.setXY(0, 0);

    functionButtons.setXY(0, 455);

    scrollableContainer1.setPosition(0, 25, 800, 430);
    scrollableContainer1.setScrollbarsColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));

    backgroundImage.setBitmap(touchgfx::Bitmap(BITMAP_DARK_TEXTURES_CARBON_FIBRE_ID));
    backgroundImage.setPosition(0, 0, 800, 480);
    backgroundImage.setOffset(0, 0);
    scrollableContainer1.add(backgroundImage);

    line1.setPosition(538, 17, 250, 240);
    line1Painter.setColor(touchgfx::Color::getColorFrom24BitRGB(71, 71, 71));
    line1.setPainter(line1Painter);
    line1.setStart(3, 3);
    line1.setEnd(247, 3);
    line1.setLineWidth(5);
    line1.setLineEndingStyle(touchgfx::Line::ROUND_CAP_ENDING);
    scrollableContainer1.add(line1);

    line1_1.setPosition(538, 17, 250, 240);
    line1_1Painter.setColor(touchgfx::Color::getColorFrom24BitRGB(71, 71, 71));
    line1_1.setPainter(line1_1Painter);
    line1_1.setStart(3, 3);
    line1_1.setEnd(3, 237);
    line1_1.setLineWidth(5);
    line1_1.setLineEndingStyle(touchgfx::Line::ROUND_CAP_ENDING);
    scrollableContainer1.add(line1_1);

    line1_2.setPosition(538, 17, 250, 240);
    line1_2Painter.setColor(touchgfx::Color::getColorFrom24BitRGB(71, 71, 71));
    line1_2.setPainter(line1_2Painter);
    line1_2.setStart(3, 237);
    line1_2.setEnd(247, 237);
    line1_2.setLineWidth(5);
    line1_2.setLineEndingStyle(touchgfx::Line::ROUND_CAP_ENDING);
    scrollableContainer1.add(line1_2);

    line1_3.setPosition(538, 17, 250, 240);
    line1_3Painter.setColor(touchgfx::Color::getColorFrom24BitRGB(71, 71, 71));
    line1_3.setPainter(line1_3Painter);
    line1_3.setStart(247, 3);
    line1_3.setEnd(247, 237);
    line1_3.setLineWidth(5);
    line1_3.setLineEndingStyle(touchgfx::Line::ROUND_CAP_ENDING);
    scrollableContainer1.add(line1_3);

    toggleProgTrack.setXY(547, 27);
    toggleProgTrack.setBitmaps(touchgfx::Bitmap(BITMAP_PROGTRACKBUTTONUP_ID), touchgfx::Bitmap(BITMAP_PROGTRACKBUTTONDOWN_ID));
    scrollableContainer1.add(toggleProgTrack);

    scrollList1.setPosition(17, 17, 350, 240);
    scrollList1.setHorizontal(false);
    scrollList1.setCircular(false);
    scrollList1.setEasingEquation(touchgfx::EasingEquations::backEaseOut);
    scrollList1.setSwipeAcceleration(10);
    scrollList1.setDragAcceleration(10);
    scrollList1.setNumberOfItems(1);
    scrollList1.setPadding(0, 0);
    scrollList1.setSnapping(false);
    scrollList1.setDrawableSize(100, 0);
    scrollList1.setDrawables(scrollList1ListItems, updateItemCallback);
    scrollableContainer1.add(scrollList1);
    scrollableContainer1.setScrollbarsPermanentlyVisible();
    scrollableContainer1.setScrollbarsVisible(false);

    add(statusBar);
    add(functionButtons);
    add(scrollableContainer1);
}

void DCCConfigViewBase::setupScreen()
{
    statusBar.initialize();
    functionButtons.initialize();
    scrollList1.initialize();
    for (int i = 0; i < scrollList1ListItems.getNumberOfDrawables(); i++)
    {
        scrollList1ListItems[i].initialize();
    }
}

void DCCConfigViewBase::updateItemCallbackHandler(touchgfx::DrawableListItemsInterface* items, int16_t containerIndex, int16_t itemIndex)
{
    if (items == &scrollList1ListItems)
    {
        touchgfx::Drawable* d = items->getDrawable(containerIndex);
        ListItem* cc = (ListItem*)d;
        scrollList1UpdateItem(*cc, itemIndex);
    }
}
