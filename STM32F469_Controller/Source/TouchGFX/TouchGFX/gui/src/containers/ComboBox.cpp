#include <gui/containers/ComboBox.hpp>
#include <stdio.h>
#include <string.h>
#include "Utility.h"

ComboBox::ComboBox() :
    updateItemCallback(this, &ComboBox::updateItemHandler),
    selectedIndexChangedCallback(this, &ComboBox::selectedIndexChangedHandler),
	selectionChangedCallback(nullptr)
{
	setUpdateItemCallback(&updateItemCallback);
	setSelectedIndexChangedCallback(&selectedIndexChangedCallback);
	itemList.reserve(100);	// minimise chance of having to realloc
}

ComboBox::~ComboBox()
{
	for (auto it = itemList.begin(); it != itemList.end(); it++)
		if (it->heap)
			delete(it->str);
}


void ComboBox::addComboItem(const char16_t *item, int id)
{
	ComboBox::ItemData data;
	data.id = id;
	data.str = item;
	data.heap = false;
	itemList.push_back(data);
	setNumberOfItems(itemList.size());
}

void ComboBox::addComboItem(const char *item, int id)
{
	ComboBox::ItemData data;
	data.id = id;
	data.str = strdup16(item);
	data.heap = true;
	itemList.push_back(data);
	setNumberOfItems(itemList.size());
}

void ComboBox::updateItemHandler(ComboBoxBase& cbo, ComboItem& cboItem, int16_t itemIndex)
{
	if (itemIndex < itemList.size())
	{
		cboItem.setText(itemList[itemIndex].str);
	}
}


void ComboBox::selectedIndexChangedHandler(ComboBoxBase& cbo, int16_t index)
{
    if ( selectionChangedCallback && selectionChangedCallback->isValid())
    {
        selectionChangedCallback->execute(*this, index, itemList[index].id);
    }
}
