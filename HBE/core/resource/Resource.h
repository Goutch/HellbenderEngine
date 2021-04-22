#pragma once
namespace HBE{
    class Resource
    {
        Resource & operator=(const Resource&) = delete;
        Resource(const Resource&) = delete;

    protected:
        Resource() = default;
    };
}
