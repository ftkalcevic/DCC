#include <gui/common/FullKeyboard.hpp>

FullKeyboard::FullKeyboard() : keyboard(),
    modeBtnTextArea(),
    capslockPressed(this, &FullKeyboard::capslockPressedHandler),
    backspacePressed(this, &FullKeyboard::backspacePressedHandler),
    modePressed(this, &FullKeyboard::modePressedHandler),
    setPressed(this, &FullKeyboard::setPressedHandler),
    cancelPressed(this, &FullKeyboard::cancelPressedHandler),
    keyPressed(this, &FullKeyboard::keyPressedhandler),
    alphaKeys(true),
    uppercaseKeys(false),
    firstCharacterEntry(false)
{
	backgroundBox.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
	backgroundBox.setAlpha(128);
	backgroundBox.setPosition(0, 0, HAL::DISPLAY_WIDTH, HAL::DISPLAY_HEIGHT);
	backgroundBox.setTouchable(true);
	add(backgroundBox);
	
    //Set the callbacks for the callback areas of the keyboard and set its layout.
    fullKeyboardLayout.callbackAreaArray[KeyboardCallbacks::CapsLock].callback = &capslockPressed;
    fullKeyboardLayout.callbackAreaArray[KeyboardCallbacks::Backspace].callback = &backspacePressed;
    fullKeyboardLayout.callbackAreaArray[KeyboardCallbacks::Mode].callback = &modePressed;
    fullKeyboardLayout.callbackAreaArray[KeyboardCallbacks::Set].callback = &setPressed;
    fullKeyboardLayout.callbackAreaArray[KeyboardCallbacks::Cancel].callback = &cancelPressed;
    keyboard.setLayout(&fullKeyboardLayout);
    keyboard.setKeyListener(keyPressed);
    keyboard.setPosition(37, 15, 726, 455);
    //Allocate the buffer associated with keyboard.
    memset(buffer, 0, sizeof(buffer));
    keyboard.setBuffer(buffer, BUFFER_SIZE);

    uppercaseKeys = true;
    firstCharacterEntry = true;

//    modeBtnTextArea.setPosition(5, 196, 56, 40);
//    modeBtnTextArea.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));

    setKeyMappingList();

    add(keyboard);
//    add(modeBtnTextArea);
}

void FullKeyboard::setKeyMappingList()
{
    if (alphaKeys)
    {
	    //modeBtnTextArea.setTypedText(TypedText(T_ALPHAMODE));
		fullKeyboardCallbackAreas[KeyboardCallbacks::Mode].str = u"Abc";
        if (uppercaseKeys)
        {
            keyboard.setKeymappingList(&fullKeyboardKeyMappingListAlphaUpper);
        }
        else
        {
            keyboard.setKeymappingList(&fullKeyboardKeyMappingListAlphaLower);
        }
    }
    else
    {
        //modeBtnTextArea.setTypedText(TypedText(T_NUMMODE));
		fullKeyboardCallbackAreas[KeyboardCallbacks::Mode].str = u"123";
        if (uppercaseKeys)
        {
            keyboard.setKeymappingList(&fullKeyboardKeyMappingListNumUpper);
        }
        else
        {
            keyboard.setKeymappingList(&fullKeyboardKeyMappingListNumLower);
        }
    }
	keyboard.invalidateKeyRect(fullKeyboardCallbackAreas[KeyboardCallbacks::Mode].keyId);
}

void FullKeyboard::backspacePressedHandler()
{
    uint16_t pos = keyboard.getBufferPosition();
    if (pos > 0)
    {
        //Delete the previous entry in the buffer and decrement the position.
        buffer[pos - 1] = 0;
        keyboard.setBufferPosition(pos - 1);

        //Change keymappings if we have reached the first position.
        if (1 == pos)
        {
            firstCharacterEntry = true;
            uppercaseKeys = true;
            setKeyMappingList();
        }
    }
}

void FullKeyboard::capslockPressedHandler()
{
    uppercaseKeys = !uppercaseKeys;
    setKeyMappingList();
}

void FullKeyboard::modePressedHandler()
{
    alphaKeys = !alphaKeys;

    // if we have changed back to alpha and still has no chars in the buffer,
    // we show upper case letters.
    if (firstCharacterEntry && alphaKeys)
    {
        uppercaseKeys = true;
    }
    else
    {
        uppercaseKeys = false;
    }
    setKeyMappingList();
}

void FullKeyboard::setPressedHandler()
{
	emitCloseWindow(true);
}

void FullKeyboard::cancelPressedHandler()
{
	emitCloseWindow(false);
}


void FullKeyboard::keyPressedhandler(Unicode::UnicodeChar keyChar)
{
    // After the first keypress, the keyboard will shift to lowercase.
    if (firstCharacterEntry && keyChar != 0)
    {
        firstCharacterEntry = false;
        uppercaseKeys = false;
        setKeyMappingList();
    }
}

void FullKeyboard::setTouchable(bool touch)
{
    Container::setTouchable(touch);
    keyboard.setTouchable(touch);
}

Rect FullKeyboard::getContainedArea() const
{
	return Container::getContainedArea();
}
	
void FullKeyboard::getLastChild(int16_t x, int16_t y, Drawable** last)
{
	Container::getLastChild(x, y, last);
}	

