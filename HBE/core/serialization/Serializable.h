#pragma once
#include "Core.h"

namespace HBE {
    class Serializer;
    class Deserializer;
    class HB_API Serializable {
    public:
    public:

        virtual void serialize(Serializer *serializer) const = 0;

        virtual void deserialize(Deserializer *deserializer) = 0;
    };
}