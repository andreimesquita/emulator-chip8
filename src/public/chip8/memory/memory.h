#pragma once

#include <cstdint>
#include <cstring>

#include "chip8/constants.h"

class Memory {
    uint8_t memory[CHIP8_MEMORY_SIZE];

public:
    void WriteBuffer(uint16_t address, const uint8_t* source, size_t size);

    void Write(uint16_t address, const uint8_t value);

    uint8_t Read(uint16_t address);

    uint8_t* GetPtr(uint16_t address);
};
