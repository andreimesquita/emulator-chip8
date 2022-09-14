#include "chip8/constants.h"

namespace Config {
    namespace Cpu {
        uint32_t CyclesPerFrame = 10u;
        double FrameTime = (1000.0 / 60.0);
    }
    namespace Screen {
        uint32_t SizeMultiplier = 10u;
        uint32_t Padding = 1u;
    }
}
