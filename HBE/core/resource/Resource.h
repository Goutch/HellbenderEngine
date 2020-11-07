#pragma once

class Serializer;
#include "core/utility/Job.h"
#include "core/utility/ModelImporter.h"
#include "list"

class Resource {
public:
    virtual void serialize(Serializer *serializer) const = 0;
};
