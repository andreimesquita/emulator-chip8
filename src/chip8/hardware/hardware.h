#pragma once

#include "cpu/timers.h"

struct Hardware {
    struct {
        bool Halted = false;
        bool Draw = false;
        bool Sound = false;
    } Flags;

    Memory Memory {};
    Stack Stack {};
    Timers Timers {};
    Registers Registers {};
    VirtualKeyboard Keyboard {};
    Screen Screen {};

    uint8_t ReadNextByte() {
        const uint8_t byte = Memory.Read(Registers.PC);
        SkipNextBytes(1);
        return byte;
    }

    void SkipNextBytes(const uint16_t num) {
        Registers.PC += num;
    }
};
