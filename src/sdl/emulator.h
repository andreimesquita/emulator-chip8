#pragma once

#include <cstdio>
#include <cassert>
#include <iostream>
#include <fstream>

#include <SDL.h>

#include "chip8/console.h"
#include "chip8/constants.h"
#include "chip8/cartridge/cartridge.h"
#include "sdl/beeper.h"

class Emulator {
    Cartridge cartridge = {};
    Console console = {};

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    const uint8_t keysMap[CHIP8_KEYS_SIZE] = {
            SDLK_1, SDLK_2, SDLK_3, SDLK_4,
            SDLK_q, SDLK_w, SDLK_e, SDLK_r,
            SDLK_a, SDLK_s, SDLK_d, SDLK_f,
            SDLK_z, SDLK_x, SDLK_c, SDLK_v
    };

    void LoadCartridgeFromFile(const char* filePath) {
        std::cout << "Loading CHIP-8 Cartridge from path: " << filePath << std::endl;

        // clear struct content
        cartridge.clear();

        // load file
        std::ifstream stream(filePath, std::ios_base::binary);
        assert(stream.good());

        // find file's size
        stream.seekg (0, std::ios_base::end);
        cartridge.size = stream.tellg();
        assert(cartridge.size > 0);

        // read file contents into the Cartridge's buffer
        stream.seekg(0, std::ios_base::beg);
        cartridge.buffer = new uint8_t[cartridge.size];
        stream.read((char*)cartridge.buffer, cartridge.size);
    }

    void Draw() const {
        // clear the Screen
        SDL_SetRenderDrawColor(renderer, 0u, 0u, 0u, 0u);
        SDL_RenderClear(renderer);

        // draw active pixels as scalable rectangles
        SDL_SetRenderDrawColor(renderer, 0u, 255u, 0u, 0u);
        SDL_Rect tempRect = {};

        const int pixelSize = Config::Screen::SizeMultiplier - (Config::Screen::Padding * 2);
        tempRect.w = pixelSize;
        tempRect.h = pixelSize;

        for (uint32_t x = 0; x < CHIP8_SCREEN_WIDTH; x++) {
            for (uint32_t y = 0; y < CHIP8_SCREEN_HEIGHT; y++) {
                if (console.hardware.Screen.isSet(x, y)) {
                    tempRect.x = (x * Config::Screen::SizeMultiplier) + Config::Screen::Padding;
                    tempRect.y = (y * Config::Screen::SizeMultiplier) + Config::Screen::Padding;

                    SDL_RenderFillRect(renderer, &tempRect);
                }
            }
        }
        SDL_RenderPresent(renderer);
    }

    bool FindCorrespondingVirtualKey(const SDL_Keycode keycode, uint8_t& vKey) const {
        for (int i = 0; i < CHIP8_KEYS_SIZE; i++)
        {
            if (keysMap[i] == keycode)
            {
                vKey = i;
                return true;
            }
        }

        return false;
    }

    void ReadInput() {
        // handle input events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    exit(0);
                    break;

                case SDL_KEYDOWN: {
                    const SDL_Keycode keycode = event.key.keysym.sym;
                    uint8_t vKey;
                    if (FindCorrespondingVirtualKey(keycode, vKey)) {
                        console.hardware.Keyboard.SetKeyDown(vKey);
                    }
                }
                    break;

                case SDL_KEYUP: {
                    const SDL_Keycode keycode = event.key.keysym.sym;
                    uint8_t vKey;
                    if (FindCorrespondingVirtualKey(keycode, vKey)) {
                        console.hardware.Keyboard.SetKeyUp(vKey);
                    }
                }
                    break;
            }
        }
    }

public:
    ~Emulator() {
        if (window != nullptr) {
            SDL_DestroyWindow(window);
        }
    }

    void InitializeWindow() {
        // create SDL Window
        SDL_Init(SDL_INIT_EVERYTHING);
        window = SDL_CreateWindow(
                EMULATOR_WINDOW_TITLE,
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                CHIP8_SCREEN_WIDTH * Config::Screen::SizeMultiplier,
                CHIP8_SCREEN_HEIGHT * Config::Screen::SizeMultiplier,
                SDL_WINDOW_SHOWN
        );
        assert(window);

        // create SDL Renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);
        assert(renderer);
    }

    void Run(const char* filePath) {
        assert(window == nullptr && renderer == nullptr);

        LoadCartridgeFromFile(filePath);

        console.InsertCartridge(cartridge);

        InitializeWindow();

        // run game loop
        IsRunning = true;
        uint64_t time = SDL_GetTicks64();
        uint64_t prevTime = time;
        uint64_t accumulatedTime = 0u;
        while (IsRunning) {

            // if paused, ignore game loop but keep handling user inputs
            if (IsPaused) {
                accumulatedTime = 0u;
                ReadInput();
                continue;
            }

            // calculate the deltaTime
            time = SDL_GetTicks64();
            accumulatedTime += (time - prevTime);

            while (accumulatedTime >= static_cast<uint64_t>(Config::Cpu::FrameTime)) {
                accumulatedTime -= static_cast<uint64_t>(Config::Cpu::FrameTime);
                console.Cycle();
            }

            // draw (if needed)
            if (console.hardware.Flags.Draw) {
                Draw();
            }

            // play sound
            if (console.hardware.Flags.Sound) {
                Beeper::play();
            } else {
                Beeper::stop();
            }

            // read device inputs
            ReadInput();

            // update previous time
            prevTime = time;
        }

        SDL_DestroyWindow(window);
    }

    void Pause() {
        assert(IsRunning);
        IsPaused = true;
    }

    void Resume() {
        assert(IsRunning);
        IsPaused = false;
    }

    bool IsRunning = false;
    bool IsPaused = false;
};
