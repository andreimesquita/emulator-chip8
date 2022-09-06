cmake_minimum_required(VERSION 3.23)

project(emu_chip_8)

set(CMAKE_CXX_STANDARD 14)

# Set up directories to include (src, include and libs)
include_directories(${PROJECT_SOURCE_DIR})
set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
link_directories(${LIBS_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

# Generate STATIC LIB
add_library(chip8_core_lib
        # Emulator
        ${SOURCE_DIR}/chip8/constants.cpp
        ${SOURCE_DIR}/chip8/console.cpp
        # Cartridge
        ${SOURCE_DIR}/chip8/cartridge/cartridge.cpp
        # Hardware
        ${SOURCE_DIR}/chip8/hardware/cpu/registers.cpp
        ${SOURCE_DIR}/chip8/hardware/cpu/timers.h
        ${SOURCE_DIR}/chip8/hardware/hardware.cpp
        ${SOURCE_DIR}/chip8/hardware/cpu/opcode.cpp
        ${SOURCE_DIR}/chip8/hardware/memory/stack.cpp
        ${SOURCE_DIR}/chip8/hardware/memory/memory.cpp
        ${SOURCE_DIR}/chip8/hardware/IO/keyboard.cpp
        ${SOURCE_DIR}/chip8/hardware/IO/screen.cpp
        ${SOURCE_DIR}/chip8/hardware/cpu/cpu.cpp
)
# Creates a custom command that copies the library file from the build directory into the project's LIBS folder
add_custom_command(
        TARGET chip8_core_lib
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/chip8_core_lib.lib # the file to copy
        ${LIBS_DIR}/chip8/chip8_core.lib       # the destination
)

# add executable for SDL2 integration
add_executable(${PROJECT_NAME}
        # SDL Front-end
        ${PROJECT_SOURCE_DIR}/main.cpp
        ${SOURCE_DIR}/sdl/emulator.cpp
        ${SOURCE_DIR}/sdl/beeper.cpp
)
# Link SDL2 library
target_link_libraries(${PROJECT_NAME} SDL2/SDL2main SDL2/SDL2 chip8/chip8_core)
