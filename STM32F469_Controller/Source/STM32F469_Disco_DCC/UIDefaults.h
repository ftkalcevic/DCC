#pragma once

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/Color.hpp>
#include <fonts/ApplicationFontProvider.hpp>

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




constexpr uint32_t              DefaultTextColour = GETCOLORFROM24BITRGB(0, 0, 0);
constexpr uint32_t              DefaultDisabledTextColour = GETCOLORFROM24BITRGB(128, 128, 128);
constexpr touchgfx::Alignment   DefaultAlignment = touchgfx::LEFT;
constexpr touchgfx::FontId      DefaultFontId = Typography::SANSSERIF40PX;         
constexpr uint32_t              DefaultBoxColor = GETCOLORFROM24BITRGB(163, 163, 163);
constexpr uint32_t              DefaultDisabledBoxColor = GETCOLORFROM24BITRGB(163, 163, 163);
constexpr uint8_t               DefaultAlpha = 255;

