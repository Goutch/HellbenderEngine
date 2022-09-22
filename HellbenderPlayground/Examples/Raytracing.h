#pragma once

#include "HBE.h"
#include "vector"

using namespace HBE;

struct RaytracingPipelineResources {
	RaytracingPipelineInstance *pipeline_instance;
	RaytracingPipeline *pipeline;
	Shader *raygen_shader;
	std::vector<Shader *> miss_shaders;
	std::vector<Shader *> hit_shaders;
	std::vector<RaytracingShaderGroup> shader_groups;
};

struct Frame {
	float time = 0;
	uint32_t index = 0;
};

class RaytracingScene {
private:
	RaytracingPipelineResources raytracing_resources;
	RaytracingPipelineResources pathtracing_resources;
	std::vector<Shader *> common_hit_shaders;
	RenderTarget *render_target;
	RootAccelerationStructure *root_acceleration_structure;
	AABBAccelerationStructure *aabb_acceleration_structure;
	bool use_pathtracing = false;
	Frame frame{};
public:
	void onResolutionChange(RenderTarget *rt) {
		render_target->setResolution(rt->getResolution().x, rt->getResolution().y);

		raytracing_resources.pipeline_instance->setTexture("image", render_target);
		pathtracing_resources.pipeline_instance->setTexture("image", render_target);

		Scene *scene = Application::getScene();
		Camera &camera = scene->getCameraEntity().get<Camera>();
		camera.render_target = render_target;
		camera.calculateProjection();
	}

	void onRender() {
		Entity camera_entity = Application::getScene()->getCameraEntity();
		frame.time = Application::getTime() * 0.05;


		RaytracingPipelineResources resources = use_pathtracing ? pathtracing_resources : raytracing_resources;


		resources.pipeline_instance->setUniform("frame", &frame);
		Graphics::raytrace(
				*root_acceleration_structure,
				*resources.pipeline_instance,
				*render_target,
				camera_entity.get<Camera>().projection,
				glm::inverse(camera_entity.get<Transform>().world()));
		frame.index++;
	}

	void onUpdate(float delta) {
		if (Input::getKeyDown(KEY::P)) {
			use_pathtracing = !use_pathtracing;
		}
	}

	~RaytracingScene() {
		for (Shader *shader : raytracing_resources.miss_shaders) {
			delete shader;
		}
		for (Shader *shader : pathtracing_resources.miss_shaders) {
			delete shader;
		}
		for (Shader *shader : raytracing_resources.hit_shaders) {
			delete shader;
		}
		for (Shader *shader : pathtracing_resources.hit_shaders) {
			delete shader;
		}
		delete raytracing_resources.raygen_shader;
		delete pathtracing_resources.raygen_shader;

		delete aabb_acceleration_structure;
		delete root_acceleration_structure;
		delete raytracing_resources.pipeline_instance;
		delete raytracing_resources.pipeline;
		delete pathtracing_resources.pipeline_instance;
		delete pathtracing_resources.pipeline;

		delete render_target;
	}

