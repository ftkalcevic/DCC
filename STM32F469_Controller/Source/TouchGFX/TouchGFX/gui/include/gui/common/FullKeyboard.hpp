#ifndef FULLKEYBOARD_HPP_
#define FULLKEYBOARD_HPP_

#include <touchgfx/widgets/Keyboard.hpp>
#include <touchgfx/containers/Container.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <gui/common/FullKeyboardLayout.hpp>
#include <fonts/ApplicationFontProvider.hpp>
#include <gui/common/FullKeyboardKeyMapping.hpp>

using namespace touchgfx;

/**
 * An alphanumeric keyboard with backspace, space-bar and delete functionality.
 * Keyboard mappings for the keyboard are defined in gui/include/gui/common/KeyboardKeyMapping.hpp.
 * Keyboard layout for the keyboard is defined in gui/include/gui/common/KeyboardLayout.hpp.
 * Please note that the ApplicationFontProvider is initialized and set with the FontManager in main.cpp
 * The fonts and the characters used by the keyboard have to be defined in the assets/texts/texts.xlsx file.
 */
class FullKeyboard : public Container
{
public:
    FullKeyboard();
    virtual ~FullKeyboard() { }

    /*
     * Override setTouchable to also affect the keyboard object inside this
     * FullKeyboard.
     */
    void setTouchable(bool touch);
    void setTitle(const Unicode::UnicodeChar *title) { keyboard.setTitle(title);}
	void setText(const Unicode::UnicodeChar *text)
	{
		Unicode::strncpy(buffer, text, BUFFER_SIZE);
		buffer[BUFFER_SIZE - 1] = 0;
	}
	const Unicode::UnicodeChar *getText() const
	{
		return buffer;
	}
	void setTextMaxLen(uint16_t maxLen);
	void setEntryAreaFont(FontId fontId) { keyboard.setEntryAreaFont(fontId); }
	void setEntryAreaAlignment(Alignment align) { keyboard.setEntryAreaAlignment( align ); }
	
    void setCloseWindowCallback(touchgfx::GenericCallback<bool>& callback)
    {
        this->closeWindowCallback = &callback;
    }
    virtual void getLastChild(int16_t x, int16_t y, Drawable** last);
    virtual Rect getContainedArea() const;
	void Reset();
	
private:
    /*
     * The size of the buffer that is used by the keyboard.
     * The size determines how much text the keyboard can contain in its textfield.
     */
    static const uint8_t BUFFER_SIZE = 127;

    /**
     * The keyboard which this FullKeyboard wraps.
     */
    Keyboard keyboard;

    /**
     * The buffer used by the keyboard for text input.
     */
    Unicode::UnicodeChar buffer[BUFFER_SIZE];

    /**
     * Used to display text on top of the button for changing keyboard mode.
     */
    Callback<FullKeyboard> capslockPressed;
    Callback<FullKeyboard> backspacePressed;
    Callback<FullKeyboard> modePressed;
    Callback<FullKeyboard> setPressed;
    Callback<FullKeyboard> cancelPressed;

    /**
     * Callback for when keys are pressed on the keyboard.
     */
    Callback<FullKeyboard, Unicode::UnicodeChar> keyPressed;

    /**
     * True if the keyboard should show alpha keys, false for numeric.
     */
    bool alphaKeys;

    /**
     * True if the keyboard should show upper-case keys.
     */
    bool uppercaseKeys;

    /**
     * True if the input position in the text field, and hence the buffer, is at the beginning.
     */
    bool firstCharacterEntry;

    /*
     * Sets the correct key mappings of the keyboard according to alpha/numeric and upper-case/lower-case.
     */
    void setKeyMappingList();

    /**
     * Callback handler for the backspace button.
     */
    void backspacePressedHandler();

    /**
     * Callback handler for the caps-lock button.
     */
    void capslockPressedHandler();

    /**
     * Callback handler for the mode button.
     */
    void modePressedHandler();
    void setPressedHandler();
    void cancelPressedHandler();

    /**
     * Callback handler for key presses.
     * @param keyChar The UnicodeChar for the key that was pressed.
     */
    void keyPressedhandler(Unicode::UnicodeChar keyChar);
	
	touchgfx::GenericCallback<bool>* closeWindowCallback;
    virtual void emitCloseWindow(bool success)
    {
        if (closeWindowCallback != nullptr && closeWindowCallback->isValid())
        {
            this->closeWindowCallback->execute(success);
        }
    }	
};

#endif /* FULLKEYBOARD_HPP_ */
