#pragma once

#include "HBE.h"
#include "vector"

using namespace HBE;

class Raytracing {
public:
	static void onResolutionChange(RenderTarget *render_target) {
		Resources::get<RenderTarget>("rt")->setResolution(render_target->getResolution().x, render_target->getResolution().y);
		Resources::get<RaytracingPipelineInstance>("rtpi")->setTexture("image", Resources::get<RenderTarget>("rt"));
		Scene *scene = Application::getScene();
		Camera &camera = scene->getCameraEntity().get<Camera>();
		camera.render_target = Resources::get<RenderTarget>("rt");
		camera.calculateProjection();
	}

	static void onRender() {
		Entity camera_entity = Application::getScene()->getCameraEntity();
		Graphics::raytrace(
				*Resources::get<RootAccelerationStructure>("rootas"),
				*Resources::get<RaytracingPipelineInstance>("rtpi"),
				*Resources::get<RenderTarget>("rt"),
				camera_entity.get<Camera>().projection,
				glm::inverse(camera_entity.get<Transform>().world()));
	}


	static void init() {
		ShaderInfo ragen_shader_info{};
		ragen_shader_info.path = "shaders/raytracing/raygen.glsl";
		ragen_shader_info.stage = SHADER_STAGE_RAY_GEN;
		Shader *ray_gen_shader = Resources::createShader(ragen_shader_info, "raygen");

		ShaderInfo miss_shader_info{};
		miss_shader_info.path = "shaders/raytracing/miss.glsl";
		miss_shader_info.stage = SHADER_STAGE_RAY_MISS;
		Shader *miss_shader = Resources::createShader(miss_shader_info, "miss");

		ShaderInfo closest_hit_shader_info{};
		closest_hit_shader_info.path = "shaders/raytracing/closesthit.glsl";
		closest_hit_shader_info.stage = SHADER_STAGE_CLOSEST_HIT;
		Shader *closest_hit_shader = Resources::createShader(closest_hit_shader_info, "closesthit");

		ShaderInfo box_intersection_shader_info{};
		box_intersection_shader_info.path = "shaders/raytracing/intersect_box.glsl";
		box_intersection_shader_info.stage = SHADER_STAGE_INTERSECTION;
		Shader *box_intersection_shader = Resources::createShader(box_intersection_shader_info, "box_intersection");

		std::vector<Shader *> hit_shaders;
		hit_shaders.push_back(closest_hit_shader);
		hit_shaders.push_back(box_intersection_shader);

		RaytracingShaderGroup triangle_shader_group{};
		triangle_shader_group.closest_hit_shader_index = 0;

		RaytracingShaderGroup aabb_shader_group{};
		aabb_shader_group.closest_hit_shader_index = 0;
		aabb_shader_group.intersection_shader_index = 1;


		std::vector<RaytracingShaderGroup> shader_groups;
		shader_groups.push_back(aabb_shader_group);
		shader_groups.push_back(triangle_shader_group);

		RaytracingPipelineInfo raytracing_pipeline_info{};
		raytracing_pipeline_info.raygen_shader = ray_gen_shader;
		raytracing_pipeline_info.miss_shaders = &miss_shader;
		raytracing_pipeline_info.miss_shader_count = 1;
		raytracing_pipeline_info.hit_shaders = hit_shaders.data();
		raytracing_pipeline_info.hit_shader_count = hit_shaders.size();

		raytracing_pipeline_info.shader_group_count = shader_groups.size();
		raytracing_pipeline_info.shader_groups = shader_groups.data();

		RaytracingPipeline *raytracing_pipeline = Resources::createRaytracingPipeline(raytracing_pipeline_info, "raytracing_pipeline");


		AABBAccelerationStructureInfo aabb__acceleration_structure_info{};
		aabb__acceleration_structure_info.max = vec3(0.5, 0.5, 0.5);
		aabb__acceleration_structure_info.min = vec3(-0.5, -0.5, -0.5);

		AABBAccelerationStructure *aabb_acceleration_structure = Resources::createAABBAccelerationStructure(aabb__acceleration_structure_info, "aabbas");


		Log::message("Raytracing init");
		std::vector<VertexBindingInfo> vertex_binding_infos{};
		vertex_binding_infos.push_back(VertexBindingInfo{0, sizeof(vec3), VERTEX_BINDING_FLAG_NONE});
		MeshInfo mesh_info{};
		mesh_info.flags = MESH_FLAG_USED_IN_RAYTRACING;
		mesh_info.binding_infos = vertex_binding_infos.data();
		mesh_info.binding_info_count = vertex_binding_infos.size();

		Mesh *mesh = Resources::createMesh(mesh_info, "mesh");

		Geometry::createCube(*mesh, 2, 2, 2, VERTEX_FLAG_NONE);

		MeshAccelerationStructureInfo mesh_acceleration_structure_info{};
		mesh_acceleration_structure_info.mesh = mesh;
		MeshAccelerationStructure *mesh_acceleration_structure = Resources::createMeshAccelerationStructure(mesh_acceleration_structure_info, "meshas");


		std::vector<AccelerationStructureInstance> acceleration_structure_instances{};

		Transform transform_mesh{};
		transform_mesh.translate(vec3(0, 0, 5));

		Transform transform_aabb{};
		transform_aabb.translate(vec3(0, 0, -5));

		acceleration_structure_instances.push_back(AccelerationStructureInstance{0, transform_aabb.world(), ACCELERATION_STRUCTURE_TYPE_AABB});
		acceleration_structure_instances.push_back(AccelerationStructureInstance{0, transform_mesh.world(), ACCELERATION_STRUCTURE_TYPE_MESH});


		RootAccelerationStructureInfo root_acceleration_structure_info{};
		root_acceleration_structure_info.aabb_acceleration_structures = &aabb_acceleration_structure;
		root_acceleration_structure_info.aabb_acceleration_structure_count = 1;
		root_acceleration_structure_info.mesh_acceleration_structures = &mesh_acceleration_structure;
		root_acceleration_structure_info.mesh_acceleration_structure_count = 1;
		root_acceleration_structure_info.instances = acceleration_structure_instances.data();
		root_acceleration_structure_info.instance_count = acceleration_structure_instances.size();

		RootAccelerationStructure *root_acceleration_structure = Resources::createRootAccelerationStructure(root_acceleration_structure_info, "rootas");

		RenderTargetInfo render_target_info{};
		render_target_info.width = Graphics::getDefaultRenderTarget()->getResolution().x;
		render_target_info.height = Graphics::getDefaultRenderTarget()->getResolution().y;
		render_target_info.flags = RENDER_TARGET_FLAG_USED_IN_RAYTRACING;

		RenderTarget *render_target = Resources::createRenderTarget(render_target_info, "rt");
		Graphics::setRenderTarget(render_target);

		RaytracingPipelineInstanceInfo raytracing_pipeline_instance_info{};
		raytracing_pipeline_instance_info.raytracing_pipeline = raytracing_pipeline;
		raytracing_pipeline_instance_info.root_acceleration_structure = root_acceleration_structure;
		RaytracingPipelineInstance *pipeline_instance = Resources::createRaytracingPipelineInstance(raytracing_pipeline_instance_info, "rtpi");

		pipeline_instance->setTexture("image", render_target);
		Scene *scene = Application::getScene();

		scene->onRender.subscribe(onRender);

		auto group = scene->group<Camera>();

		for (auto[handle, camera]:group) {
			Log::message("Camera found");
		}

		Entity camera_entity = scene->createEntity();
		camera_entity.attach<Camera>();
		camera_entity.get<Camera>().render_target = Resources::get<RenderTarget>("rt");
		camera_entity.get<Camera>().calculateProjection();
		camera_entity.get<Camera>().active = false;
		scene->setCameraEntity(camera_entity);


		Graphics::getDefaultRenderTarget()->onResolutionChange.subscribe(onResolutionChange);
	}
};


