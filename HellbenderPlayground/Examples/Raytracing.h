#pragma once

#include "HBE.h"
#include "vector"
#include "array"
#include "NRD.h"

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
	float scattering_multiplier = 12.0f;
	float density_falloff = 10.0f;
	int use_blue_noise = 0;
};
#define HYSTORY_COUNT 8

struct MaterialData {
	vec4 albedo;
	vec4 emission;
	alignas(16) float roughness;
};

class RaytracingScene {

private:
	enum RENDER_MODE {
		DENOISED = 0,
		ALBEDO = 1,
		NORMAL = 2,
		MOTION = 3,
	};
	RaytracingPipelineResources pathtracing_resources;
	std::vector<Shader *> common_hit_shaders;
	RootAccelerationStructure *root_acceleration_structure;
	AABBAccelerationStructure *aabb_acceleration_structure;
	AABBAccelerationStructure *mesh_acceleration_structure;
	StorageBuffer *material_buffer;
	const std::array<MaterialData, 7> materials = {{
														   {vec4(0.8, 0.8, 0.8, 0.0), vec4(0.0, 0.0, 0.0, 0.0), 1.0},
														   {vec4(0.5, 0.5, 0.5, 0.0), vec4(1.0, 1.0, 1.0, 1.0), 1.0},
														   {vec4(1.0, 0.1, 0.1, 0.0), vec4(0.0, 0.0, 0.0, 0.0), 1.0},
														   {vec4(0.1, 0.5, 0.1, 0.0), vec4(0.0, 0.0, 0.0, 0.0), 1.0},
														   {vec4(0.1, 0.1, 0.5, 0.0), vec4(0.0, 0.0, 0.0, 0.0), 1.0},
														   {vec4(0.2, 0.2, 0.2, 0.0), vec4(0.0, 0.0, 0.0, 0.0), 0.4},
														   {vec4(0.8, 0.8, 0.8, 0.0), vec4(0.0, 0.0, 0.0, 0.0), 0.0}}
	};
	std::vector<Texture *> history_albedo;
	std::vector<Texture *> history_normal_depth;
	std::vector<Texture *> history_motion;
	Texture *blue_noise;
	Texture *output_texture = nullptr;
	Frame frame{};
	bool paused = false;
	RENDER_MODE render_mode = DENOISED;

