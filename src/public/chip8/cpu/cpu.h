#pragma once

#include "opcode.h"
#include "chip8/constants.h"
#include "chip8/IO/keyboard.h"
#include "chip8/IO/screen.h"
#include "chip8/memory/memory.h"
#include "chip8/memory/stack.h"

struct CPU {
    void Init(Memory* memory, Stack* stack, Screen* Screen, Keyboard* keyboard);
    uint16_t ReadNextOpcode();
    void ExecNextOpcode();
    void Exec(uint16_t code);
    void UpdateTimers();
    void SkipNextBytes(uint16_t num);

    bool Halted = false;

    struct {
        bool Draw = false;
        bool Sound = false;
    } Flags;

    // Timers
    uint8_t Delay = 0u;
    uint8_t Sound = 0u;

    // Registers
    uint8_t V[CHIP8_DATA_REGISTERS_SIZE] {};
    uint16_t I = 0u;
    uint16_t PC = 0u;

private:
    void ExecExtended(const Opcode& opcode);
    void ExecExtended_8(const Opcode& opcode);
    void ExecExtended_F(const Opcode& opcode);

    // Hardware components
    Keyboard* _keyboard = nullptr;
    Screen* _screen = nullptr;
    Memory* _memory = nullptr;
    Stack* _stack = nullptr;
};
