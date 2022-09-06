#include "cpu.h"

void CPU::Init(Hardware* hardware) {
    this->_hardware = hardware;
}

void CPU::UpdateTimers() {
    if (Timers.Delay > 0u) {
        Timers.Delay -= 1u;
    }

    if (Timers.Sound > 0u) {
        Timers.Sound -= 1u;
    }
}

void CPU::ExecNextOpcode() {
    const uint16_t code = ReadNextOpcode();
    Exec(code);
}

uint16_t CPU::ReadNextOpcode() {
// read the next opcode
    const uint8_t byte1 = _hardware->Memory.Read(Registers.PC);
    const uint8_t byte2 = _hardware->Memory.Read(Registers.PC + 1u);

    SkipNextBytes(2);

    const uint16_t opcode = (byte1 << 8) | byte2;
    return opcode;
}

void CPU::Exec(const uint16_t code) {
    switch (code) {
        case 0x00E0: { // CLS: Clear the display
            _hardware->Screen.clear();
            break;
        }
        case 0x00EE: { // Ret: Return from subroutine
            Registers.PC = _hardware->Stack.Pop();
            break;
        }
        default: {
            const Opcode opcode = Opcode(code);
            ExecExtended(opcode);
            break;
        }
    }
}

void CPU::ExecExtended(const Opcode &opcode) {
    switch (opcode.Code & 0xF000) {
        case 0x1000: // [1nnn] JP addr, 1nnn - Jump to location nnn's
            Registers.PC = opcode.NNN();
            break;

        case 0x2000: // [2nnn] CALL addr, 2nnn - Call subroutine at location nnn
            _hardware->Stack.Push(Registers.PC);
            Registers.PC = opcode.NNN();
            break;

        case 0x3000: // [3xkk] SE Vx, byte - Skip next instruction if Vx == kk
            if (Registers.V[opcode.X()] == opcode.KK()) {
                Registers.PC += 2;
            }
            break;

        case 0x4000: // [4xkk] SNE Vx, byte - Skip next instruction if Vx != kk
            if (Registers.V[opcode.X()] != opcode.KK()) {
                Registers.PC += 2;
            }
            break;

        case 0x5000: // [5xy0] SE Vx, Vy - Skip next instruction if Vx == Vy
            if (Registers.V[opcode.X()] == Registers.V[opcode.Y()]) {
                Registers.PC += 2;
            }
            break;

        case 0x6000: // [6xkk] LD Vx, byte - Set Vx = kk
            Registers.V[opcode.X()] = opcode.KK();
            break;

        case 0x7000: // [7xkk] ADD Vx, byte - Set Vx = Vx + kk
            Registers.V[opcode.X()] += opcode.KK();
            break;

        case 0x8000: // [8xy(0...E)]
            ExecExtended_8(opcode);
            break;

        case 0x9000: // [9xy0] - SNE Vx, Vy - Skit next instruction if Vx != Vy
            if (Registers.V[opcode.X()] != Registers.V[opcode.Y()]) {
                Registers.PC += 2;
            }
            break;

        case 0xA000: // [Annn] - LD I, addr - Set I = nnn
            Registers.I = opcode.NNN();
            break;

        case 0xB000: // [Bnnn] - JP V0, addr - Jump to location nnn + V0
            Registers.PC = opcode.NNN() + Registers.V[0];
            break;

        case 0xC000: // [Cxkk] - RND Vx, byte - Set Vx = random byte AND kk
            Registers.V[opcode.X()] = rand() % 255 & opcode.KK();
            break;

        case 0xD000: { // [Dxyn] - DRW Vx, Vy, nibble
            const uint8_t Vx = Registers.V[opcode.X()];
            const uint8_t Vy = Registers.V[opcode.Y()];
            const uint8_t* spritePtr = _hardware->Memory.GetPtr(Registers.I);

            const bool pixelCollision = _hardware->Screen.drawSprite(Vx, Vy, spritePtr, opcode.N());
            Registers.V[REGISTER_CARRY_FLAG_INDEX] = pixelCollision;
            break;
        }
        case 0xE000: {
            switch (opcode.Code & 0x00FF) {
                case 0x9E: // [Ex9E] - SKP Vx - Skip the next instruction if key Vx is pressed
                    if (_hardware->Keyboard.IsKeyDown(Registers.V[opcode.X()])) {
                        Registers.PC += 2;
                    }
                    break;

                case 0xA1: // [ExA1] - SKNP Vx - Skip the next instruction if key Vx is not pressed
                    if (!_hardware->Keyboard.IsKeyDown(Registers.V[opcode.X()])) {
                        Registers.PC += 2;
                    }
                    break;
            }
            break;
        }
        case 0xF000:
            ExecExtended_F(opcode);
            break;
    }
}

