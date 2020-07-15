#ifndef PREFERENCESVIEW_HPP
#define PREFERENCESVIEW_HPP

#include <gui_generated/preferences_screen/PreferencesViewBase.hpp>
#include <gui/preferences_screen/PreferencesPresenter.hpp>
#include <touchgfx/widgets/AbstractButton.hpp>
#include <gui/common/CustomButton.hpp>
#include <vector>
#include <memory>

class PreferencesView : public PreferencesViewBase
{
public:
    PreferencesView();
    virtual ~PreferencesView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
	virtual void	handleClickEvent(const ClickEvent & evt);
	virtual void	handleDragEvent(const DragEvent & evt);
	virtual void	handleGestureEvent(const GestureEvent & evt);
	virtual void	handleKeyEvent(uint8_t key);
protected:
    BitmapId bmpId;
    touchgfx::Callback<PreferencesView, const touchgfx::AbstractButton&> buttonClickCallback;
    void buttonClickHandler(const touchgfx::AbstractButton& src);
	std::vector<std::shared_ptr<touchgfx::CustomButton>> buttons;
};

#endif // PREFERENCESVIEW_HPP
