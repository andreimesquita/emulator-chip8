#include "chip8/IO/keyboard.h"

#include <cassert>

#include "chip8/constants.h"

static void AssertKeyInBounds(int vKey)
{
    assert(vKey >= 0 && vKey < CHIP8_KEYS_SIZE);
}

void Keyboard::SetKeyDown(int vKey)
{
    AssertKeyInBounds(vKey);
    _keyboard[vKey] = true;

    if (OnKeyDownEvent != nullptr) {
        OnKeyDownEvent(vKey);
    }
}

void Keyboard::SetKeyUp(int vKey)
{
    AssertKeyInBounds(vKey);
    _keyboard[vKey] = false;
}

bool Keyboard::IsKeyDown(int vKey) const
{
    AssertKeyInBounds(vKey);
    return _keyboard[vKey];
}
