#include "TracedRendererSystem.h"

TracedRendererSystem::TracedRendererSystem(Scene *scene) : System(scene) {
	scene->onDraw.subscribe(this, &TracedRendererSystem::draw);


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
	uint32_t count=0;
	for (int i = -range; i < range; ++i) {
		for (int j = -range; j < range; ++j) {
			for (int k = -range; k < range; ++k) {
				transforms.emplace_back(mat4(1.0f));

				transforms[count]=glm::translate(transforms[count],vec3(i, j, k) * 2.0f);
				count++;
			}
		}
	}
	mesh->setInstanceBuffer(1, transforms.data(), transforms.size());
}

void TracedRendererSystem::draw() {
	Profiler::begin("CubeRendererUpdate");

	Graphics::drawInstanced(*mesh, *pipeline);

	Profiler::end();
}


TracedRendererSystem::~TracedRendererSystem() {
	delete mesh;
	delete vertex_shader;
	delete fragment_shader;
	delete pipeline;
	scene->onDraw.unsubscribe(this);
}