void CPU::ExecExtended_8(const Opcode &opcode) {
    switch (opcode.Code & 0x000F) {
        case 0x0: { // [8xy0] - LD Vx, Vy - Set Vx = Vy
            Registers.V[opcode.X()] = Registers.V[opcode.Y()];
            break;
        }
        case 0x1: { // [8xy1] - OR Vx, Vy - Set Vx = Vx OR Vy
            Registers.V[opcode.X()] |= Registers.V[opcode.Y()];
            break;
        }
        case 0x2: { // [8xy2] - AND Vx, Vy - Set Vx = Vx AND Vy
            Registers.V[opcode.X()] &= Registers.V[opcode.Y()];
            break;
        }
        case 0x3: { // [8xy3] - XOR Vx, Vy - Set Vx = Vx XOR Vy
            Registers.V[opcode.X()] ^= Registers.V[opcode.Y()];
            break;
        }
        case 0x4: { // [8xy4] - ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry
            const uint16_t tmp16 = Registers.V[opcode.X()] + Registers.V[opcode.Y()];
            Registers.V[REGISTER_CARRY_FLAG_INDEX] = tmp16 > 0xFF;
            Registers.V[opcode.X()] = tmp16;
            break;
        }
        case 0x5: { // [8xy5] - SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow
            Registers.V[REGISTER_CARRY_FLAG_INDEX] = Registers.V[opcode.X()] > Registers.V[opcode.Y()];
            Registers.V[opcode.X()] -= Registers.V[opcode.Y()];
            break;
        }
        case 0x6: { // [8xy6] - SHR Vx {, Vy}
            Registers.V[REGISTER_CARRY_FLAG_INDEX] = Registers.V[opcode.X()] & 0x01;
            Registers.V[opcode.X()] /= 2;
            break;
        }
        case 0x7: { // [8xy7] - SUBN Vx, Vy
            Registers.V[REGISTER_CARRY_FLAG_INDEX] = Registers.V[opcode.Y()] > Registers.V[opcode.X()];
            Registers.V[opcode.X()] = Registers.V[opcode.Y()] - Registers.V[opcode.X()];
            break;
        }
        case 0xE: { // [8xyE] - SHL Vx {, Vy}
            Registers.V[REGISTER_CARRY_FLAG_INDEX] = (Registers.V[opcode.X()] & 0b1000'0000) >> 7;
            Registers.V[opcode.X()] *= 2;
            break;
        }
    }
}

void CPU::ExecExtended_F(const Opcode &opcode) {
    switch (opcode.Code & 0x00FF) {
        case 0x07: { // [Fx07] - LD Vx, DT - Set Vx = delay timer value
            Registers.V[opcode.X()] = Timers.Delay;
            break;
        }
        case 0x0A: { //[Fx0A] - LD Vx, K - Wait for a key press, store the value of the key in Vx
            Halted = true;

            _hardware->Keyboard.OnKeyDownEvent = [this, opcode](uint8_t vKey) {
                const uint8_t x = opcode.X();
                this->Registers.V[x] = vKey;

                _hardware->Keyboard.OnKeyDownEvent = nullptr;
                Halted = false;
            };
            break;
        }
        case 0x15: { // [Fx15] - LD DT, Vx - Set delay timer = Vx
            Timers.Delay = Registers.V[opcode.X()];
            break;
        }
        case 0x18: { // [Fx18] - LD ST, Vx - Set sound timer = Vx
            Timers.Sound = Registers.V[opcode.X()];
            break;
        }
        case 0x1E: { // [Fx1E] - ADD I, Vx - Set I = I + Vx
            Registers.I += Registers.V[opcode.X()];
            break;
        }
        case 0x29: { // [Fx29] - LD F, Vx - Set I = location of sprite for digit Vx
            Registers.I = Registers.V[opcode.X()] * CHIP8_DEFAULT_SPRITE_HEIGHT;
            break;
        }
        case 0x33: { //[Fx33] - LD B, Vx - Store BCD representation of Vx in Memory locations I, I+1, and I+2
            const uint8_t hundreds = Registers.V[opcode.X()] / 100;
            const uint8_t tens = Registers.V[opcode.X()] / 10 % 10;
            const uint8_t units = Registers.V[opcode.X()] % 10;

            _hardware->Memory.Write(Registers.I, hundreds);
            _hardware->Memory.Write(Registers.I + 1, tens);
            _hardware->Memory.Write(Registers.I + 2, units);
            break;
        }
        case 0x55: { // [Fx55] - LD [I], Vx - Store Registers V0 through Vx in Memory starting at location I
            for (uint8_t i = 0u; i <= opcode.X(); i++) {
                const uint16_t address = Registers.I + i;
                _hardware->Memory.Write(address, Registers.V[i]);
            }
            break;
        }
        case 0x65: { // [Fx65] - LD Vx, [I] - Read Registers V0 through Vx from Memory starting at location I
            for (uint8_t i = 0u; i <= opcode.X(); i++) {
                const uint16_t address = Registers.I + i;
                Registers.V[i] = _hardware->Memory.Read(address);
            }
            break;
        }
    }
}

void CPU::SkipNextBytes(const uint16_t num) {
    Registers.PC += num;
}
