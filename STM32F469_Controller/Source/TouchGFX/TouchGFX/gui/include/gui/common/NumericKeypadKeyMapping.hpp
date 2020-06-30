#ifndef NUMERICKEYPAD_KEY_MAPPING_H
#define NUMERICKEYPAD_KEY_MAPPING_H

#include <touchgfx/widgets/Keyboard.hpp>
using namespace touchgfx;

/**
 * This file contains all the keymappings for the CustomKeyboard.
 */
static const Keyboard::KeyMapping numericKeypadKeyMappings[30] =
{
    { 1, 48},  // 0
    { 2, 49},  // 1
    { 3, 50},  // 2
    { 4, 51},  // 3
    { 5, 52},  // 4
    { 6, 53},  // 5
    { 7, 54},  // 6
    { 8, 55},  // 7
    { 9, 56},  // 8
    {10, 57},  // 9
    {11, 65},  // A
    {12, 66},  // B
    {13, 67},  // C
    {14, 68},  // D
    {15, 69},  // E
    {16, 70},  // F
};

static const Keyboard::KeyMappingList numericKeypadKeyMappingList =
{
    numericKeypadKeyMappings,
    16
};


#endif
