#include "keyboard.h"

#include <cassert>

static void assertKeyInBounds(int vKey)
{
    assert(vKey >= 0 && vKey < CHIP8_KEYS_SIZE);
}

void Keyboard::SetKeyDown(int vKey)
{
    assertKeyInBounds(vKey);
    _keyboard[vKey] = true;

    if (OnKeyDownEvent != nullptr) {
        OnKeyDownEvent(vKey);
    }
}

void Keyboard::SetKeyUp(int vKey)
{
    assertKeyInBounds(vKey);
    _keyboard[vKey] = false;
}

bool Keyboard::IsKeyDown(int vKey) const
{
    assertKeyInBounds(vKey);
    return _keyboard[vKey];
}