	RaytracingScene() {
		ShaderInfo shader_info{};


		shader_info.stage = SHADER_STAGE_RAY_GEN;
		shader_info.path = "shaders/raytracing/raygen.glsl";
		raytracing_resources.raygen_shader = Resources::createShader(shader_info);
		shader_info.path = "shaders/pathtracing/raygen_pathtrace.glsl";
		pathtracing_resources.raygen_shader = Resources::createShader(shader_info);


		shader_info.stage = SHADER_STAGE_RAY_MISS;
		shader_info.path = "shaders/raytracing/miss.glsl";
		raytracing_resources.miss_shaders.push_back(Resources::createShader(shader_info));
		shader_info.path = "shaders/raytracing/miss_occlusion.glsl";
		raytracing_resources.miss_shaders.push_back(Resources::createShader(shader_info));
		shader_info.path = "shaders/pathtracing/miss_pathtrace.glsl";
		pathtracing_resources.miss_shaders.push_back(Resources::createShader(shader_info));


		shader_info.stage = SHADER_STAGE_CLOSEST_HIT;
		shader_info.path = "shaders/raytracing/closesthit.glsl";
		raytracing_resources.hit_shaders.push_back(Resources::createShader(shader_info));
		shader_info.path = "shaders/raytracing/closest_hit_occlusion.glsl";
		raytracing_resources.hit_shaders.push_back(Resources::createShader(shader_info));
		shader_info.path = "shaders/pathtracing/closesthit_pathtrace.glsl";
		pathtracing_resources.hit_shaders.push_back(Resources::createShader(shader_info));


		shader_info.stage = SHADER_STAGE_INTERSECTION;
		shader_info.path = "shaders/raytracing/intersect_box.glsl";
		raytracing_resources.hit_shaders.push_back(Resources::createShader(shader_info));
		pathtracing_resources.hit_shaders.push_back(Resources::createShader(shader_info));
		shader_info.path = "shaders/raytracing/intersect_sphere.glsl";
		raytracing_resources.hit_shaders.push_back(Resources::createShader(shader_info));
		pathtracing_resources.hit_shaders.push_back(Resources::createShader(shader_info));


		raytracing_resources.shader_groups.push_back({0, -1, 2});//box
		raytracing_resources.shader_groups.push_back({0, -1, 3});//sphere
		raytracing_resources.shader_groups.push_back({1, -1, 2});//box ao
		raytracing_resources.shader_groups.push_back({1, -1, 3});//sphere ao

		pathtracing_resources.shader_groups.push_back({0, -1, 1});//box
		pathtracing_resources.shader_groups.push_back({0, -1, 2});//sphere



		RaytracingPipelineInfo raytracing_pipeline_info{};
		raytracing_pipeline_info.raygen_shader = raytracing_resources.raygen_shader;
		raytracing_pipeline_info.miss_shaders = raytracing_resources.miss_shaders.data();
		raytracing_pipeline_info.miss_shader_count = raytracing_resources.miss_shaders.size();
		raytracing_pipeline_info.hit_shaders = raytracing_resources.hit_shaders.data();
		raytracing_pipeline_info.hit_shader_count = raytracing_resources.hit_shaders.size();

		raytracing_pipeline_info.shader_group_count = raytracing_resources.shader_groups.size();
		raytracing_pipeline_info.shader_groups = raytracing_resources.shader_groups.data();
		raytracing_pipeline_info.max_recursion_depth = 2;

		raytracing_resources.pipeline = Resources::createRaytracingPipeline(raytracing_pipeline_info);


		raytracing_pipeline_info.raygen_shader = pathtracing_resources.raygen_shader;
		raytracing_pipeline_info.miss_shaders = pathtracing_resources.miss_shaders.data();
		raytracing_pipeline_info.miss_shader_count = pathtracing_resources.miss_shaders.size();
		raytracing_pipeline_info.hit_shaders = pathtracing_resources.hit_shaders.data();
		raytracing_pipeline_info.hit_shader_count = pathtracing_resources.hit_shaders.size();

		raytracing_pipeline_info.shader_group_count = pathtracing_resources.shader_groups.size();
		raytracing_pipeline_info.shader_groups = pathtracing_resources.shader_groups.data();
		raytracing_pipeline_info.max_recursion_depth = 16;

		pathtracing_resources.pipeline = Resources::createRaytracingPipeline(raytracing_pipeline_info);


		AABBAccelerationStructureInfo aabb__acceleration_structure_info{};
		aabb__acceleration_structure_info.max = vec3(0.5, 0.5, 0.5);
		aabb__acceleration_structure_info.min = vec3(-0.5, -0.5, -0.5);

		aabb_acceleration_structure = Resources::createAABBAccelerationStructure(aabb__acceleration_structure_info);

		std::vector<AABBAccelerationStructure *> aabb_acceleration_structures{aabb_acceleration_structure};

		std::vector<VertexBindingInfo> vertex_binding_infos{};
		vertex_binding_infos.push_back(VertexBindingInfo{0, sizeof(vec3), VERTEX_BINDING_FLAG_NONE});
		MeshInfo mesh_info{};
		mesh_info.flags = MESH_FLAG_USED_IN_RAYTRACING;
		mesh_info.binding_infos = vertex_binding_infos.data();
		mesh_info.binding_info_count = vertex_binding_infos.size();

		ModelInfo model_info{};
		model_info.path = "models/dragon.gltf";
		model_info.flags = MODEL_FLAG_DONT_LOAD_MATERIALS | MODEL_FLAG_USED_IN_RAYTRACING;

		std::vector<AccelerationStructureInstance> acceleration_structure_instances{};

		Transform transform_mesh{};
		transform_mesh.translate(vec3(0, 0, 5));

		Transform transform_aabb_sphere{};
		transform_aabb_sphere.translate(vec3(0, 0, -4));
		Transform transform_aabb_sphere2{};
		transform_aabb_sphere2.translate(vec3(0, 0, -6));

		Transform transform_aabb_cube{};
		transform_aabb_cube.translate(vec3(0, 0, -5));
		Transform transform_aabb_floor{};
		transform_aabb_floor.translate(vec3(0, -1, 0));
		transform_aabb_floor.setScale(vec3(100, 1, 100));

		acceleration_structure_instances.push_back(AccelerationStructureInstance{0, 0, transform_aabb_floor.world(), ACCELERATION_STRUCTURE_TYPE_AABB});

		//for (int i = 0; i < 1000; ++i) {
		//	Transform t{};
		//	t.translate(vec3(Random::floatRange(-50, 50), 0, Random::floatRange(-50, 50)));
		//	acceleration_structure_instances.push_back(
		//			AccelerationStructureInstance{0, 0, t.world(), ACCELERATION_STRUCTURE_TYPE_AABB});
		//}
		for (int i = 0; i < 1000; ++i) {
			Transform t{};
			t.translate(vec3(Random::floatRange(-50, 50), 0, Random::floatRange(-50, 50)));
			acceleration_structure_instances.push_back(
					AccelerationStructureInstance{0, 1, t.world(), ACCELERATION_STRUCTURE_TYPE_AABB});
		}

		RootAccelerationStructureInfo root_acceleration_structure_info{};
		root_acceleration_structure_info.aabb_acceleration_structures = aabb_acceleration_structures.data();
		root_acceleration_structure_info.aabb_acceleration_structure_count = aabb_acceleration_structures.size();
		//root_acceleration_structure_info.mesh_acceleration_structures = &mesh_acceleration_structure;
		//root_acceleration_structure_info.mesh_acceleration_structure_count = 1;
		root_acceleration_structure_info.instances = acceleration_structure_instances.data();
		root_acceleration_structure_info.instance_count = acceleration_structure_instances.size();

		root_acceleration_structure = Resources::createRootAccelerationStructure(root_acceleration_structure_info);

		RenderTargetInfo render_target_info{};
		render_target_info.width = Graphics::getDefaultRenderTarget()->getResolution().x;
		render_target_info.height = Graphics::getDefaultRenderTarget()->getResolution().y;
		render_target_info.flags = RENDER_TARGET_FLAG_USED_IN_RAYTRACING;

		render_target = Resources::createRenderTarget(render_target_info);
		Graphics::setRenderTarget(render_target);

		RaytracingPipelineInstanceInfo raytracing_pipeline_instance_info{};
		raytracing_pipeline_instance_info.raytracing_pipeline = raytracing_resources.pipeline;
		raytracing_pipeline_instance_info.root_acceleration_structure = root_acceleration_structure;
		raytracing_resources.pipeline_instance = Resources::createRaytracingPipelineInstance(raytracing_pipeline_instance_info);


		raytracing_pipeline_instance_info.raytracing_pipeline = pathtracing_resources.pipeline;
		raytracing_pipeline_instance_info.root_acceleration_structure = root_acceleration_structure;
		pathtracing_resources.pipeline_instance = Resources::createRaytracingPipelineInstance(raytracing_pipeline_instance_info);


		raytracing_resources.pipeline_instance->setTexture("image", render_target);
		pathtracing_resources.pipeline_instance->setTexture("image", render_target);

		Scene *scene = Application::getScene();

		scene->onRender.subscribe(this, &RaytracingScene::onRender);
		scene->onUpdate.subscribe(this, &RaytracingScene::onUpdate);
		Entity camera_entity = scene->createEntity();
		camera_entity.attach<Camera>();
		camera_entity.get<Camera>().render_target = render_target;
		camera_entity.get<Camera>().calculateProjection();
		camera_entity.get<Camera>().active = false;
		scene->setCameraEntity(camera_entity);


		Graphics::getDefaultRenderTarget()->onResolutionChange.subscribe(this, &RaytracingScene::onResolutionChange);
	}
};


