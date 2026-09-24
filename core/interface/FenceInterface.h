#pragma once
#include "Handle.h"

namespace HBE {
    enum FENCE_STATUS {
        FENCE_STATUS_NOT_READY,
        FENCE_STATUS_READY,
        FENCE_STATUS_ERROR,
    };

    typedef uint8_t FENCE_FLAGS;

    enum FENCE_FLAG :uint8_t {
        FENCE_FLAG_NONE = 0,
    };

    struct FenceInfo {
        FENCE_FLAGS flags = FENCE_FLAG_NONE;
    };

    using FenceHandle = Handle;
}
