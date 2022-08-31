cmake_minimum_required(VERSION 3.23)
project(emu_chip_8)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

set(SDL2_INCLUDE_DIR C:/Projects/SDL2/include)
include_directories(${SDL2_INCLUDE_DIR})

set(SDL2_LIB_DIR C:/Projects/SDL2/lib/x64)
link_directories(${SDL2_LIB_DIR})

set(sources
        # cartridge
        src/chip8/cartridge/cartridge.cpp

        # hardware
        src/chip8/hardware/cpu/registers.h
        src/chip8/hardware/cpu/opcode.h
        src/chip8/hardware/cpu/timers.h
        src/chip8/hardware/io/keyboard.cpp
        src/chip8/hardware/memory/stack.cpp
        src/chip8/hardware/memory/memory.h
        src/chip8/hardware/screen.cpp
        src/chip8/hardware/hardware.h

        # console
        src/chip8/constants.cpp
        src/chip8/console.cpp

        # SDL
        src/sdl/beeper.cpp
        main.cpp src/sdl/emulator.h)
add_executable(emu_chip_8 ${sources})

# Link SDL2 libraries
target_link_libraries(emu_chip_8 SDL2main SDL2)
