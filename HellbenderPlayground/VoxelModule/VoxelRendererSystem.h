#pragma once

#include "HBE.h"

using namespace HBE;
struct CubeRenderer {

};

class VoxelRendererSystem : public System {
	Mesh *mesh;
	Shader *fragment_shader;
	Shader *vertex_shader;
	GraphicPipeline *pipeline;
	std::vector<mat4> transforms;
	~VoxelRendererSystem();
public:
	void draw();
	VoxelRendererSystem(Scene *scene);
};
