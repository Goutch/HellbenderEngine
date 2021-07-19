#pragma once


#include "core/utility/Log.h"
#include "unordered_map"
#include "memory"
#include "ResourceFactory.h"
#include "core/graphics/Renderer.h"
#include "core/graphics/Graphics.h"

#include "core/resource/Shader.h"
#include "core/resource/GraphicPipeline.h"
#include "core/resource/VertexLayout.h"
#include "core/resource/ComputePipeline.h"
#include "core/resource/Mesh.h"
#include "core/resource/Material.h"
#include "core/resource/Texture.h"
#include "core/resource/Model.h"

namespace HBE {

    template<typename T>
    class ResourcesRegistry {
        static std::unordered_map<std::string, T *> registry;
    public:
        static void clear() {
            Log::message("clear with count:" + std::to_string(ResourcesRegistry<T>::registry.size()));
            for (auto &n_t:ResourcesRegistry<T>::registry) {
                Log::message("destroy res: " + n_t.first);
                delete n_t.second;
            }
            ResourcesRegistry<T>::registry.clear();
        }

        static bool exist(std::string &name) {
            Log::message("exist with count:" + std::to_string(ResourcesRegistry<T>::registry.size()));
            return ResourcesRegistry<T>::registry.find(name) != registry.end();
        }

        static void add(std::string &name, T *t) {
            ResourcesRegistry<T>::registry.emplace(name, t);
            Log::message("added:" + name);
            Log::message("add with count:" + std::to_string(ResourcesRegistry<T>::registry.size()));
        }

        static T *get(std::string &name) {
#ifdef DEBUG_MODE
            if (!exist(name)) {
                Log::error("Cant find resource:" + name);
            }
#endif
            Log::message("get with count:" + std::to_string(ResourcesRegistry<T>::registry.size()));
            return ResourcesRegistry<T>::registry.at(name);
        }

        static void remove(std::string &name) {
            Log::message("remove with count:" + std::to_string(ResourcesRegistry<T>::registry.size()));
            delete get(name);
            ResourcesRegistry<T>::registry.erase(name);
        }
    };

    template<typename T>
    std::unordered_map<std::string, T *> ResourcesRegistry<T>::registry;

    namespace Resources {
        template<class T>
        static T *get(std::string unique_name) {
#ifdef DEBUG_MODE
            if (!ResourcesRegistry<T>::exist(unique_name)) {
                Log::warning("Cant find resource:" + unique_name);
            }
#endif
            return ResourcesRegistry<T>::get(unique_name);
        }


        template<class T>
        static void destroy() {
            ResourcesRegistry<T>::clear();
        }

        static void destroyAll() {
            Resources::destroy<Mesh>();
            Resources::destroy<Texture>();
            Resources::destroy<GraphicPipeline>();
            Resources::destroy<ComputePipeline>();
            Resources::destroy<Model>();
            Resources::destroy<Shader>();
            Resources::destroy<VertexLayout>();
            Resources::destroy<Material>();
        }

        template<class T>
        static void destroy(std::string unique_name) {
            return ResourcesRegistry<T>::remove(unique_name);
        }

        /**
         * Add a shared resource to the managed resources that will be deleted at the end of the application lifetime
         * @tparam Resource type to add to registry.
         * @param unique_name unique identifier to retrieve and delete this resource.
         * @param t Resource to add to the managed resources
         */
        template<class T>
        static void add(std::string unique_name, T *t) {
            return ResourcesRegistry<T>::remove(unique_name);
        }

        /**
         * Create an unmanaged resource.
         * @tparam Resource type to create.
         * @return Unmanaged pointer to the created resource.
         */
        template<class T>
        static T *create() {
            Log::error("Resources can't create non resource type");
            return nullptr;
        };

        /**
         * Create an unmanaged shared resource
         * @tparam Resource type to create.
         * @return managed pointer to the created resource
         */
        template<class T>
        static std::shared_ptr<T> createShared() {
            return std::make_shared<T>(create<T>());
        };

        /**
         * Create a managed resource that will be deleted at the end of the application lifetime.
         * @tparam Resource type to add to registry.
         * @param unique_name unique identifier to retrieve and delete this resource.
         * @return managed pointer to the created resource
         */
        template<class T>
        static T *createInRegistry(std::string unique_name) {
#ifdef DEBUG_MODE
            if (ResourcesRegistry<T>::exist(unique_name)) {
                delete get<T>(unique_name);
                ResourcesRegistry<T>::remove(unique_name);
                Log::warning("Resource with name:\"" + unique_name + "\" already exist in registry, deleted old resource.");
            }
#endif
            auto ptr = create<T>();
            ResourcesRegistry<T>::add(unique_name, ptr);
            return ptr;
        }

        template<>
        Material *create<Material>() {
            return new Material();
        }

        template<>
        Model *create<Model>() {
            return new Model();
        }

        template<>
        Mesh *create<Mesh>() {
            return Graphics::getRenderer()->getResourceFactory()->createMesh();
        }

        template<>
        Shader *create<Shader>() {
            return Graphics::getRenderer()->getResourceFactory()->createShader();
        }

        template<>
        GraphicPipeline *create<GraphicPipeline>() {
            return Graphics::getRenderer()->getResourceFactory()->createGraphicsPipeline();
        }

        template<>
        ComputePipeline *create<ComputePipeline>() {
            return Graphics::getRenderer()->getResourceFactory()->createComputePipeline();
        }

        template<>
        VertexLayout *create<VertexLayout>() {
            return Graphics::getRenderer()->getResourceFactory()->createVertexLayout();
        }

        template<>
        Texture *create<Texture>() {
            return Graphics::getRenderer()->getResourceFactory()->createTexture();
        }
    };


}