#include "catch2/catch_all.hpp"

#include "utils.hpp"
#include "chip8/console.h"

TEST_CASE("Screen.clear()") {
    Console console;

    // check if the Screen is clear
    ForEach<bool>(&console.screen.pixels[0][0], CHIP8_SCREEN_HEIGHT * CHIP8_SCREEN_WIDTH, [&](bool* pixel) {
        CHECK(*pixel == false);
    });
}
