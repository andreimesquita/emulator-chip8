#include "chip8/hardware/memory/memory.h"

void Memory::WriteBuffer(const uint16_t address, const uint8_t* source, const size_t size) {
    memcpy(&memory[address], source, size);
}

void Memory::Write(const uint16_t address, const uint8_t value) {
    memory[address] = value;
}

uint8_t Memory::Read(const uint16_t address) {
    const uint8_t value = memory[address];
    return value;
}

uint8_t* Memory::GetPtr(const uint16_t address) {
    return &memory[address];
}
