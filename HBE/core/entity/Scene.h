#pragma once
#include "Core.h"

#include <string>
#include "Entity.h"
#include "unordered_set"
#include "core/utility/Event.h"
namespace HBE {
	class HB_API Scene : public Serializable {
        friend class Entity;

        std::vector<Entity *> scene_tree;
		Event<Scene*> on_init;
		Event<Scene*> on_draw;
		Event<Scene*> on_render;
		Event<Scene*,float> on_update;
		Event<Scene*> on_terminate;
        std::unordered_set<Component *> update_listeners;
        std::unordered_set<Component *> draw_listeners;
    public:

        Scene(std::string path);

        Scene();

        virtual ~Scene();

        void init();

        void update(float delta);

        void draw();

        void render();

        void terminate();

        const std::vector<Entity *> &getSceneTree();

        Entity *instantiate(std::string name, Entity *parent = nullptr);

        Entity *instantiate(Entity *parent = nullptr);

        template<class ComponentType>
        ComponentType *instantiate(std::string name, Entity *parent = nullptr) {
            Entity *e = new Entity(name, parent);
            scene_tree.emplace_back(e);
            e->init();
            return e->attach<ComponentType>();
        }

        template<class ComponentType>
        ComponentType *instantiate(Entity *parent = nullptr) {
            Entity *e = new Entity( parent);
            scene_tree.emplace_back(e);
            e->init();
            return e->attach<ComponentType>();
        }

        void destroy(Entity *entity);

        void subscribeDraw(Component *component);

        void unsubscribeDraw(Component *component);

        void subscribeUpdate(Component *component);

        void unsubscribeUpdate(Component *component);

        void serialize(Serializer *serializer) const override;

        void deserialize(Deserializer *deserializer) override;
    };
}
