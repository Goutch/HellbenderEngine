#pragma once

#include "ShaderProgram.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Mesh.h"
#include "CompilationConfigs.h"
#include "core/utility/Log.h"
namespace Resource {
    template<class T> T* get(){ Log::error(std::string("Resource")+typeid(T).name()+"is not implemented for the current renderer");};

    template<> Texture* get<Texture>();

    template<> ShaderProgram* get<ShaderProgram>();

    template<> Mesh* get<Mesh>();

    template<> Framebuffer* get<Framebuffer>();
};
