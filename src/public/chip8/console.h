#pragma once

#include <cstdint>

#include "constants.h"
#include "chip8/cartridge/cartridge.h"
#include "chip8/cpu/cpu.h"
#include "chip8/memory/memory.h"
#include "chip8/memory/stack.h"
#include "chip8/IO/keyboard.h"
#include "chip8/IO/screen.h"

struct Console
{
    Console();

    void LoadDefaultCharacterSet();
    void InsertCartridge(const Cartridge& outCartridge);
    void Cycle();

    Memory Memory {};
    Stack Stack {};
    Keyboard Keyboard {};
    Screen Screen {};
    CPU Cpu {};
};
