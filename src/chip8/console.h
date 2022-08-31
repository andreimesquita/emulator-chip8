#pragma once

#include <cstdint>

#include "chip8/constants.h"
#include "chip8/cartridge/cartridge.h"
#include "chip8/hardware/memory/stack.h"
#include "chip8/hardware/memory/memory.h"
#include "chip8/hardware/cpu/registers.h"
#include "chip8/hardware/io/keyboard.h"
#include "chip8/hardware/screen.h"
#include "chip8/hardware/hardware.h"
#include "chip8/hardware/cpu/opcode.h"

struct Console
{
    Console();

    void InsertCartridge(const Cartridge& outCartridge);
    void Exec(uint16_t code);
    void Cycle();

    Hardware hardware {};

protected:
    void ExecExtended(const Opcode& opcode);
    void ExecExtended_8(const Opcode& opcode);
    void ExecExtended_F(const Opcode& opcode);
};
