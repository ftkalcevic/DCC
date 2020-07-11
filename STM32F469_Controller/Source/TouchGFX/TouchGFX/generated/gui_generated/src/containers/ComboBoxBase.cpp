/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/containers/ComboBoxBase.hpp>
#include "BitmapDatabase.hpp"

ComboBoxBase::ComboBoxBase() :
    updateItemCallback(this, &ComboBoxBase::updateItemCallbackHandler)
{
    setWidth(360);
    setHeight(50);
    scrollWheel.setPosition(0, 0, 360, 50);
    scrollWheel.setHorizontal(true);
    scrollWheel.setCircular(false);
    scrollWheel.setEasingEquation(touchgfx::EasingEquations::backEaseOut);
    scrollWheel.setSwipeAcceleration(10);
    scrollWheel.setDragAcceleration(10);
    scrollWheel.setNumberOfItems(10);
    scrollWheel.setSelectedItemOffset(40);
    scrollWheel.setDrawableSize(260, 10);
    scrollWheel.setDrawables(scrollWheelListItems, updateItemCallback);
    scrollWheel.animateToItem(0, 0);

    tiledImage.setBitmap(touchgfx::Bitmap(BITMAP_COMBOSCROLLWHEELMASK_ID));
    tiledImage.setPosition(0, 0, 360, 50);
    tiledImage.setOffset(0, 0);

    add(scrollWheel);
    add(tiledImage);
}

void ComboBoxBase::initialize()
{
    scrollWheel.initialize();
    for (int i = 0; i < scrollWheelListItems.getNumberOfDrawables(); i++)
    {
        scrollWheelListItems[i].initialize();
    }
}

void ComboBoxBase::updateItemCallbackHandler(touchgfx::DrawableListItemsInterface* items, int16_t containerIndex, int16_t itemIndex)
{
    if (items == &scrollWheelListItems)
    {
        touchgfx::Drawable* d = items->getDrawable(containerIndex);
        ComboItem* cc = (ComboItem*)d;
        scrollWheelUpdateItem(*cc, itemIndex);
    }
}