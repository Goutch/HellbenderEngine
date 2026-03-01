#include "Allocator.h"
#include "core/Context.h"

namespace HBE
{
    Allocator::Allocator(Context* context, AllocatorHandle handle)
    {
        this->context = context;
        this->handle = handle;
    }
}
