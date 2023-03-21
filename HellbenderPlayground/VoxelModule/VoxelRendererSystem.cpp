#include "VoxelRendererSystem.h"

struct VoxelChunkInfo {
	alignas(16)uvec3 resolution;
	alignas(16)vec3 size;
};

VoxelRendererSystem::VoxelRendererSystem(Scene *scene) : System(scene) {
	scene->onDraw.subscribe(this, &VoxelRendererSystem::draw);


	ShaderInfo compute_info{};
	compute_info.path = "shaders/pathTracer/MipGeneration.comp";
	compute_info.stage = SHADER_STAGE_COMPUTE;

	Shader *compute_shader = Resources::createShader(compute_info, "MipMapperShader");
	ComputePipelineInfo compute_pipeline_info{};
	compute_pipeline_info.compute_shader = compute_shader;
	compute_pipeline_info.flags = COMPUTE_PIPELINE_FLAG_NONE;
	compute_pipeline = Resources::createComputePipeline(compute_pipeline_info, "MipMapperPipeline");

	ComputeInstanceInfo compute_instance_info{};
	compute_instance_info.compute_pipeline = compute_pipeline;
	compute_instance_info.flags = COMPUTE_INSTANCE_FLAG_NONE;
	compute_instance = Resources::createComputeInstance(compute_instance_info, "MipMapperInstance");

	VertexAttributeInfo binding_infos[2];
	binding_infos[0].flags = VERTEX_ATTRIBUTE_FLAG_NONE;
	binding_infos[0].binding = 0;
	binding_infos[0].size = sizeof(vec3);
	binding_infos[1].binding = 1;
	binding_infos[1].flags = VERTEX_ATTRIBUTE_FLAG_PER_INSTANCE;
	binding_infos[1].size = sizeof(mat4);
	MeshInfo mesh_info{};
	mesh_info.flags = MESH_FLAG_NONE;
	mesh_info.attribute_info_count = 2;
	mesh_info.attribute_infos = binding_infos;
	mesh = Resources::createMesh(mesh_info);

	ShaderInfo vert_info{};
	ShaderInfo frag_info{};
	frag_info.stage = SHADER_STAGE_FRAGMENT;
	vert_info.stage = SHADER_STAGE_VERTEX;
	frag_info.path = "shaders/pathTracer/SamplerPathTracer.frag";
	vert_info.path = "shaders/pathTracer/PathTracer.vert";
	vertex_shader = Resources::createShader(vert_info);
	fragment_shader = Resources::createShader(frag_info);

	GraphicPipelineInfo pipeline_info{};
	pipeline_info.attribute_infos = binding_infos;
	pipeline_info.attribute_info_count = 2;
	pipeline_info.vertex_shader = vertex_shader;
	pipeline_info.fragement_shader = fragment_shader;
	pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_CULL_FRONT;
	pipeline = Resources::createGraphicPipeline(pipeline_info);

	GraphicPipelineInstanceInfo material_info{};
	material_info.graphic_pipeline = pipeline;
	material_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;
	material = Resources::createGraphicPipelineInstance(material_info);

	Geometry::createCube(*mesh, 1, 1, 1, 0);

	int32_t range = 10; //10*10*10 = 1000 cubes
	uint32_t count = 0;
	for (int i = -range; i < range; ++i) {
		for (int j = -range; j < range; ++j) {
			for (int k = -range; k < range; ++k) {
				transforms.emplace_back(mat4(1.0f));
				transforms[count] = glm::translate(transforms[count], vec3(i, j, k) * vec3(2.0f));
				transforms[count] = glm::scale(transforms[count], vec3(2, 2, 2));
				count++;
			}
		}
	}
	mesh->setInstanceBuffer(1, transforms.data(), transforms.size());

	TextureInfo texture_info{};
	texture_info.width = 64;
	texture_info.height = 64;
	texture_info.depth = 64;
	//texture_info.flags = IMAGE_FLAG_NO_SAMPLER;
	texture_info.flags = IMAGE_FLAG_SHADER_WRITE | IMAGE_FLAG_FILTER_NEAREST;
	texture_info.format = IMAGE_FORMAT_R8;
	texture_info.generate_mip_maps = true;
	auto raw_voxels = Resources::createTexture(texture_info, "voxels");
	std::vector<uint8_t> data;
	vec3 resoluton = vec3(raw_voxels->getWidth(), raw_voxels->getHeight(), raw_voxels->getDepth());
	for (int x = 0; x < raw_voxels->getWidth(); ++x) {
		for (int y = 0; y < raw_voxels->getHeight(); ++y) {
			for (int z = 0; z < raw_voxels->getDepth(); ++z) {

				float distance = glm::distance(vec3(x + 0.5, y + 0.5, z + 0.5) - (resoluton / 2.0f), vec3(0, 0, 0));
				if (distance <= (resoluton.x / 2.0f))
					//if (z == 7 && y ==7)
					data.emplace_back(1);
				else
					data.emplace_back(0);
			}
		}
	}
	raw_voxels->update(data.data());
	compute_instance->setTexture("inTexture", raw_voxels, 0, 0);
	compute_instance->setTexture("outTexture", raw_voxels, 0, 1);
	compute_instance->dispatch(raw_voxels->getWidth() / 2, raw_voxels->getHeight() / 2, raw_voxels->getWidth() / 2);
	compute_instance->wait();
	compute_instance->setTexture("inTexture", raw_voxels, 0, 1);
	compute_instance->setTexture("outTexture", raw_voxels, 0, 2);
	compute_instance->dispatch(raw_voxels->getWidth() / 4, raw_voxels->getHeight() / 4, raw_voxels->getWidth() / 4);
	compute_instance->wait();
	compute_instance->setTexture("inTexture", raw_voxels, 0, 2);
	compute_instance->setTexture("outTexture", raw_voxels, 0, 3);
	compute_instance->dispatch(raw_voxels->getWidth() / 8, raw_voxels->getHeight() / 8, raw_voxels->getWidth() / 8);
	compute_instance->wait();
	VoxelChunkInfo voxel_info = {
			uvec3(raw_voxels->getWidth(),
				  raw_voxels->getHeight(),
				  raw_voxels->getDepth()),
			vec3(2.0f)};
	material->setUniform("VoxelData", static_cast<void *>(&voxel_info), -1);
	material->setTexture("voxels", raw_voxels, 0, 0);
	//pipeline->setTexture("voxels0", raw_voxels, 0);
	//pipeline->setTexture("voxels1", raw_voxels, 1);
	//pipeline->setTexture("voxels2", raw_voxels, 2);
	//pipeline->setTexture("voxels3", raw_voxels, 3);
}

void VoxelRendererSystem::draw(RenderGraph* render_graph) {
	HB_PROFILE_BEGIN("CubeRendererUpdate");

	DrawCmdInfo draw_cmd{};
	draw_cmd.pipeline_instance = material;
	draw_cmd.mesh = mesh;

	render_graph->draw(draw_cmd);

	HB_PROFILE_END("CubeRendererUpdate");
}


VoxelRendererSystem::~VoxelRendererSystem() {
	delete mesh;
	delete vertex_shader;
	delete fragment_shader;
	delete material;
	delete pipeline;
	scene->onDraw.unsubscribe(this);
}