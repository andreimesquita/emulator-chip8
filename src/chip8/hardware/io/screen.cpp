#include "screen.h"
#include <cassert>
#include <cstring>

static void assertPixelInScreenBounds(const uint32_t x, const uint32_t y)
{
    assert(x >= 0u && x < CHIP8_SCREEN_WIDTH
        && y >= 0u && y < CHIP8_SCREEN_HEIGHT);
}

void Screen::set(uint32_t x, uint32_t y)
{
    assertPixelInScreenBounds(x, y);
    if (!Pixels[y][x]) {
        Pixels[y][x] = true;
        Dirty = true;
    }
}

bool Screen::isSet(uint32_t x, uint32_t y) const
{
    assertPixelInScreenBounds(x, y);
    return Pixels[y][x];
}

bool Screen::drawSprite(const uint32_t x, const uint32_t y, const uint8_t* sprite, const int numBytes)
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

void Screen::clear()
{
    memset(&Pixels, 0, sizeof(Pixels));
    Dirty = true;
}
