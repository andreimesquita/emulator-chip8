#pragma once

#include <cstdint>

struct Opcode {
    const uint16_t Code;

    explicit Opcode(uint16_t code);

    uint8_t X() const;
    uint8_t Y() const;
    uint8_t N() const;
    uint8_t KK() const;
    uint16_t NNN() const;
};
