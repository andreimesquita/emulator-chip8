#pragma once

#include "chip8/hardware/hardware.h"
#include "chip8/hardware/cpu/opcode.h"

struct CPU {
    void Init(Hardware* hardware);
    uint16_t ReadNextOpcode();
    void ExecNextOpcode();
    void Exec(uint16_t code);
    void UpdateTimers();
    void SkipNextBytes(uint16_t num);

    Timers Timers = {};
    Registers Registers = {};
    bool Halted = false;

private:
    void ExecExtended(const Opcode& opcode);
    void ExecExtended_8(const Opcode& opcode);
    void ExecExtended_F(const Opcode& opcode);

    Hardware* _hardware;
};
