#include "chip8/console.h"

#include <cstdint>
#include <cassert>
#include <ctime>

Console::Console()
{
    Cpu.Init(&Memory, &Stack, &Screen, &Keyboard);

    LoadDefaultCharacterSet();

    //TODO: Move this into another place
    srand(clock());
}

void Console::LoadDefaultCharacterSet() {
    constexpr uint8_t defaultCharacterSet[] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0,
            0x20, 0x60, 0x20, 0x20, 0x70,
            0xF0, 0x10, 0xF0, 0x80, 0xF0,
            0xF0, 0x10, 0xF0, 0x10, 0xF0,
            0x90, 0x90, 0xF0, 0x10, 0x10,
            0xF0, 0x80, 0xF0, 0x10, 0xF0,
            0xF0, 0x80, 0xF0, 0x90, 0xF0,
            0xF0, 0x10, 0x20, 0x40, 0x40,
            0xF0, 0x90, 0xF0, 0x90, 0xF0,
            0xF0, 0x90, 0xF0, 0x10, 0xF0,
            0xF0, 0x90, 0xF0, 0x90, 0x90,
            0xE0, 0x90, 0xE0, 0x90, 0xE0,
            0xF0, 0x80, 0x80, 0x80, 0xF0,
            0xE0, 0x90, 0x90, 0x90, 0xE0,
            0xF0, 0x80, 0xF0, 0x80, 0xF0,
            0xF0, 0x80, 0xF0, 0x80, 0x80
    };
    Memory.WriteBuffer(CHIP8_MEMORY_ADDRESS_CHARACTER_SET, defaultCharacterSet, sizeof(defaultCharacterSet));
}

void Console::InsertCartridge(const Cartridge& outCartridge) {
    assert((outCartridge.size + CHIP8_MEMORY_ADDRESS_PROGRAM_LOAD) < CHIP8_MEMORY_SIZE);
    Memory.WriteBuffer(CHIP8_MEMORY_ADDRESS_PROGRAM_LOAD, outCartridge.buffer, outCartridge.size);
    Cpu.PC = CHIP8_MEMORY_ADDRESS_PROGRAM_LOAD;
}

void Console::Cycle() {
    if (Cpu.Halted) return;

    Screen.Dirty = false;
    Cpu.Flags.Sound = false;

    for (uint32_t i = 0u; i < Config::Cpu::CyclesPerFrame; i++) {
        Cpu.ExecNextOpcode();
        if (Cpu.Halted) return;
    }

    Cpu.UpdateTimers();

    Cpu.Flags.Sound = (Cpu.Sound > 0u);
}
