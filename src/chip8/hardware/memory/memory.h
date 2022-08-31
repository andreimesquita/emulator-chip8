#pragma once

#include <cstdint>
#include <cstring>

#include "chip8/constants.h"

class Memory {
    uint8_t memory[CHIP8_MEMORY_SIZE];

public:
    void WriteRaw(const uint16_t address, const uint8_t* source, const size_t size) {
        memcpy(&memory[address], source, size);
    }

    void Write(const uint16_t address, const uint8_t value) {
        memory[address] = value;
    }

    uint8_t Read(const uint16_t address) {
        const uint8_t value = memory[address];
        return value;
    }

    uint8_t* ReadRaw(const uint16_t address) {
        return &memory[address];
    }
};
