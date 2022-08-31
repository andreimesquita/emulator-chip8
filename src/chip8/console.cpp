#include "console.h"

#include <cstdint>
#include <cassert>
#include <ctime>

#include "chip8/hardware/memory/memory.h"

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

Console::Console()
{
    hardware.Memory.WriteRaw(CHIP8_CHARACTER_SET_LOAD_ADDRESS, defaultCharacterSet, sizeof(defaultCharacterSet));
    srand(clock());
}

void Console::InsertCartridge(const Cartridge& outCartridge) {
    assert((outCartridge.size + CHIP8_PROGRAM_LOAD_ADDRESS) < CHIP8_MEMORY_SIZE);
    hardware.Memory.WriteRaw(CHIP8_PROGRAM_LOAD_ADDRESS, outCartridge.buffer, outCartridge.size);
    hardware.Registers.PC = CHIP8_PROGRAM_LOAD_ADDRESS;
}

void Console::Exec(uint16_t code) {
    switch (code) {

        case 0x00E0: { // CLS: Clear the display
            hardware.Screen.clear();
            hardware.Flags.Draw = true;
            break;
        }

        case 0x00EE: { // Ret: Return from subroutine
            hardware.Registers.PC = hardware.Stack.Pop();
            break;
        }

        default: {
            const Opcode opcode = Opcode(code);
            ExecExtended(opcode);
            break;
        }
    }
}

void Console::ExecExtended(const Opcode& opcode) {
    switch (opcode.Code & 0xF000) {
        // [1nnn] JP addr, 1nnn - Jump to location nnn's
        case 0x1000: {
            hardware.Registers.PC = opcode.NNN();
            break;
        }

            // [2nnn] CALL addr, 2nnn - Call subroutine at location nnn
        case 0x2000: {
            hardware.Stack.Push(hardware.Registers.PC);
            hardware.Registers.PC = opcode.NNN();
            break;
        }

            // [3xkk] SE Vx, byte - Skip next instruction if Vx == kk
        case 0x3000: {
            if (hardware.Registers.V[opcode.X()] == opcode.KK()) {
                hardware.Registers.PC += 2;
            }
            break;
        }

            // [4xkk] SNE Vx, byte - Skip next instruction if Vx != kk
        case 0x4000: {
            if (hardware.Registers.V[opcode.X()] != opcode.KK()) {
                hardware.Registers.PC += 2;
            }
            break;
        }

            // [5xy0] SE Vx, Vy - Skip next instruction if Vx == Vy
        case 0x5000: {
            if (hardware.Registers.V[opcode.X()] == hardware.Registers.V[opcode.Y()]) {
                hardware.Registers.PC += 2;
            }
            break;
        }

            // [6xkk] LD Vx, byte - Set Vx = kk
        case 0x6000: {
            hardware.Registers.V[opcode.X()] = opcode.KK();
            break;
        }

        case 0x7000: { // [7xkk] ADD Vx, byte - Set Vx = Vx + kk
            hardware.Registers.V[opcode.X()] += opcode.KK();
            break;
        }

        case 0x8000: { // [8xy(0...E)]
            ExecExtended_8(opcode);
            break;
        }

        case 0x9000: { // [9xy0] - SNE Vx, Vy - Skit next instruction if Vx != Vy
            if (hardware.Registers.V[opcode.X()] != hardware.Registers.V[opcode.Y()]) {
                hardware.Registers.PC += 2;
            }
            break;
        }

        case 0xA000: { // [Annn] - LD I, addr - Set I = nnn
            hardware.Registers.I = opcode.NNN();
            break;
        }

        case 0xB000: { // [Bnnn] - JP V0, addr - Jump to location nnn + V0
            hardware.Registers.PC = opcode.NNN() + hardware.Registers.V[0];
            break;
        }

        case 0xC000: { // [Cxkk] - RND Vx, byte - Set Vx = random byte AND kk
            hardware.Registers.V[opcode.X()] = rand() % 255 & opcode.KK();
            break;
        }

        case 0xD000: { // [Dxyn] - DRW Vx, Vy, nibble
            const uint8_t Vx = hardware.Registers.V[opcode.X()];
            const uint8_t Vy = hardware.Registers.V[opcode.Y()];
            const uint8_t* spritePtr = hardware.Memory.ReadRaw(hardware.Registers.I);

            const bool pixelCollision = hardware.Screen.drawSprite(Vx, Vy, spritePtr, opcode.N());
            hardware.Registers.V[REGISTER_CARRY_FLAG_INDEX] = pixelCollision;
            hardware.Flags.Draw = true;
            break;
        }

        case 0xE000:
        {
            switch (opcode.Code & 0x00FF)
            {
                case 0x9E: // [Ex9E] - SKP Vx - Skip the next instruction if key Vx is pressed
                    if (hardware.Keyboard.IsKeyDown(hardware.Registers.V[opcode.X()]))
                    {
                        hardware.Registers.PC += 2;
                    }
                    break;

                case 0xA1: // [ExA1] - SKNP Vx - Skip the next instruction if key Vx is not pressed
                    if (!hardware.Keyboard.IsKeyDown(hardware.Registers.V[opcode.X()]))
                    {
                        hardware.Registers.PC += 2;
                    }
                    break;
            }
            break;
        }

        case 0xF000: {
            ExecExtended_F(opcode);
            break;
        }
    }
}

