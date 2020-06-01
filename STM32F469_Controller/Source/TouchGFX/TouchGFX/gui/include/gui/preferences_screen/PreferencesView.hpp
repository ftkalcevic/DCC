#ifndef PREFERENCESVIEW_HPP
#define PREFERENCESVIEW_HPP

#include <gui_generated/preferences_screen/PreferencesViewBase.hpp>
#include <gui/preferences_screen/PreferencesPresenter.hpp>

class PreferencesView : public PreferencesViewBase
{
public:
    PreferencesView();
    virtual ~PreferencesView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
	virtual void scrollList1UpdateItem(ListItem& item, int16_t itemIndex);
	virtual void	handleClickEvent(const ClickEvent & evt);
	virtual void	handleDragEvent(const DragEvent & evt);
	virtual void	handleGestureEvent(const GestureEvent & evt);
	virtual void	handleKeyEvent(uint8_t key);
protected:
};

#endif // PREFERENCESVIEW_HPP
