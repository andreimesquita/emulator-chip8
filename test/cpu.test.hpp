#pragma once

//#include "utils/random.h"
#include "utils.hpp"

// Clear the display.
TEST_CASE("CLS") {
    Console console;

    // check if the Screen is clear
    ForEach<bool>(&console.screen.pixels[0][0], CHIP8_SCREEN_HEIGHT * CHIP8_SCREEN_WIDTH, [&](bool* pixel) {
        CHECK(*pixel == false);
    });

    // draw a sprite at the Screen
    console.screen.drawSprite(2, 2, &console.memory.memory[0x00], 5);

    // check if the Screen has some pixels set
    bool hasSomePixelsActivated = false;
    // check if the Screen is clear
    ForEach<bool>(&console.screen.pixels[0][0], CHIP8_SCREEN_HEIGHT * CHIP8_SCREEN_WIDTH, [&](bool* pixel) {
        hasSomePixelsActivated |= *pixel;
    });
    CHECK(hasSomePixelsActivated);

    // run the command that clears the Screen
    console.execInstruction(0x00E0);

    // check if the Screen has been cleared out
    ForEach<bool>(&console.screen.pixels[0][0], CHIP8_SCREEN_HEIGHT * CHIP8_SCREEN_WIDTH, [&](bool* pixel) {
        CHECK(*pixel == false);
    });
}

// Return from a subroutine.
// The interpreter sets the program counter to the address at the top of the stack,
// then subtracts 1 from the stack pointer.
TEST_CASE("RET") {
    Console console;
    console.registers.PC = 0x00;
    console.stack.push(0x11);
    console.execInstruction(0x00EE);
    CHECK(console.registers.PC == 0x11);
}

// Jump to location nnn.
// The interpreter sets the program counter to nnn.
TEST_CASE("JP addr") {
    Console console;
    console.registers.PC = 0x00;

    // JP FFF
    console.execInstruction(0x1FFF);
    CHECK(console.registers.PC == 0xFFF);

    // JP 000
    console.execInstruction(0x1000);
    CHECK(console.registers.PC == 0x000);
}

// Call subroutine at nnn.
// The interpreter increments the stack pointer, then puts the current PC on the top
// of the stack. The PC is then set to nnn.
TEST_CASE("CALL addr") {
    Console console;

    // check if the stack is empty
    console.registers.SP = 0x00;
    console.registers.PC = 0x123;

    // CALL FFF
    console.execInstruction(0x2FFF);

    // check if the stack was incremented
    CHECK(console.registers.SP == 1);

    // check if PC has the nnn
    CHECK(console.registers.PC == 0xFFF);

    // check if the previous PC value has been put into the stack
    CHECK(console.stack.pop() == 0x123);
}

// Skip next instruction if Vx = kk.
// The interpreter compares register Vx to kk, and if they are equal, increments the
// program counter by 2.
TEST_CASE("SE Vx, byte") {
    Console console;
    console.registers.PC = 0;
    console.registers.V[1] = 0x22;

    console.execInstruction(0x3121);

    // check if PC has the same value as before
    CHECK(console.registers.PC == 0);

    console.execInstruction(0x3122);

    // check if PC was incremented by 2
    CHECK(console.registers.PC == 2);
}

// Skip next instruction if Vx != kk.
// The interpreter compares register Vx to kk, and if they are not equal, increments
// the program counter by 2.
TEST_CASE("SNE Vx, byte") {
    Console console;
    console.registers.PC = 0;
    console.registers.V[1] = 0x21;

    console.execInstruction(0x4121);

    // check if PC has the same value as before
    CHECK(console.registers.PC == 0);

    console.execInstruction(0x4122);

    // check if PC was incremented by 2
    CHECK(console.registers.PC == 2);
}

// Skip next instruction if Vx = Vy.
// The interpreter compares register Vx to register Vy, and if they are equal, increments
// the program counter by 2.
TEST_CASE("SE Vx, Vy") {
    Console console;
    console.registers.PC = 0;
    console.registers.V[1] = 0x22;
    console.registers.V[2] = 0x21;

    console.execInstruction(0x5120);

    // check if PC has the same value as before
    CHECK(console.registers.PC == 0);

    console.registers.V[2] = 0x22;
    console.execInstruction(0x5120);

    // check if PC was incremented by 2
    CHECK(console.registers.PC == 2);
}

