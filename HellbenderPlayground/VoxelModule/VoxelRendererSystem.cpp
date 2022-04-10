#include "VoxelRendererSystem.h"

struct VoxelChunkInfo {
	alignas(16)uvec4 dimensions;
};

VoxelRendererSystem::VoxelRendererSystem(Scene *scene) : System(scene) {
	scene->onDraw.subscribe(this, &VoxelRendererSystem::draw);

	VertexBindingInfo binding_infos[2];
	binding_infos[0].flags = VERTEX_BINDING_FLAG_NONE;
	binding_infos[0].binding = 0;
	binding_infos[0].size = sizeof(vec3);
	binding_infos[1].binding = 1;
	binding_infos[1].flags = VERTEX_BINDING_FLAG_PER_INSTANCE | VERTEX_BINDING_FLAG_FAST_WRITE;
	binding_infos[1].size = sizeof(mat4);
	MeshInfo mesh_info{};
	mesh_info.flags = MESH_FLAG_NONE;
	mesh_info.binding_info_count = 2;
	mesh_info.binding_infos = binding_infos;
	mesh = Resources::createMesh(mesh_info);

	ShaderInfo vert_info{};
	ShaderInfo frag_info{};
	frag_info.stage = SHADER_STAGE_FRAGMENT;
	vert_info.stage = SHADER_STAGE_VERTEX;
	frag_info.path = "../../res/shaders/pathTracer/PathTracer.frag";
	vert_info.path = "../../res/shaders/pathTracer/PathTracer.vert";
	vertex_shader = Resources::createShader(vert_info);
	fragment_shader = Resources::createShader(frag_info);

	GraphicPipelineInfo pipeline_info{};
	pipeline_info.binding_infos = binding_infos;
	pipeline_info.binding_info_count = 2;
	pipeline_info.vertex_shader = vertex_shader;
	pipeline_info.fragement_shader = fragment_shader;
	pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_CULL_FRONT;
	pipeline = Resources::createGraphicPipeline(pipeline_info);

	Geometry::createCube(*mesh, 1, 1, 1, 0);

	int32_t range = 25; //10*10*10 = 1000 cubes
	uint32_t count = 0;
	for (int i = -range; i < range; ++i) {
		for (int j = -range; j < range; ++j) {
			for (int k = -range; k < range; ++k) {
				transforms.emplace_back(mat4(1.0f));

				transforms[count] = glm::translate(transforms[count], vec3(i, j, k));
				count++;
			}
		}
	}
	mesh->setInstanceBuffer(1, transforms.data(), transforms.size());


	TextureInfo texture_info{};
	texture_info.width = 64;
	texture_info.height = 64;
	texture_info.depth = 64;
	texture_info.flags = IMAGE_FLAG_NO_SAMPLER;
	texture_info.format = IMAGE_FORMAT_R8;
	auto raw_voxels = Resources::createTexture(texture_info, "voxels");
	std::vector<uint8_t> data;
	vec3 dimensions = vec3(raw_voxels->getWidth(), raw_voxels->getHeight(), raw_voxels->getDepth());
	for (int x = 0; x < raw_voxels->getWidth(); ++x) {
		for (int y = 0; y < raw_voxels->getHeight(); ++y) {
			for (int z = 0; z < raw_voxels->getDepth(); ++z) {
				float distance = glm::distance(vec3(x, y, z) - ((dimensions+vec3(1,1,1)) / 2.0f), vec3(0, 0, 0));
				if (distance < (dimensions.x / 2.0f))
					data.emplace_back(1);
				else
					data.emplace_back(0);
			}
		}
	}
	raw_voxels->update(data.data());

	VoxelChunkInfo voxel_info = {
			uvec4(raw_voxels->getWidth(),
				  raw_voxels->getHeight(),
				  raw_voxels->getDepth(),
				  0)};
	pipeline->setUniform("VoxelData", static_cast<void *>(&voxel_info));
	pipeline->setTexture("voxels", raw_voxels);
}

void VoxelRendererSystem::draw() {
	Profiler::begin("CubeRendererUpdate");

	Graphics::drawInstanced(*mesh, *pipeline);

	Profiler::end();
}


VoxelRendererSystem::~VoxelRendererSystem() {
	delete mesh;
	delete vertex_shader;
	delete fragment_shader;
	delete pipeline;
	scene->onDraw.unsubscribe(this);
}