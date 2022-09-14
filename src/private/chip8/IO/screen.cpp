#include "chip8/IO/screen.h"

#include <cassert>
#include <cstring>

#include "chip8/constants.h"

static void AssertPixelInScreenBounds(const uint32_t x, const uint32_t y)
{
    assert(x >= 0u && x < CHIP8_SCREEN_WIDTH
        && y >= 0u && y < CHIP8_SCREEN_HEIGHT);
}

void Screen::Set(uint32_t x, uint32_t y)
{
    AssertPixelInScreenBounds(x, y);
    if (!Pixels[y][x]) {
        Pixels[y][x] = true;
        Dirty = true;
    }
}

bool Screen::IsSet(uint32_t x, uint32_t y) const
{
    AssertPixelInScreenBounds(x, y);
    return Pixels[y][x];
}

bool Screen::DrawSprite(const uint32_t x, const uint32_t y, const uint8_t* sprite, const int numBytes)
{
    bool pixelCollision = false;
    for (int ly = 0; ly < numBytes; ly++) {
        const uint8_t c = sprite[ly];
        for (int lx = 0; lx < 8; lx++) {
            const bool isBitSet = (c & (0b1000'0000 >> lx)) != 0;
            if (!isBitSet)
            {
                continue;
            }

            Dirty = true;

            bool& pixel = Pixels[(ly + y) % CHIP8_SCREEN_HEIGHT][(lx + x) % CHIP8_SCREEN_WIDTH];
            pixelCollision |= pixel;
            pixel ^= true;
        }
    }
    return pixelCollision;
}

void Screen::Clear()
{
    memset(&Pixels, 0, sizeof(Pixels));
    Dirty = true;
}
