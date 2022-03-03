#include "Scene.h"
#include "core/graphics/Graphics.h"

namespace HBE {

	bool Entity::valid() {
		return scene->valid(handle);
	}

	entity_handle Entity::getHandle() {
		return handle;
	}

	Entity::Entity(const Entity &other) {
		this->handle = other.handle;
		this->scene = other.scene;
	}

	Entity::Entity(entity_handle handle, Scene *scene) {
		this->handle = handle;
		this->scene = scene;
	}

	Scene::Scene() {
		systems.push_back(new MeshRendererSystem(this));
		systems.push_back(new CameraSystem(this));
		systems.push_back(new CameraControllerSystem(this));
		systems.push_back(new InstancedRendererSystem(this));
		Graphics::getDefaultRenderTarget()->onResolutionChange.subscribe(this, &Scene::calculateCameraProjection);
	}


	Entity* Scene::getCameraEntity() {
		return &main_camera_entity;
	}

	void Scene::destroyEntity(Entity entity) {
		registry.destroy(entity.getHandle());

	}

	void Scene::calculateCameraProjection(RenderTarget *renderTarget) {

		if(main_camera_entity.has<Camera>())
		{
			main_camera_entity.get<Camera>().calculateProjection();
		}
		else if(main_camera_entity.has<Camera2D>())
		{
			main_camera_entity.get<Camera2D>().calculateProjection();
		}
	}

	void Scene::setCameraEntity(Entity camera) {
		main_camera_entity = camera;
		calculateCameraProjection(Graphics::getDefaultRenderTarget());
	}

	void Scene::update(float delta_t) {
		onUpdate.invoke(delta_t);
	}

	void Scene::draw() {
		onDraw.invoke();
	}

	void Scene::render() {
		onRender.invoke();
	}

	Scene::~Scene() {
		Graphics::getDefaultRenderTarget()->onResolutionChange.unsubscribe(this);
		for (System *system:systems) {
			delete system;
		}
	}

	void Scene::addSystem(System *system) {
		systems.emplace_back(system);
	}

	bool Scene::valid(entity_handle handle) {
		return registry.valid(handle);
	}


	Entity Scene::createEntity(const std::string &name) {
		Entity e = createEntity();
		Identity &identity = e.attach<Identity>();
		identity.name = name;
		return e;
	}


	Entity Scene::createEntity() {
		Entity e(registry.create(), this);
		attach<Transform>(e.getHandle());
		return e;
	}
}