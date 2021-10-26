#pragma once

#include "Core.h"
#include "memory"
#include "ResourceFactory.h"
#include "core/graphics/Renderer.h"
#include "core/graphics/Graphics.h"
#include "core/resource/Shader.h"
#include "core/resource/GraphicPipeline.h"
#include "core/resource/VertexLayout.h"
#include "core/resource/ComputePipeline.h"
#include "core/resource/Mesh.h"
#include "core/resource/Texture.h"
#include "core/resource/Model.h"
#include "core/resource/ResourcesRegistry.h"
#include "core/resource/Image.h"
namespace HBE {
    namespace Resources {
        template<class T>
        static T *get(std::string unique_name) {
#ifdef DEBUG_MODE
            if (!ResourcesRegistry::exist(unique_name)) {
                Log::warning("Cant find resource:" + unique_name);
            }
#endif
            return (T *) ResourcesRegistry::get(unique_name);
        }

        static Resource *get(std::string unique_name) {
#ifdef DEBUG_MODE
            if (!ResourcesRegistry::exist(unique_name)) {
                Log::warning("Cant find resource:" + unique_name);
            }
#endif
            return ResourcesRegistry::get(unique_name);
        }

        template<class T>
        static void destroy() {
            ResourcesRegistry::clear();
        }

        static void destroyAll() {
            Resources::destroy<Mesh>();
            Resources::destroy<Texture>();
            Resources::destroy<GraphicPipeline>();
            Resources::destroy<ComputePipeline>();
            Resources::destroy<Model>();
            Resources::destroy<Shader>();
            Resources::destroy<VertexLayout>();
			Resources::destroy<Image>();
        }

        template<class T>
        static void destroy(std::string unique_name) {
            return ResourcesRegistry::remove(unique_name);
        }

        /**
         * Add a shared resource to the managed resources that will be deleted at the end of the application lifetime
         * @tparam Resource type to add to registry.
         * @param unique_name unique identifier to retrieve and delete this resource.
         * @param t Resource to add to the managed resources
         */
        template<class T>
        static void add(std::string unique_name, T *t) {
            return ResourcesRegistry::remove(unique_name);
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
            if (ResourcesRegistry::exist(unique_name)) {
                delete get<T>(unique_name);
                ResourcesRegistry::remove(unique_name);
                Log::warning("Resource with name:\"" + unique_name + "\" already exist in registry, deleted old resource.");
            }
#endif
            auto ptr = create<T>();
            ResourcesRegistry::add(unique_name, reinterpret_cast<Resource*>(ptr));
            return ptr;
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