#pragma once

#include "Core.h"


#include "core/scene/ecs/Registry.h"
#include "core/scene/Entity.h"

#include "Components.h"
#include "System.h"
#include "unordered_map"
#include "core/scene/systems/CameraSystem.h"
#include "core/scene/systems/MeshRendererSystem.h"
#include "core/scene/systems/CameraControllerSystem.h"
#include "typeinfo"
#include "core/utility/Event.h"

namespace HBE {

    class RenderTarget;

    struct Transform;

    struct SceneNode {
        Entity entity;
        bool has_parent = false;
        SceneNode *parent = nullptr;
        std::list<SceneNode> children;

        bool operator==(const SceneNode &other) const {
            return entity.getHandle() == other.entity.getHandle();
        }

    };

    class HB_API Scene {
    public:
        Event<> onRender;
        Event<> onDraw;
        Event<float> onUpdate;
    private:
        Registry registry;
        std::list<SceneNode> root_nodes;
        std::unordered_map<entity_handle, SceneNode *> node_map;

        Scene(const Scene &scene) = delete;

        Scene(Scene &scene) = delete;

        std::vector<System *> systems;
        Entity main_camera_entity;
        std::unordered_map<size_t, Event<Entity>> attach_events;
        std::unordered_map<size_t, Event<Entity>> detach_events;

    public:
        Scene();

        ~Scene();

        void update(float deltaTime);

        void draw();

        void render();

        Entity createEntity3D();

        Entity createEntity2D();

        Entity createEntity();

        void destroyEntity(Entity entity);

        Entity getCameraEntity();


        void setCameraEntity(Entity camera);

        void addSystem(System *system);

        template<typename ... Components>
        auto group();

        template<typename Component>
        Component &get(entity_handle handle);

        template<typename Component>
        Component &attach(entity_handle handle);

        template<typename Component>
        Component &attach(entity_handle handle, Component &component);


        template<typename Component>
        bool has(entity_handle handle);

        bool has(entity_handle handle, size_t component_hash);

        template<typename Component>
        void detach(entity_handle handle);

        bool valid(entity_handle handle);

        template<typename Component>
        Event<Entity> &onAttach();

        template<typename Component>
        Event<Entity> &onDetach();

        void calculateCameraProjection(RenderTarget *renderTarget);


        //sceneHierachy

        void setParent(Entity entity, Entity parent = {});

        Entity getParent(Entity entity);

        const std::list<SceneNode> &getChildren(Entity entity);

        void print();

    private:
        SceneNode *getNode(Entity entity);

        void printNode(SceneNode &node, int level);

        void removeFromTree(Entity entity);

        void onAttachTransform(Entity entity);
    };

    template<typename Component>
    Component &Entity::attach(Component &component) {
        return scene->attach<Component>(handle, component);
    }

    template<typename Component>
    Component &Entity::attach() {
        return scene->attach<Component>(handle);
    }

    template<typename Component>
    Component &Entity::get() {

        return scene->get<Component>(handle);
    }

    template<typename Component>
    void Entity::detach() {
        scene->detach<Component>(handle);
    }

    template<typename Component>
    bool Entity::has() {
        return scene->has<Component>(handle);
    }


    template<typename ... Components>
    auto Scene::group() {
        return registry.group<Components...>();
    }


    template<typename Component>
    Component &Scene::get(entity_handle handle) {
        return registry.get<Component>(handle);
    }

    template<typename Component>
    Component &Scene::attach(entity_handle handle) {
        size_t hash = typeHash<Component>();
        Component &component = registry.attach<Component>(handle);
        Entity e = Entity(handle, this);
        if (attach_events.find(hash) != attach_events.end())
            attach_events[hash].invoke(e);
        return component;
    };


    template<typename Component>
    Component &Scene::attach(entity_handle handle, Component &component) {
        size_t hash = typeHash<Component>();
        Component &component_ref = registry.attach<Component>(handle, component);

        if (attach_events.find(hash) != attach_events.end())
            attach_events[hash].invoke(Entity(handle, this));
        return component_ref;
    };

    template<typename Component>
    bool Scene::has(entity_handle handle) {
        return registry.has<Component>(handle);
    }

    template<typename Component>
    void Scene::detach(entity_handle handle) {
        size_t hash = typeHash<Component>();

        if (detach_events.find(hash) != detach_events.end())
            detach_events[hash].invoke(Entity(handle, this));
        registry.detach<Component>(handle);
    };

    template<typename Component>
    Event<Entity> &Scene::onAttach() {
        size_t hash = typeHash<Component>();
        if (attach_events.find(hash) == attach_events.end()) {
            attach_events.emplace(hash, Event<Entity>());
        }
        return attach_events[hash];
    };

    template<typename Component>
    Event<Entity> &Scene::onDetach() {

        size_t hash = typeHash<Component>();
        if (detach_events.find(hash) == detach_events.end()) {
            detach_events.emplace(hash, Event<Entity>());
        }
        return detach_events[hash];

    }

}


