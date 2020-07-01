#include <gui/common/NumericKeypad.hpp>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

NumericKeypad::NumericKeypad() : keyboard(),
	mode(Dec),
    clearPressed(this, &NumericKeypad::clearPressedHandler),
    modePressed(this, &NumericKeypad::modePressedHandler),
    setPressed(this, &NumericKeypad::setPressedHandler),
    cancelPressed(this, &NumericKeypad::cancelPressedHandler),
    keyPressed(this, &NumericKeypad::keyPressedhandler)
{
	backgroundBox.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
	backgroundBox.setAlpha(128);
	backgroundBox.setPosition(0, 0, HAL::DISPLAY_WIDTH, HAL::DISPLAY_HEIGHT);
	backgroundBox.setTouchable(true);
	add(backgroundBox);
	
	rangeMin = 0;
	rangeMax = 0xFF;
	
    //Set the callbacks for the callback areas of the keyboard and set its layout.
    numericKeypadLayout.callbackAreaArray[NumericCallbacks::Clear].callback = &clearPressed;
    numericKeypadLayout.callbackAreaArray[NumericCallbacks::Mode].callback = &modePressed;
    numericKeypadLayout.callbackAreaArray[NumericCallbacks::Cancel].callback = &cancelPressed;
    numericKeypadLayout.callbackAreaArray[NumericCallbacks::Set].callback = &setPressed;
    keyboard.setLayout(&numericKeypadLayout);
    keyboard.setKeyListener(keyPressed);
    keyboard.setPosition((800-465)/2, 15, 465, 455);

	//Allocate the buffer associated with keyboard.
    memset(buffer, 0, sizeof(buffer));
    keyboard.setBuffer(buffer, BUFFER_SIZE);

    setKeyMappingList();
	setMode(Dec);
	
    add(keyboard);
}

void NumericKeypad::setKeyMappingList()
{
    keyboard.setKeymappingList(&numericKeypadKeyMappingList);
}

void NumericKeypad::clearPressedHandler()
{
    buffer[0] = '0';
    buffer[1] = 0;
    keyboard.setBufferPosition(1);
	keyboard.enteredTextInvalidate();
}

void NumericKeypad::setMode(EMode m)
{
	Keyboard::CallbackArea &cb = numericKeypadLayout.callbackAreaArray[NumericCallbacks::Mode];
	mode = m;
	switch (mode)
	{
		case Dec: cb.str = u"Dec"; break;
		case Hex: cb.str = u"Hex"; break;
		case Bin: cb.str = u"Bin"; break;
	}
	keyboard.invalidateKeyRect(cb.keyId);
}

static long Unicode_strtol(const Unicode::UnicodeChar *buffer, const Unicode::UnicodeChar **endPtr, int base)
{
	long out = 0;
	const Unicode::UnicodeChar *ptr = buffer;
	while ( *ptr != 0 )
	{
		Unicode::UnicodeChar c = *ptr;
		uint16_t n;
		if (c >= '0' && c <= '9')
			n = c - '0';
		else if (c >= 'A' && c <= 'F')
			n = c - 'A' + 0x0A;
		else if (c >= 'a' && c <= 'f')
			n = c - 'a' + 0x0A;
		else
			break;
		if (n >= base)
			break;
		out *= base;
		out += n;
		ptr++;
	}
	if ( endPtr != NULL )
		*endPtr = ptr;
	return out;
}

uint16_t NumericKeypad::getNumber() const
{
	uint16_t n;
	if (mode == Dec)
	{
		n = Unicode_strtol(buffer, NULL, 10);
	}
	else if (mode == Hex)
	{
		n = Unicode_strtol(buffer, NULL, 16);
	}
	else //if (mode == Bin)
	{
		n = Unicode_strtol(buffer, NULL, 2);
	}
	return n;
}

void NumericKeypad::setNumber(uint16_t n)
{
	if (mode == Dec)
	{
		Unicode::itoa(n, buffer, BUFFER_SIZE, 10);
	}
	else if (mode == Hex)
	{
		Unicode::snprintf(buffer, BUFFER_SIZE, (const Unicode::UnicodeChar *)u"%02X", n);
	}
	else //if (mode == Bin)
	{
		n &= 0xFF;	// max 8 bits/characters
		for(int i = 0 ; i < 8 ; i++)
			buffer[i] = (n & (1 << (7-i))) ? '1' : '0';
		buffer[8] = 0;
	}
	keyboard.setBufferPosition(Unicode::strlen(buffer));
	keyboard.enteredTextInvalidate();
}

void NumericKeypad::modePressedHandler()
{
	uint16_t n = getNumber();
	switch ( mode )
	{
		case Dec: setMode(Hex); break;
		case Hex: setMode(Bin); break;
		case Bin:
		default:  setMode(Dec); break;
	}
	setNumber(n);
    setKeyMappingList();
}

void NumericKeypad::setPressedHandler()
{
	emitCloseWindow(true);
}

void NumericKeypad::cancelPressedHandler()
{
	emitCloseWindow(false);
}

void NumericKeypad::keyPressedhandler(Unicode::UnicodeChar keyChar)
{
}

void NumericKeypad::setTouchable(bool touch)
{
    Container::setTouchable(touch);
    keyboard.setTouchable(touch);
}


void NumericKeypad::handleDragEvent(const DragEvent& evt)
{
}