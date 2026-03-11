#pragma once
#include "Handle.h"

namespace HBE
{
    enum FENCE_STATUS
    {
        FENCE_STATUS_NOT_READY,
        FENCE_STATUS_READY,
        FENCE_STATUS_ERROR,
    };
    using FenceHandle = Handle;
}
