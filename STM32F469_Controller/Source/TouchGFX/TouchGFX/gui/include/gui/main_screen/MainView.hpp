#ifndef MAIN_VIEW_HPP
#define MAIN_VIEW_HPP

#include <gui_generated/main_screen/MainViewBase.hpp>
#include <gui/main_screen/MainPresenter.hpp>
#include <touchgfx/widgets/AbstractButton.hpp>
#include <gui/common/CustomButton.hpp>
#include <vector>
#include <memory>


class MainView : public MainViewBase
{
public:
    MainView();
	virtual ~MainView();
    virtual void setupScreen();
    virtual void tearDownScreen();


protected:
	virtual void	handleClickEvent(const ClickEvent & evt);
	virtual void	handleDragEvent(const DragEvent & evt);
	virtual void	handleGestureEvent(const GestureEvent & evt);
	virtual void	handleKeyEvent(uint8_t key);
private:
	BitmapId bmpId;
	std::vector<std::shared_ptr<touchgfx::Container>> pages;
	std::vector<std::shared_ptr<touchgfx::CustomButton>> buttons;
	
    touchgfx::Callback<MainView, const touchgfx::AbstractButton&> buttonClickCallback;
    void buttonClickHandler(const touchgfx::AbstractButton& src);
	void deletePage(Drawable& d);
};

#endif // MAIN_VIEW_HPP
