#pragma once
#include "Core.h"
#include "unordered_map"
#include "string"
#include "core/utility/Log.h"
#include "Resource.h"
namespace HBE {


    class HB_API ResourcesRegistry {
        static std::unordered_map<std::string, Resource *> registry;
    public:
        static void clear();

        static bool exist(std::string &name);

        static void add(std::string &name, Resource *res);

        static Resource *get(std::string &name);

        static void remove(std::string &name);
    };




}
