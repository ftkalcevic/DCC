#include <gui/common/FullKeyboard.hpp>

FullKeyboard::FullKeyboard() : keyboard(),
    modeBtnTextArea(),
    capslockPressed(this, &FullKeyboard::capslockPressedHandler),
    backspacePressed(this, &FullKeyboard::backspacePressedHandler),
    modePressed(this, &FullKeyboard::modePressedHandler),
    keyPressed(this, &FullKeyboard::keyPressedhandler),
    alphaKeys(true),
    uppercaseKeys(false),
    firstCharacterEntry(false)
{
    //Set the callbacks for the callback areas of the keyboard and set its layout.
    fullKeyboardLayout.callbackAreaArray[0].callback = &capslockPressed;
    fullKeyboardLayout.callbackAreaArray[1].callback = &backspacePressed;
    fullKeyboardLayout.callbackAreaArray[2].callback = &modePressed;
    keyboard.setLayout(&fullKeyboardLayout);
    keyboard.setKeyListener(keyPressed);
    keyboard.setPosition(37, 15, 726, 455);
    keyboard.setTextIndentation();
    //Allocate the buffer associated with keyboard.
    memset(buffer, 0, sizeof(buffer));
    keyboard.setBuffer(buffer, BUFFER_SIZE);

    uppercaseKeys = true;
    firstCharacterEntry = true;

    modeBtnTextArea.setPosition(5, 196, 56, 40);
    modeBtnTextArea.setColor(Color::getColorFrom24BitRGB(0xFF, 0xFF, 0xFF));

    setKeyMappingList();

    add(keyboard);
    add(modeBtnTextArea);
}

void FullKeyboard::setKeyMappingList()
{
    if (alphaKeys)
    {
        modeBtnTextArea.setTypedText(TypedText(T_ALPHAMODE));
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
        modeBtnTextArea.setTypedText(TypedText(T_NUMMODE));
        if (uppercaseKeys)
        {
            keyboard.setKeymappingList(&fullKeyboardKeyMappingListNumUpper);
        }
        else
        {
            keyboard.setKeymappingList(&fullKeyboardKeyMappingListNumLower);
        }
    }
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
