#pragma once

#include <cstdint>

#include "chip8/constants.h"

struct Registers
{
    uint8_t V[CHIP8_DATA_REGISTERS_SIZE] {};
    uint16_t I = 0u;
    uint16_t PC = 0u;
};
