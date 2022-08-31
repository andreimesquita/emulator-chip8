#pragma once

#include <cstdint>

struct Cartridge {
    char* filePath = nullptr;
    uint8_t* buffer = nullptr;
    size_t size = 0;

    ~Cartridge();

    void clear();
};
