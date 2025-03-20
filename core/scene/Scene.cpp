#pragma once

#include "Scene.h"
#include "Entity.h"
#include "vector"
#include "components/HierarchyNode.h"
#include "core/scene/systems/CameraSystem.h"
#include "core/scene/systems/CameraControllerSystem.h"
#include "core/scene/systems/ModelRendererSystem.h"
#include "core/scene/systems/MeshRendererSystem.h"
#include "systems/TransformSystem.h"

namespace HBE
{
	const Entity Entity::NULL_ENTITY = Entity();

	Entity Scene::getCameraEntity()
	{
		return main_camera_entity;
	}

	void Scene::destroyEntity(entity_handle entity)
	{
		if (!valid(entity)) return;

		HierarchyNode* node = get<HierarchyNode>(entity);
		for (int32_t i = node->children.size() - 1; i >= 0; --i)
		{
			HierarchyNode* child_node = get<HierarchyNode>(node->children[i]);
			child_node->local_index = node->children.size() - 1;
			destroyEntity(node->children[i]);
		}
		std::bitset<REGISTRY_MAX_COMPONENT_TYPES> signature = registry.getSignature(entity);

		for (auto e : detach_events)
		{
			if (signature.test(e.first))
			{
				e.second.invoke(Entity(entity, this));
			}
		}
		registry.destroy(entity);
	}

	void Scene::setCameraEntity(Entity camera)
	{
		main_camera_entity = camera;
	}


	void Scene::update(float delta_t)
	{
		if (is_active)
		{
			onUpdate.invoke(delta_t);
			hierarchy_system->updateNodeIndices();
		}
	}

	void Scene::draw()
	{
		if (is_active)
		{
			onDraw.invoke(&render_graph);
		}
	}

	void Scene::present()
	{
	}

	void Scene::render()
	{
		if (is_active)
		{
			onRender.invoke(&render_graph);
		}
	}

	Scene::~Scene()
	{
		Graphics::getDefaultRenderTarget()->onResolutionChange.unsubscribe(this);
		for (auto event : detach_events)
		{
			std::vector<entity_handle> entities;
			registry.group(entities, event.first);
			for (unsigned int entitie : entities)
			{
				event.second.invoke(Entity(entitie, this));
			}
		}

		for (int i = 0; i < systems.size(); ++i)
		{
			delete systems[i];
		}
		Application::onDraw.unsubscribe(this);
		Application::onRender.unsubscribe(this);
		Application::onUpdate.unsubscribe(this);
	}

	bool Scene::valid(entity_handle handle)
	{
		return registry.valid(handle);
	}


	bool Scene::has(entity_handle handle, size_t signature_bit)
	{
		return registry.has(handle, signature_bit);
	}


	const std::vector<entity_handle>& Scene::getRootNodes() const
	{
		return hierarchy_system->getRootNodes();
	}

	void Scene::setActive(bool active)
	{
		if (active != is_active)
		{
			if (active)
				onSceneActivate.invoke(this);
			else
				onSceneDeactivate.invoke(this);
		}
		is_active = active;
	}

	bool Scene::isActive()
	{
		return is_active;
	}

	void Scene::onFrameChange(uint32_t frame)
	{
		render_graph.clear();
	}

	bool Entity::valid()
	{
		return scene != nullptr && scene->valid(handle);
	}

	entity_handle Entity::getHandle() const
	{
		return handle;
	}

	Entity::Entity(const Entity& other)
	{
		this->handle = other.handle;
		this->scene = other.scene;
	}

	Entity::Entity(entity_handle handle, Scene* scene)
	{
		this->handle = handle;
		this->scene = scene;
	}

	bool Entity::has(size_t sigature_bit)
	{
		return scene->has(handle, sigature_bit);
	}

	Scene* Entity::getScene()
	{
		return scene;
	}

	void Entity::destroy()
	{
		scene->destroyEntity(handle);
	}

	int Entity::operator==(const Entity& other) const
	{
		return other.handle == handle && other.scene == scene;
	}

	void Scene::addSystem(System* system)
	{
		systems.push_back(system);
	}

	Scene::Scene()
	{
		transform_system = new TransformSystem(this);
		hierarchy_system = new HierarchySystem(this);
		systems.reserve(5);
		systems.push_back(hierarchy_system);
		systems.push_back(new CameraSystem(this));
		systems.push_back(new CameraControllerSystem(this));
		systems.push_back(new ModelRendererSystem(this));
		systems.push_back(new MeshRendererSystem(this));

		Application::onDraw.subscribe(this, &Scene::draw);
		Application::onRender.subscribe(this, &Scene::render);
		Application::onUpdate.subscribe(this, &Scene::update);
		Application::onPresent.subscribe(this, &Scene::present);
		Graphics::onFrameChange.subscribe(this, &Scene::onFrameChange);
	}

	Entity Scene::createEntity()
	{
		Entity e = Entity(registry.create(), this);

		HierarchyNode* node = attach<HierarchyNode>(e.getHandle());
		node->entity = e;
		return e;
	}

	Entity Scene::createEntity3D()
	{
		Entity e = createEntity();

		attach<Transform>(e.getHandle());
		return e;
	}

	Entity Scene::createEntity2D()
	{
		Entity e = createEntity();

		attach<Transform2D>(e.getHandle());
		return e;
	}

	void Scene::setParent(Entity entity, Entity parent)
	{
		setParent(entity.getHandle(), parent.getHandle());
	}

	void Scene::setParent(entity_handle entity, entity_handle parent)
	{
		hierarchy_system->setParent(entity, parent);
	}

	Image* Scene::getMainCameraTexture()
	{
		if (main_camera_entity.valid() && (main_camera_entity.has<Camera>() || main_camera_entity.has<Camera2D>() || main_camera_entity.has<PixelCamera>()))
		{
			if (main_camera_entity.valid() && (main_camera_entity.has<Camera>()))
			{
				return main_camera_entity.get<Camera>()->getRenderTarget()->getFramebufferTexture(Graphics::getCurrentFrame());
			}
			if (main_camera_entity.valid() && (main_camera_entity.has<Camera2D>()))
			{
				return main_camera_entity.get<Camera2D>()->getRenderTarget()->getFramebufferTexture(Graphics::getCurrentFrame());
			}
			if (main_camera_entity.valid() && (main_camera_entity.has<PixelCamera>()))
			{
				return main_camera_entity.get<PixelCamera>()->getRenderTarget()->getFramebufferTexture(Graphics::getCurrentFrame());
			}
		}
		return nullptr;
	}
}
