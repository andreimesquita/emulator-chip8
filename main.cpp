#include <cstdio>
#include <cassert>

#include "sdl/emulator.h"

int main (int argc, char* argv[]) {
    assert(argc >= 2);

    char* filePath = argv[1];

    Emulator emulator = {};
    emulator.Run(filePath);

    return 0;
}
