#include <gui/preferences_screen/PreferencesView.hpp>

PreferencesView::PreferencesView()
{
    scrollList1.setNumberOfItems(5);
}

void PreferencesView::setupScreen()
{
    PreferencesViewBase::setupScreen();
}

void PreferencesView::tearDownScreen()
{
    PreferencesViewBase::tearDownScreen();
}


void PreferencesView::scrollList1UpdateItem(ListItem& item, int16_t itemIndex)
{
	switch (itemIndex)
	{
		case 0: item.setText((const uint16_t*)u"Settings"); break;
		
	}
}

void PreferencesView::handleClickEvent(const ClickEvent & evt)
{
	PreferencesViewBase::handleClickEvent(evt);
}
void PreferencesView::handleDragEvent(const DragEvent & evt)
{
	PreferencesViewBase::handleDragEvent(evt);
}
void PreferencesView::handleGestureEvent(const GestureEvent & evt)
{
	PreferencesViewBase::handleGestureEvent(evt);
}
void PreferencesView::handleKeyEvent(uint8_t key)
{
	PreferencesViewBase::handleKeyEvent(key);
}
