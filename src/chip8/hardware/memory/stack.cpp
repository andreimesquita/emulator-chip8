#include "stack.h"

#include <cassert>

static void assertStackInBounds(const uint8_t SP)
{
    assert(SP < CHIP8_STACK_SIZE);
}

void Stack::Push(uint16_t value)
{
    assertStackInBounds(SP);
    stack[SP] = value;
    SP += 1u;
}

uint16_t Stack::Pop()
{
    SP -= 1u;
    assertStackInBounds(SP);
    return stack[SP];
}
