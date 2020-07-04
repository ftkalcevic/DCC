#ifndef NUMERICKEYPAD_HPP_
#define NUMERICKEYPAD_HPP_

#include <touchgfx/widgets/Keyboard.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <gui/common/NumericKeypadLayout.hpp>
#include <fonts/ApplicationFontProvider.hpp>
#include <gui/common/NumericKeypadKeyMapping.hpp>

using namespace touchgfx;

/**
 * An alphanumeric keyboard with backspace, space-bar and delete functionality.
 * Keyboard mappings for the keyboard are defined in gui/include/gui/common/KeyboardKeyMapping.hpp.
 * Keyboard layout for the keyboard is defined in gui/include/gui/common/KeyboardLayout.hpp.
 * Please note that the ApplicationFontProvider is initialized and set with the FontManager in main.cpp
 * The fonts and the characters used by the keyboard have to be defined in the assets/texts/texts.xlsx file.
 */
class NumericKeypad : public Container
{
public:
    NumericKeypad();
    virtual ~NumericKeypad() { }

    /*
     * Override setTouchable to also affect the keyboard object inside this
     * NumericKeypad.
     */
    void setTouchable(bool touch);
    void setTitle(const Unicode::UnicodeChar *title) { keyboard.setTitle(title);}
	void setNumber(uint16_t n);
	uint16_t getNumber() const;
	void setRange(uint16_t min, uint16_t max)
	{
		rangeMin = min;
		rangeMax = max;
	}
    void setCloseWindowCallback(touchgfx::GenericCallback<bool>& callback)
    {
        this->closeWindowCallback = &callback;
    }
	virtual void handleDragEvent(const DragEvent& evt);
	void Reset();
	
private:
	uint16_t rangeMin;
	uint16_t rangeMax;
	static const uint8_t BUFFER_SIZE = 9;
    Unicode::UnicodeChar buffer[BUFFER_SIZE];

    Keyboard keyboard;

	enum EMode
	{
		Dec,
		Hex,
		Bin
	} mode;
/**
     * Callback for the capslock button.
     */
    Callback<NumericKeypad> clearPressed;
    Callback<NumericKeypad> modePressed;
    Callback<NumericKeypad> setPressed;
    Callback<NumericKeypad> cancelPressed;
    Callback<NumericKeypad, Unicode::UnicodeChar> keyPressed;

	touchgfx::GenericCallback<bool>* closeWindowCallback;
    virtual void emitCloseWindow(bool success)
    {
        if (closeWindowCallback != nullptr && closeWindowCallback->isValid())
        {
            this->closeWindowCallback->execute(success);
        }
    }	
	

    /*
     * Sets the correct key mappings of the keyboard according to alpha/numeric and upper-case/lower-case.
     */
    void setKeyMappingList();

    /**
     * Callback handler for the backspace button.
     */
    void clearPressedHandler();
    void modePressedHandler();
    void setPressedHandler();
    void cancelPressedHandler();

    /**
     * Callback handler for key presses.
     * @param keyChar The UnicodeChar for the key that was pressed.
     */
    void keyPressedhandler(Unicode::UnicodeChar keyChar);

	void setMode(EMode m);
};

#endif /* NUMERICKEYPAD_HPP_ */
