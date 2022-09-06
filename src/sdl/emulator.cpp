#include "emulator.h"

Emulator::~Emulator() {
    if (_window != nullptr) {
        SDL_DestroyWindow(_window);
    }
}

void Emulator::InitializeWindow() {
    // create SDL Window
    SDL_Init(SDL_INIT_EVERYTHING);
    _window = SDL_CreateWindow(
            EMULATOR_WINDOW_TITLE,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            CHIP8_SCREEN_WIDTH * Config::Screen::SizeMultiplier,
            CHIP8_SCREEN_HEIGHT * Config::Screen::SizeMultiplier,
            SDL_WINDOW_SHOWN
    );
    assert(_window);

    // create SDL Renderer
    _renderer = SDL_CreateRenderer(_window, -1, SDL_TEXTUREACCESS_TARGET);
    assert(_renderer);
}

void Emulator::Run(char* filePath) {
    assert(_window == nullptr && _renderer == nullptr);

    LoadCartridgeFromFile(filePath);

    _console.InsertCartridge(_cartridge);

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
            _console.Cycle();
        }

        // draw (if needed)
        if (_console.Hardware.Screen.Dirty) {
            Draw();
        }

        // play sound
        if (_console.Hardware.Flags.Sound) {
            Beeper::play();
        } else {
            Beeper::stop();
        }

        // read device inputs
        ReadInput();

        // update previous time
        prevTime = time;
    }

    SDL_DestroyWindow(_window);
}

void Emulator::LoadCartridgeFromFile(char* filePath) {
    std::cout << "Loading CHIP-8 Cartridge from path: " << filePath << std::endl;

    // clear struct content
    _cartridge.clear();

    // load file
    std::ifstream stream(filePath, std::ios_base::binary);
    assert(stream.good());

    // find file's size
    stream.seekg (0, std::ios_base::end);
    _cartridge.size = stream.tellg();
    assert(_cartridge.size > 0);

    _cartridge.filePath = filePath;

    // read file contents into the Cartridge's buffer
    stream.seekg(0, std::ios_base::beg);
    _cartridge.buffer = new uint8_t[_cartridge.size];
    stream.read((char*)_cartridge.buffer, _cartridge.size);
}

void Emulator::Draw() const {
    // clear the Screen
    SDL_SetRenderDrawColor(_renderer, 0u, 0u, 0u, 0u);
    SDL_RenderClear(_renderer);

    // draw active pixels as scalable rectangles
    SDL_SetRenderDrawColor(_renderer, 0u, 255u, 0u, 0u);
    SDL_Rect tempRect = {};

    const uint32_t pixelSize = Config::Screen::SizeMultiplier - (Config::Screen::Padding * 2);
    tempRect.w = pixelSize;
    tempRect.h = pixelSize;

    for (uint32_t x = 0; x < CHIP8_SCREEN_WIDTH; x++) {
        for (uint32_t y = 0; y < CHIP8_SCREEN_HEIGHT; y++) {
            if (_console.Hardware.Screen.isSet(x, y)) {
                tempRect.x = (x * Config::Screen::SizeMultiplier) + Config::Screen::Padding;
                tempRect.y = (y * Config::Screen::SizeMultiplier) + Config::Screen::Padding;

                SDL_RenderFillRect(_renderer, &tempRect);
            }
        }
    }
    SDL_RenderPresent(_renderer);
}

bool Emulator::FindCorrespondingVirtualKey(const SDL_Keycode keycode, uint8_t& vKey) const {
    for (int i = 0; i < CHIP8_KEYS_SIZE; i++)
    {
        if (_keysMap[i] == keycode)
        {
            vKey = i;
            return true;
        }
    }

    return false;
}

void Emulator::ReadInput() {
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
                    _console.Hardware.Keyboard.SetKeyDown(vKey);
                }
            }
                break;

            case SDL_KEYUP: {
                const SDL_Keycode keycode = event.key.keysym.sym;
                uint8_t vKey;
                if (FindCorrespondingVirtualKey(keycode, vKey)) {
                    _console.Hardware.Keyboard.SetKeyUp(vKey);
                }
            }
                break;
        }
    }
}

void Emulator::Pause() {
    assert(IsRunning);
    IsPaused = true;
}

void Emulator::Resume() {
    assert(IsRunning);
    IsPaused = false;
}
