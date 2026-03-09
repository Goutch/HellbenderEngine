#pragma once

#include "Core.h"
#include "Image.h"

namespace HBE
{
    class HB_API TexelBuffer
    {
    public:
        virtual void update(const void* data) = 0;

        virtual ~TexelBuffer()
        {
        };
    };
}