// Set Vx = kk.
// The interpreter puts the value kk into register Vx.
TEST_CASE("LD Vx, byte") {
    Console console;
    console.registers.V[5] = 0x00;
    console.execInstruction(0x6588);
    CHECK(console.registers.V[5] == 0x88);
}

// Set Vx = Vx + kk.
// Adds the value kk to the value of register Vx, then stores the result in Vx.
TEST_CASE("ADD Vx, byte") {
    Console console;
    console.registers.V[3] = 0x01;
    console.execInstruction(0x7309);
    CHECK(console.registers.V[3] == 0x0A);
}

// Set Vx = Vy.
// Stores the value of register Vy in register Vx.
TEST_CASE("LD Vx, Vy") {
    Console console;
    console.registers.V[1] = 0x66;
    console.registers.V[2] = 0x55;
    console.execInstruction(0x8120);
    CHECK(console.registers.V[1] == 0x55);
}

// Set Vx = Vx OR Vy.
// Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
// A bitwise OR compares the corrseponding bits from two values, and if either bit is 1,
// then the same bit in the result is also 1. Otherwise, it is 0.
TEST_CASE("OR Vx, Vy") {
    Console console;
    console.registers.V[1] = 0b1001;
    console.registers.V[2] = 0b0011;
    console.execInstruction(0x8121);
    CHECK(console.registers.V[1] == 0b1011);
}

//Set Vx = Vx AND Vy.
//
//Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
// A bitwise AND compares the corrseponding bits from two values, and if both bits are 1,
// then the same bit in the result is also 1. Otherwise, it is 0.
TEST_CASE("AND Vx, Vy") {
    Console console;
    console.registers.V[1] = 0b1001;
    console.registers.V[2] = 0b0011;
    console.execInstruction(0x8122);
    CHECK(console.registers.V[1] == 0b0001);
}

//Set Vx = Vx XOR Vy.
// Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
// An exclusive OR compares the corrseponding bits from two values, and if the bits are not
// both the same, then the corresponding bit in the result is set to 1. Otherwise, it is 0.
TEST_CASE("XOR Vx, Vy") {
    Console console;
    console.registers.V[1] = 0b1001;
    console.registers.V[2] = 0b0011;
    console.execInstruction(0x8123);
    CHECK(console.registers.V[1] == 0b1010);
}

// Set Vx = Vx + Vy, set VF = carry.
// The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,)
// VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
TEST_CASE("ADD Vx, Vy") {
    Console console;
    console.registers.V[REGISTER_CARRY_FLAG_INDEX] = 0;
    console.registers.V[0] = 200;
    console.registers.V[1] = 50;
    console.execInstruction(0x8014);
    CHECK(console.registers.V[0] == 250);
    CHECK(console.registers.V[REGISTER_CARRY_FLAG_INDEX] == 0);

    console.registers.V[REGISTER_CARRY_FLAG_INDEX] = 0;
    console.registers.V[0] = 200;
    console.registers.V[1] = 60;
    console.execInstruction(0x8014);
    CHECK(console.registers.V[0] == 4);
    CHECK(console.registers.V[REGISTER_CARRY_FLAG_INDEX] == 1);
}

// Set Vx = Vx - Vy, set VF = NOT borrow.
// If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results
// stored in Vx.
TEST_CASE("SUB Vx, Vy") {
    Console console;
    console.registers.V[REGISTER_CARRY_FLAG_INDEX] = 0;
    console.registers.V[0] = 200;
    console.registers.V[1] = 50;
    console.execInstruction(0x8015);
    CHECK(console.registers.V[0] == 150);
    CHECK(console.registers.V[REGISTER_CARRY_FLAG_INDEX] == 1);

    console.registers.V[REGISTER_CARRY_FLAG_INDEX] = 0;
    console.registers.V[0] = 200;
    console.registers.V[1] = 201;
    console.execInstruction(0x8015);
    CHECK(console.registers.V[0] == 255); // wrap-around behavior
    CHECK(console.registers.V[REGISTER_CARRY_FLAG_INDEX] == 0);
}

