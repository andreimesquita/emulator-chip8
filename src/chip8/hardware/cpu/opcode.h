#pragma once

#include <cstdint>

struct Opcode {
    const uint16_t Code;

    explicit Opcode(const uint16_t code) : Code(code) { }

    uint8_t X() const {
        const uint8_t x = (Code & 0x0F00) >> 8;
        return x;
    }

    uint8_t Y() const {
        const uint8_t y = (Code & 0x00F0) >> 4;
        return y;
    }

    uint16_t NNN() const {
        const uint16_t nnn = Code & 0x0FFF;
        return nnn;
    }

    uint8_t KK() const {
        const uint8_t kk = Code & 0x00FF;
        return kk;
    }

    uint8_t N() const {
        const uint8_t n = Code & 0x000F;
        return n;
    }
};
