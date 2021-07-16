#pragma once


#include "core/utility/Log.h"
#include "unordered_map"
#include "memory"


namespace HBE {

    class Resource {

    protected:
        Resource &operator=(const Resource &) = delete;

        Resource(const Resource &) = delete;

        Resource() = default;

    public:
        virtual ~Resource() = default;


    };

}