// Set Vx = Vx SHR 1.
// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is
// divided by 2.
TEST_CASE("SHR Vx, {, Vy}") {
    Console console;
    console.registers.V[REGISTER_CARRY_FLAG_INDEX] = 0;
    console.registers.V[1] = 0;
    console.execInstruction(0x8106);
    CHECK(console.registers.V[1] == 0);
    CHECK(console.registers.V[REGISTER_CARRY_FLAG_INDEX] == 0);

    console.registers.V[REGISTER_CARRY_FLAG_INDEX] = 0;
    console.registers.V[1] = 2;
    console.execInstruction(0x8106);
    CHECK(console.registers.V[1] == 1);
    CHECK(console.registers.V[REGISTER_CARRY_FLAG_INDEX] == 0);

    console.registers.V[REGISTER_CARRY_FLAG_INDEX] = 0;
    console.registers.V[1] = 7;
    console.execInstruction(0x8106);
    CHECK(console.registers.V[1] == 3);
    CHECK(console.registers.V[REGISTER_CARRY_FLAG_INDEX] == 1);
}

// Set Vx = Vy - Vx, set VF = NOT borrow.
// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the
// results stored in Vx.
TEST_CASE("SUBN Vx, Vy") {
    Console console;
    console.registers.V[REGISTER_CARRY_FLAG_INDEX] = 0;
    console.registers.V[1] = 6;
    console.registers.V[2] = 7;
    console.execInstruction(0x8127);
    CHECK(console.registers.V[1] == 1);
    CHECK(console.registers.V[REGISTER_CARRY_FLAG_INDEX] == 1);

    console.registers.V[REGISTER_CARRY_FLAG_INDEX] = 0;
    console.registers.V[1] = 6;
    console.registers.V[2] = 5;
    console.execInstruction(0x8127);
    CHECK(console.registers.V[1] == 255);
    CHECK(console.registers.V[REGISTER_CARRY_FLAG_INDEX] == 0);
}

// Set Vx = Vx SHL 1.
// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
TEST_CASE("SHL Vx {, Vy}") {
    Console console;
    console.registers.V[REGISTER_CARRY_FLAG_INDEX] = 0;
    console.registers.V[1] = 0x55;
    console.execInstruction(0x810E);
    CHECK(console.registers.V[1] == 0xAA);
    CHECK(console.registers.V[REGISTER_CARRY_FLAG_INDEX] == 0);

    console.registers.V[REGISTER_CARRY_FLAG_INDEX] = 0;
    console.registers.V[1] = 0x80;
    console.execInstruction(0x810E);
    CHECK(console.registers.V[1] == 0);
    CHECK(console.registers.V[REGISTER_CARRY_FLAG_INDEX] == 1);
}

// Skip next instruction if Vx != Vy.
// The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
TEST_CASE("SNE Vx, Vy") {
    Console console;
    console.registers.PC = 0;
    console.registers.V[2] = 1;
    console.registers.V[3] = 2;
    console.execInstruction(0x9230);
    CHECK(console.registers.PC == 2);

    console.registers.PC = 0;
    console.registers.V[2] = 1;
    console.registers.V[3] = 1;
    console.execInstruction(0x9230);
    CHECK(console.registers.PC == 0);
}

// Set I = nnn.
// The value of register I is set to nnn.
TEST_CASE("LD U, addr") {
    Console console;
    console.registers.I = 0;
    console.execInstruction(0xA123);
    CHECK(console.registers.I == 0x123);

    console.execInstruction(0xA010);
    CHECK(console.registers.I == 0x010);
}

// Jump to location nnn + V0.
// The program counter is set to nnn plus the value of V0.
TEST_CASE("JP V0, addr") {
    Console console;
    console.registers.V[0] = 0;
    console.execInstruction(0xB123);
    CHECK(console.registers.PC == 0x123);

    console.registers.V[0] = 1;
    console.execInstruction(0xB123);
    CHECK(console.registers.PC == 0x124);
}

