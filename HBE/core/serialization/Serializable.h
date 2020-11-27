#pragma once

class Serializer;
class Deserializer;
#include "Core.h"
class HB_API Serializable {
public:
    virtual void serialize(Serializer *serializer) const = 0;
    virtual void deserialize(Deserializer *deserializer) = 0;
};