	mat4 last_camera_matrices[2] = {mat4(1.0), mat4(1.0)};
public:
	void createFrameBuffers(uint32_t width, uint32_t height) {
		for (Texture *albedo : history_albedo) {
			delete albedo;
		}
		for (Texture *normal_depth : history_normal_depth) {
			delete normal_depth;
		}
		for (Texture *motion : history_motion) {
			delete motion;
		}
		if (output_texture != nullptr) {
			delete output_texture;
		}
		history_albedo.clear();
		history_normal_depth.clear();
		history_motion.clear();

		TextureInfo info{};
		info.width = width;
		info.height = height;
		info.format = IMAGE_FORMAT_RGBA32F;
		info.flags = IMAGE_FLAG_SHADER_WRITE |
					 IMAGE_FLAG_FILTER_NEAREST;

		for (uint32_t i = 0; i < HYSTORY_COUNT; i++) {
			history_albedo.push_back(Resources::createTexture(info));
			history_normal_depth.push_back(Resources::createTexture(info));
			history_motion.push_back(Resources::createTexture(info));
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
		if (!paused)
			frame.time = Application::getTime() * 0.05;

		RaytracingPipelineResources resources = pathtracing_resources;


		const Texture *albedo_history_buffer[HYSTORY_COUNT];
		const Texture *normal_depth_history_buffer[HYSTORY_COUNT];
		const Texture *motion_history_buffer[HYSTORY_COUNT];

		for (uint32_t i = 0; i < HYSTORY_COUNT; i++) {
			uint32_t f = (frame.index - i) % HYSTORY_COUNT;
			albedo_history_buffer[i] = history_albedo[f];
			normal_depth_history_buffer[i] = history_normal_depth[f];
			motion_history_buffer[i] = history_motion[f];
		}

		mat4 camera_projection = camera_entity.get<Camera>().projection;
		mat4 camera_view = glm::inverse(camera_entity.get<Transform>().world());

		resources.pipeline_instance->setUniform("frame", &frame, Graphics::getCurrentFrame());
		resources.pipeline_instance->setTexture("outputAlbedo", output_texture, Graphics::getCurrentFrame(), 0);
		resources.pipeline_instance->setTextureArray("historyAlbedo", albedo_history_buffer, HYSTORY_COUNT, Graphics::getCurrentFrame(), 0);
		resources.pipeline_instance->setTextureArray("historyNormalDepth", normal_depth_history_buffer, HYSTORY_COUNT, Graphics::getCurrentFrame(), 0);
		resources.pipeline_instance->setTextureArray("historyMotion", motion_history_buffer, HYSTORY_COUNT, Graphics::getCurrentFrame(), 0);
		resources.pipeline_instance->setTexture("blueNoise", blue_noise, Graphics::getCurrentFrame(), 0);

		resources.pipeline_instance->setUniform("last_cam", &last_camera_matrices, Graphics::getCurrentFrame());
		Graphics::raytrace(*root_acceleration_structure,
						   *resources.pipeline_instance,
						   camera_projection,
						   camera_view,
						   output_texture->getSize());

		last_camera_matrices[0] = camera_view;
		last_camera_matrices[1] = camera_projection;
		switch (render_mode) {
			case DENOISED:
				Graphics::present(output_texture);
				break;
			case ALBEDO:
				Graphics::present(albedo_history_buffer[0]);
				break;
			case NORMAL:
				Graphics::present(normal_depth_history_buffer[0]);
				break;
			case MOTION:
				Graphics::present(motion_history_buffer[0]);
				break;
		}

		frame.index++;
	}

	void onUpdate(float delta) {
		if (Input::getKeyDown(KEY::NUMBER_0)) {
			frame.use_blue_noise = !bool(frame.use_blue_noise);
		}
		if (Input::getKeyDown(KEY::NUMBER_1)) {
			render_mode = DENOISED;
		}
		if (Input::getKeyDown(KEY::NUMBER_2)) {
			render_mode = ALBEDO;
		}
		if (Input::getKeyDown(KEY::NUMBER_3)) {
			render_mode = NORMAL;
		}
		if (Input::getKeyDown(KEY::NUMBER_4)) {
			render_mode = MOTION;
		}
		if (Input::getKeyDown(KEY::P)) {
			paused = !paused;
		}
		if (Input::getKey(KEY::MINUS)) {
			frame.scattering_multiplier -= 5.0f * delta;
			Log::message("Scattering multiplier:" + std::to_string(frame.scattering_multiplier));
		}
		if (Input::getKey(KEY::EQUAL)) {
			frame.scattering_multiplier += 5.0f * delta;

			Log::message("Scattering multiplier:" + std::to_string(frame.scattering_multiplier));
		}
		if (Input::getKey(KEY::LEFT_BRACKET)) {
			frame.density_falloff -= 5.0f * delta;
			Log::message("Density falloff:" + std::to_string(frame.density_falloff));
		}
		if (Input::getKey(KEY::RIGHT_BRACKET)) {
			frame.density_falloff += 5.0f * delta;
			Log::message("Density falloff:" + std::to_string(frame.density_falloff));
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

		for (Shader *shader : pathtracing_resources.miss_shaders) {
			delete shader;
		}
		for (Shader *shader : pathtracing_resources.hit_shaders) {
			delete shader;
		}
		for (Texture *albedo : history_albedo) {
			delete albedo;
		}
		for (Texture *normal_depth : history_normal_depth) {
			delete normal_depth;
		}
		for (Texture *motion : history_motion) {
			delete motion;
		}
		if (output_texture != nullptr) {
			delete output_texture;
		}
		history_albedo.clear();
		history_normal_depth.clear();
		history_motion.clear();
		delete pathtracing_resources.raygen_shader;
		delete blue_noise;
		delete material_buffer;
		//delete mesh_acceleration_structure;
		delete aabb_acceleration_structure;
		delete root_acceleration_structure;
		delete pathtracing_resources.pipeline_instance;
		delete pathtracing_resources.pipeline;

	}

	RaytracingScene() {
		createFrameBuffers(Graphics::getDefaultRenderTarget()->getResolution().x, Graphics::getDefaultRenderTarget()->getResolution().y);

		blue_noise = Texture::load("/textures/BlueNoiseRGB.png", IMAGE_FORMAT_RGBA8, IMAGE_FLAG_FILTER_NEAREST | IMAGE_FLAG_NO_SAMPLER);

		ShaderInfo shader_info{};

		shader_info.stage = SHADER_STAGE_RAY_GEN;
		shader_info.path = "shaders/pathtracing/raygen_pathtrace.glsl";
		pathtracing_resources.raygen_shader = Resources::createShader(shader_info);


		shader_info.stage = SHADER_STAGE_RAY_MISS;
		shader_info.path = "shaders/pathtracing/miss_pathtrace.glsl";
		pathtracing_resources.miss_shaders.push_back(Resources::createShader(shader_info));


		shader_info.stage = SHADER_STAGE_CLOSEST_HIT;
		shader_info.path = "shaders/pathtracing/closesthit_aabb.glsl";
		pathtracing_resources.hit_shaders.push_back(Resources::createShader(shader_info));


		shader_info.stage = SHADER_STAGE_INTERSECTION;
		shader_info.path = "shaders/pathtracing/intersect_box.glsl";
		pathtracing_resources.hit_shaders.push_back(Resources::createShader(shader_info));
		shader_info.path = "shaders/pathtracing/intersect_sphere.glsl";
		pathtracing_resources.hit_shaders.push_back(Resources::createShader(shader_info));

		pathtracing_resources.shader_groups.push_back({0, -1, 1});//box
		pathtracing_resources.shader_groups.push_back({0, -1, 2});//sphere

		RaytracingPipelineInfo pathtracing_pipeline_info{};

		pathtracing_pipeline_info.raygen_shader = pathtracing_resources.raygen_shader;
		pathtracing_pipeline_info.miss_shaders = pathtracing_resources.miss_shaders.data();
		pathtracing_pipeline_info.miss_shader_count = pathtracing_resources.miss_shaders.size();
		pathtracing_pipeline_info.hit_shaders = pathtracing_resources.hit_shaders.data();
		pathtracing_pipeline_info.hit_shader_count = pathtracing_resources.hit_shaders.size();

		pathtracing_pipeline_info.shader_group_count = pathtracing_resources.shader_groups.size();
		pathtracing_pipeline_info.shader_groups = pathtracing_resources.shader_groups.data();
		pathtracing_pipeline_info.max_recursion_depth = 16;

		pathtracing_resources.pipeline = Resources::createRaytracingPipeline(pathtracing_pipeline_info);

		AABBAccelerationStructureInfo aabb__acceleration_structure_info{};
		aabb__acceleration_structure_info.max = vec3(0.5, 0.5, 0.5);
		aabb__acceleration_structure_info.min = vec3(-0.5, -0.5, -0.5);

		aabb_acceleration_structure = Resources::createAABBAccelerationStructure(aabb__acceleration_structure_info);

		std::vector<AABBAccelerationStructure *> aabb_acceleration_structures{aabb_acceleration_structure};

		/*std::vector<StorageBuffer *> storage_buffer_array(output_texture->getWidth(), nullptr);
		for (int i = 0; i < output_texture->getWidth(); ++i) {
			StorageBufferInfo storageBufferInfo{};
			storageBufferInfo.count = output_texture->getHeight();
			storageBufferInfo.stride = sizeof(vec4);
			std::vector<vec4> data(output_texture->getHeight(), vec4(0, 0, 0, 0));
			for (int j = 0; j < output_texture->getHeight(); ++j) {
				data[j] = vec4(Random::floatRange(0, 1), Random::floatRange(0, 1), Random::floatRange(0, 1), 1);
			}
			storage_buffer_array[i] = Resources::createStorageBuffer(storageBufferInfo);
			storage_buffer_array[i]->update(data.data());
		}*/

		/*std::vector<VertexBindingInfo> vertex_binding_infos{};
		vertex_binding_infos.push_back(VertexBindingInfo{0, sizeof(vec3), VERTEX_BINDING_FLAG_NONE});

		MeshInfo mesh_info{};
		mesh_info.flags = MESH_FLAG_USED_IN_RAYTRACING;
		mesh_info.binding_infos = vertex_binding_infos.data();
		mesh_info.binding_info_count = vertex_binding_infos.size();

		ModelInfo model_info{};
		model_info.path = "models/dragon.gltf";
		model_info.flags = MODEL_FLAG_DONT_LOAD_MATERIALS | MODEL_FLAG_USED_IN_RAYTRACING;
		MeshAccelerationStructureInfo mesh_acceleration_structure_info{};

		std::vector<MeshAccelerationStructure *> mesh_acceleration_structures{};*/

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

		acceleration_structure_instances.push_back(AccelerationStructureInstance{0, 0, transform_aabb_floor.world(), ACCELERATION_STRUCTURE_TYPE_AABB, 0});
		transform_aabb_floor.setScale(vec3(50, 1, 50));

		for (uint32_t i = 0; i < 7; ++i) {
			Transform t{};
			t.translate(vec3(2 * (i + 1), 0, 0));
			acceleration_structure_instances.push_back(
					AccelerationStructureInstance{0, 0, t.world(), ACCELERATION_STRUCTURE_TYPE_AABB, i});
		}
		for (uint32_t i = 0; i < 7; ++i) {
			Transform t{};
			t.translate(vec3(0, 0, 2 * (i + 1)));
			acceleration_structure_instances.push_back(
					AccelerationStructureInstance{0, 1, t.world(), ACCELERATION_STRUCTURE_TYPE_AABB, i % 7});
		}

		RootAccelerationStructureInfo root_acceleration_structure_info{};
		root_acceleration_structure_info.aabb_acceleration_structures = aabb_acceleration_structures.data();
		root_acceleration_structure_info.aabb_acceleration_structure_count = aabb_acceleration_structures.size();
		//root_acceleration_structure_info.mesh_acceleration_structures = &mesh_acceleration_structure;
		//root_acceleration_structure_info.mesh_acceleration_structure_count = 1;
		root_acceleration_structure_info.instances = acceleration_structure_instances.data();
		root_acceleration_structure_info.instance_count = acceleration_structure_instances.size();

		root_acceleration_structure = Resources::createRootAccelerationStructure(root_acceleration_structure_info);


		StorageBufferInfo storage_buffer_info{};
		storage_buffer_info.stride = sizeof(MaterialData);
		storage_buffer_info.count = materials.size();
		storage_buffer_info.flags = STORAGE_BUFFER_FLAG_NONE;
		material_buffer = Resources::createStorageBuffer(storage_buffer_info);
		material_buffer->update(materials.data());

		RaytracingPipelineInstanceInfo pathtracing_pipeline_instance_info{};
		pathtracing_pipeline_instance_info.raytracing_pipeline = pathtracing_resources.pipeline;
		pathtracing_resources.pipeline_instance = Resources::createRaytracingPipelineInstance(pathtracing_pipeline_instance_info);
		pathtracing_resources.pipeline_instance->setAccelerationStructure("topLevelAS", root_acceleration_structure);
		pathtracing_resources.pipeline_instance->setStorageBuffer("materials", material_buffer);
		//StorageBuffer **pbuffers = storage_buffer_array.data();
		//pathtracing_resources.pipeline_instance->setStorageBufferArray("vertices", pbuffers, storage_buffer_array.size());

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


		/*nrd::MemoryAllocatorInterface *allocator;
		nrd::DenoiserCreationDesc denoiserCreationDesc{};
		denoiserCreationDesc.enableValidation = true;
		denoiserCreationDesc.memoryAllocatorInterface = *allocator;

		nrd::Denoiser *denoiser;

		nrd::CreateDenoiser(denoiserCreationDesc, denoiser);
		nrd::DestroyDenoiser(denoiser);

		nrd::*/

	}
};


