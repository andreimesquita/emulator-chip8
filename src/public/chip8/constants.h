#pragma once

#include <cstdint>

// window
#define EMULATOR_WINDOW_TITLE "CHIP-8 Emulator"

// cpu => Registers
#define CHIP8_DATA_REGISTERS_SIZE 16
#define REGISTER_CARRY_FLAG_INDEX 0x0F
#define CHIP8_KEYS_SIZE 16

// window
#define CHIP8_SCREEN_WIDTH 64
#define CHIP8_SCREEN_HEIGHT 32

// sprite
#define CHIP8_DEFAULT_SPRITE_HEIGHT 5

#define CHIP8_MEMORY_SIZE 0x1000
#define CHIP8_MEMORY_STACK_SIZE 0x10

#define CHIP8_MEMORY_ADDRESS_PROGRAM_LOAD 0x200
#define CHIP8_MEMORY_ADDRESS_CHARACTER_SET 0x00

namespace Config {
    namespace Cpu {
        extern uint32_t CyclesPerFrame;
        extern double FrameTime;
    }
    namespace Screen {
        extern uint32_t SizeMultiplier;
        extern uint32_t Padding;
    }
}
