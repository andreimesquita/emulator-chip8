#pragma once

#include <cstdint>
#include <functional>

#include "chip8/constants.h"

struct Keyboard
{
    void SetKeyDown(int vKey);
    void SetKeyUp(int vKey);
    bool IsKeyDown(int vKey) const;

    std::function<const void(uint8_t)> OnKeyDownEvent = nullptr;

private:
    bool _keyboard[CHIP8_KEYS_SIZE] {};
};
