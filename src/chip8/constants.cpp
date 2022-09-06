#include "chip8/constants.h"

uint32_t Config::Cpu::CyclesPerFrame = 10u;
double Config::Cpu::FrameTime = (1000.0 / 60.0);

uint32_t Config::Screen::SizeMultiplier = 10u;
uint32_t Config::Screen::Padding = 1u;
