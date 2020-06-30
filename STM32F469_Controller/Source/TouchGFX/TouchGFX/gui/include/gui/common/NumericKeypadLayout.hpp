#ifndef NUMERICKEYPAD_LAYOUT
#define NUMERICKEYPAD_LAYOUT

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
static const Keyboard::Key NumericKeypadKeyArray[30] =
{
    { 1, Rect(15 + 80 * 0, 85, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    { 2, Rect(15 + 80 * 1, 85, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    { 3, Rect(15 + 80 * 2, 85, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    { 4, Rect(15 + 80 * 3, 85, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},

    { 5, Rect(15 + 80 * 0, 155, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    { 6, Rect(15 + 80 * 1, 155, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    { 7, Rect(15 + 80 * 2, 155, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    { 8, Rect(15 + 80 * 3, 155, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},

    { 9, Rect(15 + 80 * 0, 225, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {10, Rect(15 + 80 * 1, 225, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {11, Rect(15 + 80 * 2, 225, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {12, Rect(15 + 80 * 3, 225, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},

    {13, Rect(15 + 80 * 0, 295, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {14, Rect(15 + 80 * 1, 295, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {15, Rect(15 + 80 * 2, 295, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
    {16, Rect(15 + 80 * 3, 295, 60, 60), BITMAP_KEYBOARD_KEY_HIGHLIGHTED_ID},
};

/**
 * Callback areas for the special buttons on the CustomKeyboard.
 */
static Keyboard::CallbackArea NumericKeypadCallbackAreas[5] =
{
    {128, Rect(15 + 80 * 4, 85,  115, 60), 0, u"Clr",   BITMAP_KEYBOARD_KEY_SHIFT_HIGHLIGHTED_ID},      // Clear
    {129, Rect(15 + 80 * 4, 295, 115, 60), 0, u"Bin",   BITMAP_KEYBOARD_KEY_DELETE_HIGHLIGHTED_ID},     // Mode toggle
    {130, Rect(15, 365, 150, 72),          0, u"Cancel", BITMAP_KEYBOARD_KEY_NUM_HIGHLIGHTED_ID},       // cancel
    {131, Rect(300,365, 150, 72),          0, u"Set",    BITMAP_KEYBOARD_KEY_NUM_HIGHLIGHTED_ID},       // set
 };

/**
 * The layout for the CustomKeyboard.
 */
static const Keyboard::Layout numericKeypadLayout =
{
	.backColor              =GETCOLORFROM24BITRGB(216, 216, 216),
	.backAlpha              =0xFF,
    .bitmap                 =0,
    .keyArray               =NumericKeypadKeyArray,
    .numberOfKeys           =30,
    .callbackAreaArray      =NumericKeypadCallbackAreas,
    .numberOfCallbackAreas  =5,
    .textAreaPosition       =Rect(250, 15, 200, 50),
    .textAreaFont           =Typography::TYPOGRAPHY_NUMERIC_40PX,
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
