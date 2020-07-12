#include <gui/containers/ComboBoxBase.hpp>
#include <stdio.h>

ComboBoxBase::ComboBoxBase() :
    cboAnimateToPositionCallback(this, &ComboBoxBase::cboAnimateToPositionHandler),
	selectingNewItem(false)
{
	scrollWheel.setAnimateToCallback(cboAnimateToPositionCallback);
}

void ComboBoxBase::initialize()
{
    ComboBoxBaseBase::initialize();
}


void ComboBoxBase::scrollWheelUpdateItem(ComboItem& item, int16_t itemIndex)
{
    if (updateCallback && updateCallback->isValid())
    {
        updateCallback->execute(*this, item, itemIndex);
    }
}


void ComboBoxBase::cboAnimateToPositionHandler(int16_t index)
{
    if (!selectingNewItem && isVisible() && selectedIndexChangedCallback && selectedIndexChangedCallback->isValid())
    {
        selectedIndexChangedCallback->execute(*this, index);
    }
}