void Console::ExecExtended_8(const Opcode& opcode) {
    switch (opcode.Code & 0x000F) {
        case 0x00: { // [8xy0] - LD Vx, Vy - Set Vx = Vy
            hardware.Registers.V[opcode.X()] = hardware.Registers.V[opcode.Y()];
            break;
        }

        case 0x01: { // [8xy1] - OR Vx, Vy - Set Vx = Vx OR Vy
            hardware.Registers.V[opcode.X()] |= hardware.Registers.V[opcode.Y()];
            break;
        }

        case 0x02: { // [8xy2] - AND Vx, Vy - Set Vx = Vx AND Vy
            hardware.Registers.V[opcode.X()] &= hardware.Registers.V[opcode.Y()];
            break;
        }

        case 0x03: { // [8xy3] - XOR Vx, Vy - Set Vx = Vx XOR Vy
            hardware.Registers.V[opcode.X()] ^= hardware.Registers.V[opcode.Y()];
            break;
        }

        case 0x04: { // [8xy4] - ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry
            const uint16_t tmp16 = hardware.Registers.V[opcode.X()] + hardware.Registers.V[opcode.Y()];
            hardware.Registers.V[REGISTER_CARRY_FLAG_INDEX] = tmp16 > 0xFF;
            hardware.Registers.V[opcode.X()] = tmp16;
            break;
        }

        case 0x05: { // [8xy5] - SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow
            hardware.Registers.V[REGISTER_CARRY_FLAG_INDEX] = hardware.Registers.V[opcode.X()] > hardware.Registers.V[opcode.Y()];
            hardware.Registers.V[opcode.X()] -= hardware.Registers.V[opcode.Y()];
            break;
        }

        case 0x06: { // [8xy6] - SHR Vx {, Vy}
            hardware.Registers.V[REGISTER_CARRY_FLAG_INDEX] = hardware.Registers.V[opcode.X()] & 0x01;
            hardware.Registers.V[opcode.X()] /= 2;
            break;
        }

        case 0x07: { // [8xy7] - SUBN Vx, Vy
            hardware.Registers.V[REGISTER_CARRY_FLAG_INDEX] = hardware.Registers.V[opcode.Y()] > hardware.Registers.V[opcode.X()];
            hardware.Registers.V[opcode.X()] = hardware.Registers.V[opcode.Y()] - hardware.Registers.V[opcode.X()];
            break;
        }

        case 0x0E: { // [8xyE] - SHL Vx {, Vy}
            hardware.Registers.V[REGISTER_CARRY_FLAG_INDEX] = (hardware.Registers.V[opcode.X()] & 0b1000'0000) >> 7;
            hardware.Registers.V[opcode.X()] *= 2;
            break;
        }
    }
}

void Console::ExecExtended_F(const Opcode& opcode) {
    switch (opcode.Code & 0x00FF) {
        case 0x07: // [Fx07] - LD Vx, DT - Set Vx = delay timer value
            hardware.Registers.V[opcode.X()] = hardware.Timers.Delay;
            break;

        case 0x0A: //[Fx0A] - LD Vx, K - Wait for a key press, store the value of the key in Vx
        {
            hardware.Flags.Halted = true;

            hardware.Keyboard.OnKeyDownEvent = [this, opcode](uint8_t vKey) {
                const uint8_t x = opcode.X();
                hardware.Registers.V[x] = vKey;

                hardware.Keyboard.OnKeyDownEvent = nullptr;
                hardware.Flags.Halted = false;
            };


            break;
        }

        case 0x15: // [Fx15] - LD DT, Vx - Set delay timer = Vx
            hardware.Timers.Delay = hardware.Registers.V[opcode.X()];
            break;

        case 0x18: // [Fx18] - LD ST, Vx - Set sound timer = Vx
            hardware.Timers.Sound = hardware.Registers.V[opcode.X()];
            break;

        case 0x1E: // [Fx1E] - ADD I, Vx - Set I = I + Vx
            hardware.Registers.I += hardware.Registers.V[opcode.X()];
            break;

        case 0x29: // [Fx29] - LD F, Vx - Set I = location of sprite for digit Vx
            hardware.Registers.I = hardware.Registers.V[opcode.X()] * CHIP8_DEFAULT_SPRITE_HEIGHT;
            break;

        case 0x33: //[Fx33] - LD B, Vx - Store BCD representation of Vx in Memory locations I, I+1, and I+2
        {
            const uint8_t hundreds = hardware.Registers.V[opcode.X()] / 100;
            const uint8_t tens = hardware.Registers.V[opcode.X()] / 10 % 10;
            const uint8_t units = hardware.Registers.V[opcode.X()] % 10;

            hardware.Memory.Write(hardware.Registers.I, hundreds);
            hardware.Memory.Write(hardware.Registers.I + 1, tens);
            hardware.Memory.Write(hardware.Registers.I + 2, units);
        }
            break;

        case 0x55: // [Fx55] - LD [I], Vx - Store Registers V0 through Vx in Memory starting at location I
            for (uint8_t i = 0u; i <= opcode.X(); i++) {
                const uint16_t address = hardware.Registers.I + i;
                hardware.Memory.Write(address, hardware.Registers.V[i]);
            }
            break;

        case 0x65: // [Fx65] - LD Vx, [I] - Read Registers V0 through Vx from Memory starting at location I
            for (uint8_t i = 0u; i <= opcode.X(); i++) {
                const uint16_t address = hardware.Registers.I + i;
                hardware.Registers.V[i] = hardware.Memory.Read(address);
            }
            break;
    }
}

void Console::Cycle() {
    if (hardware.Flags.Halted) return;

    hardware.Flags.Draw = false;
    hardware.Flags.Sound = false;

    for (uint32_t i = 0u; i < Config::Cpu::CyclesPerFrame; i++) {
        // read the next opcode
        const uint8_t byte1 = hardware.ReadNextByte();
        const uint8_t byte2 = hardware.ReadNextByte();
        const uint16_t opcode = (byte1 << 8) | byte2;

        // execute the instruction
        Exec(opcode);

        if (hardware.Flags.Halted) return;
    }

    // Decrement timers
    if (hardware.Timers.Delay > 0u) {
        hardware.Timers.Delay -= 1u;
    }

    if (hardware.Timers.Sound > 0u) {
        hardware.Timers.Sound -= 1u;
    }
    hardware.Flags.Sound = (hardware.Timers.Sound > 0u);
}