// Set Vx = random byte AND kk.
// The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx. See instruction 8xy2 for more information on AND.
TEST_CASE("RND Vx, byte") {
    Console console;
    console.registers.V[0] = 0;
    srand(1234u); // set the random seed to be used by the test
    const uint8_t randomByte = rand() % 255;
    srand(1234u); // reset the random seed to be used by the CPU execInstruction

    console.execInstruction(0xC011);
    CHECK(console.registers.V[0] == (randomByte & 0x11));
}

//Display n-byte sprite starting at Memory location I at (Vx, Vy), set VF = collision.
//The interpreter reads n bytes from Memory, starting at the address stored in I. These bytes are then
// displayed as sprites on Screen at coordinates (Vx, Vy). Sprites are XORed onto the existing Screen.
// If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. If the sprite is
// positioned so part of it is outside the coordinates of the display, it wraps around to the opposite
// side of the Screen. See instruction 8xy3 for more information on XOR, and section 2.4, Display, for
// more information on the Chip-8 Screen and sprites.
TEST_CASE("Dxyn - DRW Vx, Vy, nibble") {
    Console console;
    ForEach<bool>(&console.screen.pixels[0][0], CHIP8_SCREEN_WIDTH * CHIP8_SCREEN_HEIGHT,
                  [&](const bool* pixel) {
                      CHECK(*pixel == false);
                  });

    // draw first sprite at 2,2
    console.registers.I = 0;
    console.registers.V[0] = 2;
    console.registers.V[1] = 2;
    console.execInstruction(0xD015);
    CHECK(console.registers.V[REGISTER_CARRY_FLAG_INDEX] == 0);
    CHECK(console.screen.pixels[2][2] == true);

    // draw second sprite at 20,20 (no pixel collision must occur)
    console.registers.I = 0;
    console.registers.V[0] = 20;
    console.registers.V[1] = 20;
    console.execInstruction(0xD015);
    CHECK(console.registers.V[REGISTER_CARRY_FLAG_INDEX] == 0);
    CHECK(console.screen.pixels[20][20] == true);

    // redraw first sprite at same location (pixel collision must occur)
    console.registers.I = 0;
    console.registers.V[0] = 2;
    console.registers.V[1] = 2;
    console.execInstruction(0xD015);
    CHECK(console.registers.V[REGISTER_CARRY_FLAG_INDEX] == 1);
    CHECK(console.screen.pixels[2][2] == false);
}

//Skip next instruction if key with the value of Vx is pressed.
//Checks the Keyboard, and if the key corresponding to the value of Vx is currently in the down position,
// PC is increased by 2.
TEST_CASE("Ex9E - SKP Vx") {
    Console console;

    console.registers.PC = 0;
    console.registers.V[3] = 0x01;
    console.execInstruction(0xE39E);
    CHECK(console.registers.PC == 0);

    console.registers.V[3] = 0x01;
    console.keyboard.setKeyDown(0x01);
    console.execInstruction(0xE39E);
    CHECK(console.registers.PC == 2);
}

//Skip next instruction if key with the value of Vx is not pressed.
//Checks the Keyboard, and if the key corresponding to the value of Vx is currently in the up position,
// PC is increased by 2.
TEST_CASE("ExA1 - SKNP Vx") {
    Console console;

    console.registers.PC = 0;
    console.registers.V[3] = 0x01;
    console.execInstruction(0xE3A1);
    CHECK(console.registers.PC == 2);

    console.registers.V[3] = 0x01;
    console.keyboard.setKeyDown(0x01);
    console.execInstruction(0xE3A1);
    CHECK(console.registers.PC == 2);
}

//Set Vx = delay timer value.
//The value of DT is placed into Vx.
TEST_CASE("Fx07 - LD Vx, DT") {
    Console console;
    console.registers.V[2] = 0x02;
    console.registers.delay_timer = 0x23;
    console.execInstruction(0xF207);
    CHECK(console.registers.V[2] == 0x23);
    CHECK(console.registers.delay_timer == 0x23);
}

//Wait for a key press, store the value of the key in Vx.
//All execution stops until a key is pressed, then the value of that key is stored in Vx.
TEST_CASE("Fx0A - LD Vx, K") {
    WaitForKeyPressEvent fakeWaitForKeyPress = []() { return 0x05; };
    Chip8ExternalCallbacks callbacks = {
            fakeWaitForKeyPress
    };
    Console console;
    console.setExternalCallbacks(callbacks);

    console.registers.V[4] = 0x00;
    console.execInstruction(0xF40A);
    CHECK(console.registers.V[4] == 0x05);
}

