#pragma once
#include "Core.h"
#include <cstdint>

namespace HBE {
    constexpr uint64_t generateComponentHash(const char *str) {
        uint64_t hash = 14695981039346656037ull;
        while (*str) {
            hash ^= static_cast<uint64_t>(*str++);
            hash *= 1099511628211ull;
        }
        return hash;
    }
}

#define COMPONENT_IDS(Type)   \
static constexpr uint64_t COMPONENT_HASH = HBE::generateComponentHash(#Type); \
static constexpr const char* COMPONENT_NAME = #Type; \
//static constexpr const size_t COMPONENT_SIZE = sizeof(Type);
