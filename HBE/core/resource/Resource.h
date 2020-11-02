#pragma once

#include "ShaderProgram.h"
#include "Texture.h"
#include "Framebuffer.h"
#include "Mesh.h"
#include "CompilationConfigs.h"
#include "core/utility/Log.h"
#include "core/resource/Material.h"
#include "core/resource/Model.h"
#include "typeinfo"

namespace Resource {
    template<class T>
    T *create() { Log::error(std::string("Resource ") + typeid(T).name() + " is not implemented for the current renderer"); };

    template<class T>
            T* load(std::string path) {Log::error(std::string("Resource ") + typeid(T).name() + " is not implemented for the current renderer"); }
    template<>
    Texture *create<Texture>();

    template<>
    Texture *load<Texture>(std::string path);

    template<>
    Material *create<Material>();

    template<>
    ShaderProgram *create<ShaderProgram>();

    template<>
    Mesh *create<Mesh>();

    template<>
    Model *load<Model>(std::string path);

    template<>
    Framebuffer *create<Framebuffer>();
};
