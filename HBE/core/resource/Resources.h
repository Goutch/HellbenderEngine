#pragma once


#include "core/utility/Log.h"
#include "unordered_map"
#include "memory"
#include "ResourceFactory.h"
#include "core/graphics/Renderer.h"
#include "core/graphics/Graphics.h"

namespace HBE {
    class GraphicPipeline;

    class Texture;

    class Mesh;

    class Shader;

    class ComputePipeline;

    class VertexLayout;

    template<typename T>
    class ResourcesRegistry {
        static std::unordered_map<std::string, T *> registry;
    public:
        static void clear() {
            for (auto n_t:registry) {
                delete n_t.second;
            }
            registry.clear();
        }

        static void add(std::string &name, T *t) {
            registry.emplace(name, t);
        }

        static T *get(std::string &name) {
#ifdef DEBUG_MODE
            if (registry.find(name) == registry.end()) {
                Log::error("Cant find resource:" + name);
            }
#endif
            return registry.at(name);
        }

        static void remove(std::string &name) {
            registry.erase(name);
        }
    };

    template<typename T>
    std::unordered_map<std::string, T *> ResourcesRegistry<T>::registry;

    namespace Resources {
        template<class T>
        static T *get(std::string unique_name) {
            return ResourcesRegistry<T>::get(unique_name);
        }

        template<class T>
        static void remove(std::string unique_name) {
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
            auto ptr = create<T>();
            ResourcesRegistry<T>::add(unique_name,ptr);
            return ptr;
        }
        template<>
        Material *create<Material>() {
            return new Material();
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