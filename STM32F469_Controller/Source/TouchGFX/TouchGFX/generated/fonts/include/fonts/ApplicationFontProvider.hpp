/* DO NOT EDIT THIS FILE */
/* This file is autogenerated by the text-database code generator */

#ifndef APPLICATIONFONTPROVIDER_HPP
#define APPLICATIONFONTPROVIDER_HPP

#include <touchgfx/FontManager.hpp>

struct Typography
{
    static const touchgfx::FontId DEFAULT = 0;
    static const touchgfx::FontId TYPOGRAPHY_00 = 1;
};

struct TypographyFontIndex
{
    static const touchgfx::FontId DEFAULT = 0;       // Asap_Regular_80_4bpp
    static const touchgfx::FontId TYPOGRAPHY_00 = 1; // Asap_Regular_20_4bpp
    static const uint16_t NUMBER_OF_FONTS = 2;
};

class ApplicationFontProvider : public touchgfx::FontProvider
{
public:
    virtual touchgfx::Font* getFont(touchgfx::FontId typography);
};

#endif // APPLICATIONFONTPROVIDER_HPP
