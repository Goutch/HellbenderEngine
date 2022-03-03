#pragma once

#include "HBE.h"
#include "TracedRendererSystem.h"
using namespace HBE;
class Raytracer {
public :
	static void init()
	{
		//-------------------RESOURCES CREATION--------------------------------------
		ShaderInfo frag_info{SHADER_STAGE_FRAGMENT, "../../res/shaders/pathTracer/PathTracer.frag"};
		ShaderInfo vert_info{SHADER_STAGE_VERTEX, "../../res/shaders/pathTracer/PathTracer.vert"};
		auto frag = Resources::createShader(frag_info, "frag");
		auto vert = Resources::createShader(vert_info, "vert");


		std::vector<VertexBindingInfo> binding_infos;
		//vertex binding
		binding_infos.emplace_back();
		binding_infos[0].binding = 0;
		binding_infos[0].size = sizeof(vec3);
		//instance binding
		binding_infos.emplace_back();
		binding_infos[1].binding = 1;
		binding_infos[1].size = sizeof(mat4);
		binding_infos[1].flags = VERTEX_BINDING_FLAG_PER_INSTANCE |
								 VERTEX_BINDING_FLAG_FAST_WRITE |//Use host visible memory.
								 VERTEX_BINDING_FLAG_MULTIPLE_BUFFERS;//We are updating this binding every frame so having multiple buffers is nice.

		GraphicPipelineInfo pipeline_info{};
		pipeline_info.binding_infos = binding_infos.data();
		pipeline_info.binding_info_count = binding_infos.size();
		pipeline_info.fragement_shader = frag;
		pipeline_info.vertex_shader = vert;
		pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_CULL_FRONT;

		auto pipeline = Resources::createGraphicPipeline(pipeline_info, "pipeline");

		MeshInfo mesh_info{};
		mesh_info.binding_infos = binding_infos.data();
		mesh_info.binding_info_count = binding_infos.size();
		mesh_info.flags = MESH_FLAG_NONE;

		auto mesh = Resources::createMesh(mesh_info, "mesh");

		Geometry::createCube(*mesh, 1, 1, 1,0);


		//-------------------SCENE CREATION--------------------------------------
		Scene &scene = *(new Scene());
		scene.addSystem(new TracedRendererSystem(&scene));
		Application::setScene(&scene, true);

		Entity camera_entity = scene.createEntity("camera");
		Camera &camera = camera_entity.attach<Camera>();
		camera.render_target = Graphics::getDefaultRenderTarget();
		camera_entity.attach<CameraController>();

		scene.setCameraEntity(camera_entity);


	}
};
