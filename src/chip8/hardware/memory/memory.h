#pragma once

#include <cstdint>
#include <cstring>

#include "chip8/constants.h"

class Memory {
    uint8_t memory[CHIP8_MEMORY_SIZE];

public:
    void WriteBuffer(const uint16_t address, const uint8_t* source, const size_t size);

    void Write(const uint16_t address, const uint8_t value);

    uint8_t Read(const uint16_t address);

    uint8_t* GetPtr(const uint16_t address);
};
