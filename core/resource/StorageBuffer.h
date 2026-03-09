#pragma once

#include "Core.h"
#include "Resource.h"

namespace HBE
{

    class HB_API StorageBuffer
    {
    public :
        virtual void update(const void* data) = 0;

        virtual uint32_t getCount() const = 0;

        virtual uint32_t getStride() const = 0;

        virtual ~StorageBuffer() = default;
    };
}
