#pragma once

#include <cstdint>
#include "chip8/constants.h"

struct Screen
{
    bool Pixels[CHIP8_SCREEN_HEIGHT][CHIP8_SCREEN_WIDTH] {};
    bool Dirty = false;

    void clear();
    void set(uint32_t x, uint32_t y);
    bool isSet(uint32_t x, uint32_t y) const;
    bool drawSprite(uint32_t x, uint32_t y, const uint8_t* sprite, int numBytes);
};