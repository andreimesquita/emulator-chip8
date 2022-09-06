#pragma once

#include "cpu/timers.h"
#include "cpu/registers.h"
#include "memory/memory.h"
#include "memory/stack.h"
#include "IO/keyboard.h"
#include "IO/screen.h"

struct Hardware {
    struct {
        bool Draw = false;
        bool Sound = false;
    } Flags;

    Memory Memory {};
    Stack Stack {};
    Keyboard Keyboard {};
    Screen Screen {};
};
