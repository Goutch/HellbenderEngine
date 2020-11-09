#pragma once

class Serializer;
#include "core/utility/Job.h"
#include "core/utility/ModelImporter.h"
#include "list"
#include "Core.h"
class HB_API Resource {
public:
    virtual void serialize(Serializer *serializer) const = 0;
};
