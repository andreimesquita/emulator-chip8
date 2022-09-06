#include "chip8/hardware/cpu/opcode.h"

Opcode::Opcode(const uint16_t code) : Code(code) { }

uint8_t Opcode::X() const {
    const uint8_t x = (Code & 0x0F00) >> 8;
    return x;
}

uint8_t Opcode::Y() const {
    const uint8_t y = (Code & 0x00F0) >> 4;
    return y;
}

uint8_t Opcode::N() const {
    const uint8_t n = Code & 0x000F;
    return n;
}

uint8_t Opcode::KK() const {
    const uint8_t kk = Code & 0x00FF;
    return kk;
}

uint16_t Opcode::NNN() const {
    const uint16_t nnn = Code & 0x0FFF;
    return nnn;
}
