#include "chip8/memory/stack.h"

#include <cassert>

#include "chip8/constants.h"

static void AssertStackInBounds(const uint8_t SP)
{
    assert(SP < CHIP8_MEMORY_STACK_SIZE);
}

void Stack::Push(uint16_t value)
{
    AssertStackInBounds(SP);
    stack[SP] = value;
    SP += 1u;
}

uint16_t Stack::Pop()
{
    SP -= 1u;
    AssertStackInBounds(SP);
    return stack[SP];
}
