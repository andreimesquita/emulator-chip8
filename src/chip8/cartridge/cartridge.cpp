#include "cartridge.h"

Cartridge::~Cartridge() {
    clear();
}

void Cartridge::clear() {
    delete[] buffer;

    filePath = nullptr;
    size = 0;
}
