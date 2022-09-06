#pragma once

#include <cstdio>
#include <cassert>
#include <iostream>
#include <fstream>

#include "SDL2/SDL.h"

#include "chip8/console.h"
#include "chip8/constants.h"
#include "chip8/cartridge/cartridge.h"
#include "beeper.h"

class Emulator {
    Cartridge _cartridge = {};
    Console _console = {};

    SDL_Window* _window = nullptr;
    SDL_Renderer* _renderer = nullptr;

    const uint8_t _keysMap[CHIP8_KEYS_SIZE] = {
            SDLK_1, SDLK_2, SDLK_3, SDLK_4,
            SDLK_q, SDLK_w, SDLK_e, SDLK_r,
            SDLK_a, SDLK_s, SDLK_d, SDLK_f,
            SDLK_z, SDLK_x, SDLK_c, SDLK_v
    };

    void LoadCartridgeFromFile(char* filePath);
    bool FindCorrespondingVirtualKey(SDL_Keycode keycode, uint8_t& vKey) const;

    void Draw() const;
    void ReadInput();

public:
    ~Emulator();

    void InitializeWindow();
    void Run(char* filePath);

    void Pause();
    void Resume();

    bool IsRunning = false;
    bool IsPaused = false;
};
