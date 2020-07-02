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
    { 1, Rect(14 + 64 * 0, 77, 52, 62), 0},
    { 2, Rect(14 + 64 * 1, 77, 52, 62), 0},
    { 3, Rect(14 + 64 * 2, 77, 52, 62), 0},
    { 4, Rect(14 + 64 * 3, 77, 52, 62), 0},
    { 5, Rect(14 + 64 * 4, 77, 52, 62), 0},
    { 6, Rect(14 + 64 * 5, 77, 52, 62), 0},
    { 7, Rect(14 + 64 * 6, 77, 52, 62), 0},
    { 8, Rect(14 + 64 * 7, 77, 52, 62), 0},
    { 9, Rect(14 + 64 * 8, 77, 52, 62), 0},
    {10, Rect(14 + 64 * 9, 77, 52, 62), 0},
    {11, Rect(14 + 64 * 10,77, 52, 62), 0},

    {12, Rect(14 + 64 * 0, 147, 52, 62), 0},
    {13, Rect(14 + 64 * 1, 147, 52, 62), 0},
    {14, Rect(14 + 64 * 2, 147, 52, 62), 0},
    {15, Rect(14 + 64 * 3, 147, 52, 62), 0},
    {16, Rect(14 + 64 * 4, 147, 52, 62), 0},
    {17, Rect(14 + 64 * 5, 147, 52, 62), 0},
    {18, Rect(14 + 64 * 6, 147, 52, 62), 0},
    {19, Rect(14 + 64 * 7, 147, 52, 62), 0},
    {20, Rect(14 + 64 * 8, 147, 52, 62), 0},
    {21, Rect(14 + 64 * 9, 147, 52, 62), 0},
    {22, Rect(14 + 64 * 10,147, 52, 62), 0},

    {23, Rect(14 + 64 * 2, 217, 52, 62), 0},
    {24, Rect(14 + 64 * 3, 217, 52, 62), 0},
    {25, Rect(14 + 64 * 4, 217, 52, 62), 0},
    {26, Rect(14 + 64 * 5, 217, 52, 62), 0},
    {27, Rect(14 + 64 * 6, 217, 52, 62), 0},
    {28, Rect(14 + 64 * 7, 217, 52, 62), 0},
    {29, Rect(14 + 64 * 8, 217, 52, 62), 0},
	
    {30, Rect(14 + 64 * 2, 287, 436, 62), 0}
};

/**
 * Callback areas for the special buttons on the CustomKeyboard.
 */
namespace KeyboardCallbacks
{
    enum KeyboardCallbacks
    {
	    CapsLock    = 0,
	    Backspace   = 1,
	    Mode        = 2,
	    Set         = 3,
	    Cancel      = 4
    };
};
static Keyboard::CallbackArea fullKeyboardCallbackAreas[5] =
{
    {128, Rect(14 + 64 * 0, 217, 116, 62), 0, u"caps",   0},     // caps-lock
    {129, Rect(14 + 64 * 9, 217, 116, 62), 0, 0,         BITMAP_BACKSPACE_ID},  // backspace
    {130, Rect(14 + 64 * 0, 287, 116, 62), 0, u"123",    0},       // mode
    {131, Rect(14,  380, 155, 62),         0, u"Set",    0},       // set
    {132, Rect(551, 380, 155, 62),         0, u"Cancel", 0},       // cancel
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
    .textAreaFont           =Typography::SANSSERIF28PX,
    .textAreaFontColor      =GETCOLORFROM24BITRGB(0, 0, 0),
    .textAreaBackColor      =GETCOLORFROM24BITRGB(239, 239, 239),
    .textAreaAlignment      = CENTER,
    .keyFont                =Typography::SANSSERIF40PX,
    .keyFontColor           =0,
    .keyBackColor           =GETCOLORFROM24BITRGB(239, 239, 239),
    .keyDownBackColor       =GETCOLORFROM24BITRGB(216, 216, 216),
    .keyAlpha               =0xFF,
	.shadowDepth            =5,
	.shadowColor            =GETCOLORFROM24BITRGB(180, 180, 180),
};

#endif
