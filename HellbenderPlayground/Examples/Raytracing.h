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
	uint32_t sample_count = 1;
	uint32_t max_bounces = 3;
};
#define HYSTORY_COUNT 8

class RaytracingScene {

private:
	RaytracingPipelineResources raytracing_resources;
	RaytracingPipelineResources pathtracing_resources;
	std::vector<Shader *> common_hit_shaders;
	RootAccelerationStructure *root_acceleration_structure;
	AABBAccelerationStructure *aabb_acceleration_structure;
	std::vector<Texture *> history_textures;
	Texture *output_texture = nullptr;
	bool use_pathtracing = false;
	Frame frame{};
public:
	void createFrameBuffers(uint32_t width, uint32_t height) {
		for (Texture *frame_buffer : history_textures) {
			delete frame_buffer;
		}
		if (output_texture != nullptr) {
			delete output_texture;
		}
		history_textures.clear();
		TextureInfo info{};
		info.width = width;
		info.height = height;
		info.format = IMAGE_FORMAT_RGBA32F;
		info.flags = IMAGE_FLAG_SHADER_WRITE |
					 IMAGE_FLAG_FILTER_NEAREST;

		for (uint32_t i = 0; i < HYSTORY_COUNT; i++) {
			history_textures.push_back(Resources::createTexture(info));
		}

		output_texture = Resources::createTexture(info);
	}

	void onResolutionChange(RenderTarget *rt) {
		createFrameBuffers(rt->getResolution().x, rt->getResolution().y);

		Scene *scene = Application::getScene();
		Camera &camera = scene->getCameraEntity().get<Camera>();
		camera.render_target = rt;
		camera.calculateProjection();
	}


	void onRender() {
		Entity camera_entity = Application::getScene()->getCameraEntity();
		frame.time = Application::getTime() * 0.05;

		RaytracingPipelineResources resources = use_pathtracing ? pathtracing_resources : raytracing_resources;


		const Texture *frame_buffer_array[HYSTORY_COUNT];
		for (uint32_t i = 0; i < HYSTORY_COUNT; i++) {
			uint32_t f = (frame.index - i) % history_textures.size();
			frame_buffer_array[i] = history_textures[f];
		}


		resources.pipeline_instance->setUniform("frame", &frame, Graphics::getCurrentFrame());
		resources.pipeline_instance->setTexture("outputImage", output_texture, Graphics::getCurrentFrame(), 0);
		resources.pipeline_instance->setTextureArray("history", frame_buffer_array, HYSTORY_COUNT, Graphics::getCurrentFrame(), 0);
		Graphics::raytrace(*root_acceleration_structure,
						   *resources.pipeline_instance,
						   camera_entity.get<Camera>().projection,
						   glm::inverse(camera_entity.get<Transform>().world()),
						   output_texture->getSize());

		/*	denoiser_instance->setTextureArray("history", frame_buffer_array, HYSTORY_COUNT, Graphics::getCurrentFrame(), 0);
			denoiser_instance->setTexture("outputImage", output_texture, Graphics::getCurrentFrame(), 0);
			denoiser_instance->dispatch(output_texture->getWidth(), output_texture->getHeight(), 1);
			denoiser_instance->wait();
	*/
		Graphics::present(output_texture);
		frame.index++;
	}

	void onUpdate(float delta) {
		if (Input::getKeyDown(KEY::P)) {
			use_pathtracing = !use_pathtracing;
		}
		if (Input::getKeyDown(KEY::R)) {
			frame.sample_count = 1;
		}
		if (Input::getKeyDown(KEY::UP)) {
			frame.sample_count++;
			Log::message("Sample count:" + std::to_string(frame.sample_count));
		}
		if (Input::getKeyDown(KEY::DOWN)) {
			if (frame.sample_count > 0) {
				frame.sample_count--;
			}
			Log::message("Sample count:" + std::to_string(frame.sample_count));
		}
		if (Input::getKeyDown(KEY::RIGHT)) {
			frame.max_bounces++;
			Log::message("Max bounces:" + std::to_string(frame.max_bounces));
		}
		if (Input::getKeyDown(KEY::LEFT)) {
			if (frame.max_bounces > 0) {
				frame.max_bounces--;
			}
			Log::message("Max bounces:" + std::to_string(frame.max_bounces));
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
		for (auto &history_texture : history_textures) {
			delete history_texture;
		}
		history_textures.clear();

		delete output_texture;
		delete raytracing_resources.raygen_shader;
		delete pathtracing_resources.raygen_shader;

		delete aabb_acceleration_structure;
		delete root_acceleration_structure;
		delete raytracing_resources.pipeline_instance;
		delete raytracing_resources.pipeline;
		delete pathtracing_resources.pipeline_instance;
		delete pathtracing_resources.pipeline;

	}

	RaytracingScene() {
		createFrameBuffers(Graphics::getDefaultRenderTarget()->getResolution().x, Graphics::getDefaultRenderTarget()->getResolution().y);
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
		transform_aabb_floor.setScale(vec3(50, 1, 50));
		transform_aabb_floor.translate(vec3(0, 10, 0));
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

		RaytracingPipelineInstanceInfo raytracing_pipeline_instance_info{};
		raytracing_pipeline_instance_info.raytracing_pipeline = raytracing_resources.pipeline;
		raytracing_pipeline_instance_info.root_acceleration_structure = root_acceleration_structure;
		raytracing_resources.pipeline_instance = Resources::createRaytracingPipelineInstance(raytracing_pipeline_instance_info);


		raytracing_pipeline_instance_info.raytracing_pipeline = pathtracing_resources.pipeline;
		raytracing_pipeline_instance_info.root_acceleration_structure = root_acceleration_structure;
		pathtracing_resources.pipeline_instance = Resources::createRaytracingPipelineInstance(raytracing_pipeline_instance_info);


		Scene *scene = Application::getScene();

		scene->onRender.subscribe(this, &RaytracingScene::onRender);
		scene->onUpdate.subscribe(this, &RaytracingScene::onUpdate);
		Entity camera_entity = scene->createEntity();
		camera_entity.attach<Camera>();
		camera_entity.get<Camera>().render_target = Graphics::getDefaultRenderTarget();
		camera_entity.get<Camera>().calculateProjection();
		camera_entity.get<Camera>().active = false;
		scene->setCameraEntity(camera_entity);

		Graphics::getDefaultRenderTarget()->onResolutionChange.subscribe(this, &RaytracingScene::onResolutionChange);
	}
};


