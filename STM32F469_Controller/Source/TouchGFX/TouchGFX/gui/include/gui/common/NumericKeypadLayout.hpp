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
    { 1, Rect(15 + 80 * 0, 85, 60, 60), 0},
    { 2, Rect(15 + 80 * 1, 85, 60, 60), 0},
    { 3, Rect(15 + 80 * 2, 85, 60, 60), 0},
    { 4, Rect(15 + 80 * 3, 85, 60, 60), 0},

    { 5, Rect(15 + 80 * 0, 155, 60, 60), 0},
    { 6, Rect(15 + 80 * 1, 155, 60, 60), 0},
    { 7, Rect(15 + 80 * 2, 155, 60, 60), 0},
    { 8, Rect(15 + 80 * 3, 155, 60, 60), 0},

    { 9, Rect(15 + 80 * 0, 225, 60, 60), 0},
    {10, Rect(15 + 80 * 1, 225, 60, 60), 0},
    {11, Rect(15 + 80 * 2, 225, 60, 60), 0},
    {12, Rect(15 + 80 * 3, 225, 60, 60), 0},

    {13, Rect(15 + 80 * 0, 295, 60, 60), 0},
    {14, Rect(15 + 80 * 1, 295, 60, 60), 0},
    {15, Rect(15 + 80 * 2, 295, 60, 60), 0},
    {16, Rect(15 + 80 * 3, 295, 60, 60), 0},
};

/**
 * Callback areas for the special buttons on the CustomKeyboard.
 */
namespace NumericCallbacks
{
    enum NumericCallbacks
    {
	    Clear  = 0,
	    Mode   = 1,
	    Cancel = 2,
	    Set    = 3
    };
};
static Keyboard::CallbackArea NumericKeypadCallbackAreas[5] =
{
    {128, Rect(15 + 80 * 4, 85,  115, 60), 0, u"Clear",  0},      // Clear
    {129, Rect(15 + 80 * 4, 295, 115, 60), 0, 0,         0},     // Mode toggle
    {130, Rect(15, 365, 150, 72),          0, u"Cancel", 0},       // cancel
    {131, Rect(300,365, 150, 72),          0, u"Set",    0},       // set
 };

/**
 * The layout for the CustomKeyboard.
 */
static const Keyboard::Layout numericKeypadLayout =
{
	.backColor              = GETCOLORFROM24BITRGB(216, 216, 216),
	.backAlpha              = 0xFF,
    .bitmap                 = 0,
    .keyArray               = NumericKeypadKeyArray,
    .numberOfKeys           = 30,
    .callbackAreaArray      = NumericKeypadCallbackAreas,
    .numberOfCallbackAreas  = 5,
    .textAreaPosition       = Rect(250, 15, 200, 50),
    .textAreaFont           = Typography::TYPOGRAPHY_NUMERIC_40PX,
    .textAreaFontColor      = GETCOLORFROM24BITRGB(0, 0, 0),
    .textAreaBackColor      = GETCOLORFROM24BITRGB(239, 239, 239),
	.textAreaAlignment      = RIGHT,
    .keyFont                = Typography::TYPOGRAPHY_40PX,
    .keyFontColor           = 0,
    .keyBackColor           = GETCOLORFROM24BITRGB(239, 239, 239),
    .keyDownBackColor       = GETCOLORFROM24BITRGB(216, 216, 216),
    .keyAlpha               = 0xFF,
	.shadowDepth            = 5,
	.shadowColor            = GETCOLORFROM24BITRGB(180, 180, 180),
    .titlePosition          = Rect(15, 40, 235, 50),
    .titleFont              = Typography::TYPOGRAPHY_40PX,
    .titleFontColor         = GETCOLORFROM24BITRGB(0, 0, 0),
	.titleAlignment         = LEFT,
};

#endif
