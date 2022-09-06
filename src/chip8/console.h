#pragma once

#include <cstdint>

#include "constants.h"
#include "chip8/cartridge/cartridge.h"
#include "chip8/hardware/hardware.h"
#include "chip8/hardware/cpu/cpu.h"

struct Console
{
    Console();

    void LoadDefaultCharacterSet();
    void InsertCartridge(const Cartridge& outCartridge);
    void Cycle();

    Hardware Hardware {};
    CPU Cpu {};
};
