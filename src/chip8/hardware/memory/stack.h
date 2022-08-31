#pragma once

#include <cstdint>

#include "chip8/constants.h"

struct Stack
{
    void Push(uint16_t value);
    uint16_t Pop();

    uint8_t SP = 0u;

private:
    uint16_t stack[CHIP8_STACK_SIZE] {};
};
