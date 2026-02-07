#pragma once
namespace HBE {
    class Fence {
        virtual void wait() const=0;
    };
}
