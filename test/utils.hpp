#pragma once

#include <functional>

template<typename T>
void ForEach(T* source, const size_t length, const std::function<void(T*)>& validation) {
    for (size_t offset = 0; offset < length; offset++) {
        validation(source + offset);
    }
}
