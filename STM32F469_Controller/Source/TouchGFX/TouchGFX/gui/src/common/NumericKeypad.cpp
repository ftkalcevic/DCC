#include <gui/common/NumericKeypad.hpp>

NumericKeypad::NumericKeypad() : keyboard(),
    modeBtnTextArea(),
    capslockPressed(this, &NumericKeypad::capslockPressedHandler),
    backspacePressed(this, &NumericKeypad::backspacePressedHandler),
    modePressed(this, &NumericKeypad::modePressedHandler),
    keyPressed(this, &NumericKeypad::keyPressedhandler),
    alphaKeys(true),
    uppercaseKeys(false),
    firstCharacterEntry(false)
{
    //Set the callbacks for the callback areas of the keyboard and set its layout.
    numericKeypadLayout.callbackAreaArray[0].callback = &capslockPressed;
    numericKeypadLayout.callbackAreaArray[1].callback = &backspacePressed;
    numericKeypadLayout.callbackAreaArray[2].callback = &modePressed;
    keyboard.setLayout(&numericKeypadLayout);
    keyboard.setKeyListener(keyPressed);
    keyboard.setPosition((800-465)/2, 15, 465, 455);
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

void NumericKeypad::setKeyMappingList()
{
    //modeBtnTextArea.setTypedText(TypedText(T_NUMMODE));
    keyboard.setKeymappingList(&numericKeypadKeyMappingList);
}

void NumericKeypad::backspacePressedHandler()
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

void NumericKeypad::capslockPressedHandler()
{
    uppercaseKeys = !uppercaseKeys;
    setKeyMappingList();
}

void NumericKeypad::modePressedHandler()
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

void NumericKeypad::keyPressedhandler(Unicode::UnicodeChar keyChar)
{
    // After the first keypress, the keyboard will shift to lowercase.
    if (firstCharacterEntry && keyChar != 0)
    {
        firstCharacterEntry = false;
        uppercaseKeys = false;
        setKeyMappingList();
    }
}

void NumericKeypad::setTouchable(bool touch)
{
    Container::setTouchable(touch);
    keyboard.setTouchable(touch);
}
