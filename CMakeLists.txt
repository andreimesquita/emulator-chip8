cmake_minimum_required(VERSION 3.23)

project(emu_chip_8)
set(CMAKE_CXX_STANDARD 14)

# Set up directories
set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)
link_directories(${LIBS_DIR})

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
include_directories(${INCLUDE_DIR})

set(SRC_PUBLIC_DIR ${PROJECT_SOURCE_DIR}/src/public)
set(SRC_PRIVATE_DIR ${PROJECT_SOURCE_DIR}/src/private)
include_directories(${SRC_PUBLIC_DIR})
include_directories(${SRC_PRIVATE_DIR})

# Generate STATIC LIB
add_library(chip8_core_lib
        # Emulator
        ${SRC_PRIVATE_DIR}/chip8/constants.cpp
        ${SRC_PRIVATE_DIR}/chip8/console.cpp
        # Cartridge
        ${SRC_PRIVATE_DIR}/chip8/cartridge/cartridge.cpp
        # Hardware
        ${SRC_PRIVATE_DIR}/chip8/cpu/opcode.cpp
        ${SRC_PRIVATE_DIR}/chip8/memory/stack.cpp
        ${SRC_PRIVATE_DIR}/chip8/memory/memory.cpp
        ${SRC_PRIVATE_DIR}/chip8/IO/keyboard.cpp
        ${SRC_PRIVATE_DIR}/chip8/IO/screen.cpp
        ${SRC_PRIVATE_DIR}/chip8/cpu/cpu.cpp
)
# Creates a custom command that copies the library file from the build directory into the project's LIBS folder
add_custom_command(
        TARGET chip8_core_lib
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/chip8_core_lib.lib # the file to copy
        ${LIBS_DIR}/chip8/chip8_core.lib       # the destination
)

# add executable for SDL2 client
add_executable(${PROJECT_NAME}
        # SDL Front-end
        ${PROJECT_SOURCE_DIR}/client/sdl/main.cpp
        ${PROJECT_SOURCE_DIR}/client/sdl/emulator.cpp
        ${PROJECT_SOURCE_DIR}/client/sdl/beeper.cpp
)
# Link SDL2 library
target_link_libraries(${PROJECT_NAME} SDL2/SDL2main SDL2/SDL2 chip8/chip8_core)
