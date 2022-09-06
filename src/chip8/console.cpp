#include "chip8/console.h"

#include <cstdint>
#include <cassert>
#include <ctime>

Console::Console()
{
    Cpu.Init(&Hardware);

    LoadDefaultCharacterSet();

    //TODO: Move this into another place
    srand(clock());
}

void Console::LoadDefaultCharacterSet() {
    const uint8_t defaultCharacterSet[] = {
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
    Hardware.Memory.WriteBuffer(CHIP8_CHARACTER_SET_LOAD_ADDRESS, defaultCharacterSet, sizeof(defaultCharacterSet));
}

void Console::InsertCartridge(const Cartridge& outCartridge) {
    assert((outCartridge.size + CHIP8_PROGRAM_LOAD_ADDRESS) < CHIP8_MEMORY_SIZE);
    Hardware.Memory.WriteBuffer(CHIP8_PROGRAM_LOAD_ADDRESS, outCartridge.buffer, outCartridge.size);
    Cpu.Registers.PC = CHIP8_PROGRAM_LOAD_ADDRESS;
}

void Console::Cycle() {
    if (Cpu.Halted) return;

    Hardware.Screen.Dirty = false;
    Hardware.Flags.Sound = false;

    for (uint32_t i = 0u; i < Config::Cpu::CyclesPerFrame; i++) {
        Cpu.ExecNextOpcode();
        if (Cpu.Halted) return;
    }

    Cpu.UpdateTimers();

    Hardware.Flags.Sound = (Cpu.Timers.Sound > 0u);
}
