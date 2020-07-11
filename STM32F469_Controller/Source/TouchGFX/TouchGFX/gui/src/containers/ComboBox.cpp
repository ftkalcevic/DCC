#include <gui/containers/ComboBox.hpp>
#include <stdio.h>

ComboBox::ComboBox() :
    cboAnimateToPositionCallback(this, &ComboBox::cboAnimateToPositionHandler),
	selectingNewItem(false)
{
	scrollWheel.setAnimateToCallback(cboAnimateToPositionCallback);
}

void ComboBox::initialize()
{
    ComboBoxBase::initialize();
}


void ComboBox::scrollWheelUpdateItem(ComboItem& item, int16_t itemIndex)
{
    if (updateCallback && updateCallback->isValid())
    {
        updateCallback->execute(*this, item, itemIndex);
    }
}


void ComboBox::cboAnimateToPositionHandler(int16_t index)
{
    if (!selectingNewItem && isVisible() && selectionChangedCallback && selectionChangedCallback->isValid())
    {
        selectionChangedCallback->execute(*this, index);
    }
}
