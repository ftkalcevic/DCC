#ifndef FULLKEYBOARD_LAYOUT
#define FULLKEYBOARD_LAYOUT

#include <touchgfx/widgets/Keyboard.hpp>
#include <touchgfx/hal/Types.hpp>
#include <fonts/ApplicationFontProvider.hpp>
#include "BitmapDatabase.hpp"

using namespace touchgfx;



#if !defined(USE_BPP) || USE_BPP==16
static_assert(false);
#elif USE_BPP==24
    #define GETCOLORFROM24BITRGB(red,green,blue)    ((red << 16) | (green << 8) | (blue))
#elif USE_BPP==4
__cpp_static_assert(false);
#elif USE_BPP==2
__cpp_static_assert(false);
#else
#error Unknown USE_BPP
#endif



/**
 * Array specifying the keys used in the CustomKeyboard.
 */
static const Keyboard::Key fullKeyboardKeyArray[30] =
{
    { 1, Rect(14 + 64 * 0, 77, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    { 2, Rect(14 + 64 * 1, 77, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    { 3, Rect(14 + 64 * 2, 77, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    { 4, Rect(14 + 64 * 3, 77, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    { 5, Rect(14 + 64 * 4, 77, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    { 6, Rect(14 + 64 * 5, 77, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    { 7, Rect(14 + 64 * 6, 77, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    { 8, Rect(14 + 64 * 7, 77, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    { 9, Rect(14 + 64 * 8, 77, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {10, Rect(14 + 64 * 9, 77, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {11, Rect(14 + 64 * 10,77, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},

    {12, Rect(14 + 64 * 0, 147, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {13, Rect(14 + 64 * 1, 147, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {14, Rect(14 + 64 * 2, 147, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {15, Rect(14 + 64 * 3, 147, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {16, Rect(14 + 64 * 4, 147, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {17, Rect(14 + 64 * 5, 147, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {18, Rect(14 + 64 * 6, 147, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {19, Rect(14 + 64 * 7, 147, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {20, Rect(14 + 64 * 8, 147, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {21, Rect(14 + 64 * 9, 147, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {22, Rect(14 + 64 * 10,147, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},

    {23, Rect(14 + 64 * 2, 217, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {24, Rect(14 + 64 * 3, 217, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {25, Rect(14 + 64 * 4, 217, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {26, Rect(14 + 64 * 5, 217, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {27, Rect(14 + 64 * 6, 217, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {28, Rect(14 + 64 * 7, 217, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {29, Rect(14 + 64 * 8, 217, 52, 62), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
	
    {30, Rect(14 + 64 * 2, 287, 436, 62), BITMAP_KEYBOARD_SPACE_HIGHLIGHTED_ID}
};

/**
 * Callback areas for the special buttons on the CustomKeyboard.
 */
static Keyboard::CallbackArea fullKeyboardCallbackAreas[5] =
{
    {128, Rect(14 + 64 * 0, 217, 116, 62), 0, u"caps",   BITMAP_KEYBOARD_KEY_SHIFT_HIGHLIGHTED_ID},     // caps-lock
    {129, Rect(14 + 64 * 9, 217, 116, 62), 0, u"bksp",   BITMAP_KEYBOARD_KEY_DELETE_HIGHLIGHTED_ID},  // backspace
    {130, Rect(14 + 64 * 0, 287, 116, 62), 0, u"123",    BITMAP_KEYBOARD_KEY_NUM_HIGHLIGHTED_ID},       // mode
    {131, Rect(14,  380, 155, 62),         0, u"Set",    BITMAP_KEYBOARD_KEY_NUM_HIGHLIGHTED_ID},       // set
    {132, Rect(551, 380, 155, 62),         0, u"Cancel", BITMAP_KEYBOARD_KEY_NUM_HIGHLIGHTED_ID},       // cancel
};

/**
 * The layout for the CustomKeyboard.
 */
static const Keyboard::Layout fullKeyboardLayout =
{
	.backColor              =GETCOLORFROM24BITRGB(216, 216, 216),
	.backAlpha              =0xFF,
    .bitmap                 =0,
    .keyArray               =fullKeyboardKeyArray,
    .numberOfKeys           =30,
    .callbackAreaArray      =fullKeyboardCallbackAreas,
    .numberOfCallbackAreas  =5,
    .textAreaPosition       =Rect(346, 15, 360, 50),
    .textAreaFont           =Typography::DISPLAY,
    .textAreaFontColor      =GETCOLORFROM24BITRGB(0, 0, 0),
    .textAreaBackColor      =GETCOLORFROM24BITRGB(239, 239, 239),
    .keyFont                =Typography::TYPOGRAPHY_40PX,
    .keyFontColor           =0,
    .keyBackColor           =GETCOLORFROM24BITRGB(239, 239, 239),
    .keyDownBackColor       =GETCOLORFROM24BITRGB(216, 216, 216),
    .keyAlpha               =0xFF,
	.shadowDepth            =5,
	.shadowColor            =GETCOLORFROM24BITRGB(180, 180, 180),
};

#endif
