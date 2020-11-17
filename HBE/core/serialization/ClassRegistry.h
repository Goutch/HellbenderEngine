#pragma once

#include <string>
#include "Core.h"
#include "map"
#include "unordered_map"
class ITypeInfo{
    virtual void* cast();
};
template<typename T>
class TypeInfo{
    virtual void* cast();
};
class HB_API ClassRegistry {
    std::unordered_map<std::string,ITypeInfo*> types;
    template<typename T>
    static void registerClass(std::string name)
    {
/*        if(types.find(name)!=types.end())
        {
            types.emplace(std::make_pair(name,new TypeInfo<T>()))
        }*/
    }
};

