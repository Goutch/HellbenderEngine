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
#include "Configs.h"
#include "memory"

/*
 * WHAT I WANT
 *
 * Resource::create<Mesh>()                             :create api agnostic mesh
 * Resource::save<Mesh>(Mesh* mesh,"mesh/path")         :save mesh to raw data file
 * Resource::load<Mesh>("mesh/path")                    :load mesh from raw data file only if it is not already loaded
 * Resource::loadAsync<Mesh>(Mesh* mesh,"mesh/path")    :load mesh asyncronously from raw data file
 * Resource::dispose<Mesh>(Mesh* mesh)                  :cleanup memory for that resource
 */
#if RENDERER == OPENGL_RENDERER

#include "graphics/gl/GL_Core.h"

#elif RENDERER == VULKAN_RENDERER

#endif


class Resource {
public:
    template<class T>
    static T *create() { Log::error(typeid(T).name() + std::string("resource instancing is not supported")); };

    template<class T>
    static T *load(std::string path) { Log::error(typeid(T).name() + std::string("resource loading is not supported")); }

    template<class T>
    static void save(T *, std::string path) { Log::error(typeid(T).name() + std::string("resource saving is not supported")); }

#pragma region Texture

    template<>
    static Texture *create<Texture>() {
#if RENDERER == OPENGL_RENDERER
        return new GL_Texture();
#elif RENDERER == VULKAN_RENDERER

#endif
        return nullptr;
    };

    template<>
    static Texture *load<Texture>(std::string path) {
        Texture* texture=create<Texture>();
        texture->load(path);
        return texture;
    };

    template<>
    static void save<Texture>(Texture *texture, std::string path) {
        Log::error("not implemented");
        //todo:
    };
#pragma endregion Texture

#pragma region Material

    template<>
    static Material *create<Material>() { return new Material(); };

    template<>
    static Material *load<Material>(std::string path) {
        Log::error("not implemented");
        //todo:
        return nullptr;
    };

    template<>
    static void save<Material>(Material *material, std::string path) {
        Log::error("not implemented");
        //todo:
    };
#pragma endregion Material

#pragma region ShaderProgram

    template<>
    static ShaderProgram *create<ShaderProgram>() {
#if RENDERER == OPENGL_RENDERER
        return new GL_ShaderProgram();
#elif RENDERER == VULKAN_RENDERER

#endif
        return nullptr;

    }

    template<>
    static ShaderProgram *load<ShaderProgram>(std::string path) {
        Log::error("not implemented");
        //todo:
        return nullptr;
    };

    template<>
    static void save<ShaderProgram>(ShaderProgram *shaderProgram, std::string path) {
        Log::error("not implemented");
        //todo:
    };
#pragma endregion ShaderProgram

#pragma region Mesh

    template<>
    static Mesh *create<Mesh>() {
#if RENDERER == OPENGL_RENDERER
        return new GL_Mesh();
#elif RENDERER == VULKAN_RENDERER

#endif
        return nullptr;
    };

    template<>
    static Mesh *load<Mesh>(std::string path) {
        Log::error("not implemented");
        //todo:
        return nullptr;
    };

    template<>
    static void save<Mesh>(Mesh *mesh, std::string path) {
        Log::error("not implemented");
        //todo:

    };
#pragma endregion Mesh

#pragma region Model

    template<>
    static Model *load<Model>(std::string path) {
        Model *model = new Model();
        model->load(path);
        return model;
    };
#pragma endregion Model

#pragma region Framebuffer

    template<>
    static Framebuffer *create<Framebuffer>() {
#if RENDERER == OPENGL_RENDERER
        if(Configs::getAntialiasing())
        {
            return new GL_MultisamplingFramebuffer();
        }

#elif RENDERER == VULKAN_RENDERER

#endif
        return nullptr;
    };

#pragma endregion Framebuffer
};