//Set delay timer = Vx.
//DT is set equal to the value of Vx.
TEST_CASE("Fx15 - LD DT, Vx") {
    Console console;
    console.registers.V[2] = 0x02;
    console.registers.delay_timer = 0x23;
    console.execInstruction(0xF215);
    CHECK(console.registers.delay_timer == 0x02);
}

//Set sound timer = Vx.
//ST is set equal to the value of Vx.
TEST_CASE("Fx18 - LD ST, Vx") {
    Console console;
    console.registers.V[2] = 0x02;
    console.registers.sound_timer = 0x23;
    console.execInstruction(0xF218);
    CHECK(console.registers.sound_timer == 0x02);
}

//Set I = I + Vx.
//The values of I and Vx are added, and the results are stored in I.
TEST_CASE("Fx1E - ADD I, Vx") {
    Console console;
    console.registers.I = 0x01;
    console.registers.V[7] = 0x02;
    console.execInstruction(0xF71E);
    CHECK(console.registers.I == 0x03);
}

//Set I = location of sprite for digit Vx.
//The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.
// See section 2.4, Display, for more information on the Chip-8 hexadecimal font.
TEST_CASE("Fx29 - LD F, Vx") {
    Console console;

    //TODO
    console.execInstruction(0xF329);
    //TODO
}

//Store BCD representation of Vx in Memory locations I, I+1, and I+2.
//The interpreter takes the decimal value of Vx, and places the hundreds digit in Memory at location in I,
// the tens digit at location I+1, and the ones digit at location I+2.
TEST_CASE("Fx33 - LD B, Vx") {
    Console console;

    console.registers.V[3] = 0xFE; // 254
    console.registers.I = 0x900;

    CHECK(console.memory.read(0x900) == 0x00);
    CHECK(console.memory.read(0x901) == 0x00);
    CHECK(console.memory.read(0x902) == 0x00);

    console.execInstruction(0xF333);

    CHECK(console.memory.read(0x900) == 0x02);
    CHECK(console.memory.read(0x901) == 0x05);
    CHECK(console.memory.read(0x902) == 0x04);
}

//Store Registers V0 through Vx in Memory starting at location I.
//The interpreter copies the values of Registers V0 through Vx into Memory, starting at the address in I.
TEST_CASE("Fx55 - LD [I], Vx") {
    Console console;

    console.registers.V[0] = 0xAA;
    console.registers.V[1] = 0xBB;
    console.registers.V[2] = 0xCC;
    console.registers.V[3] = 0x57;
    console.registers.I = 0x900;

    CHECK(console.memory.read(0x900) == 0x00);
    CHECK(console.memory.read(0x901) == 0x00);
    CHECK(console.memory.read(0x902) == 0x00);
    CHECK(console.memory.read(0x903) == 0x00);

    console.execInstruction(0xF355);

    CHECK(console.memory.read(0x900) == 0xAA);
    CHECK(console.memory.read(0x901) == 0xBB);
    CHECK(console.memory.read(0x902) == 0xCC);
    CHECK(console.memory.read(0x903) == 0x57);
}

//Read Registers V0 through Vx from Memory starting at location I.
//The interpreter reads values from Memory starting at location I into Registers V0 through Vx.
TEST_CASE("Fx65 - LD Vx, [I]") {
    Console console;

    console.registers.I = 0x900;
    console.registers.V[0] = 0x00;
    console.registers.V[1] = 0x00;
    console.registers.V[2] = 0x00;
    console.registers.V[3] = 0x00;

    console.memory.write(0x900, 0xAA);
    console.memory.write(0x901, 0xBB);
    console.memory.write(0x902, 0xCC);
    console.memory.write(0x903, 0x57);

    console.execInstruction(0xF365);

    CHECK(console.registers.V[0] == 0xAA);
    CHECK(console.registers.V[1] == 0xBB);
    CHECK(console.registers.V[2] == 0xCC);
    CHECK(console.registers.V[3] == 0x57);
}
