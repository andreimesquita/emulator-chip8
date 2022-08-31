#pragma once

#include <cstdint>
#include "chip8/constants.h"

struct Screen
{
    bool pixels[CHIP8_SCREEN_HEIGHT][CHIP8_SCREEN_WIDTH];

    void clear();
    void set(uint32_t x, uint32_t y);
    bool isSet(uint32_t x, uint32_t y) const;
    bool drawSprite(uint32_t x, uint32_t y, const uint8_t* sprite, int numBytes);
};