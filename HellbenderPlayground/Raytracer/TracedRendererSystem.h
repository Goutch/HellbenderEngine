#pragma once

#include "HBE.h"

using namespace HBE;
struct CubeRenderer {

};

class TracedRendererSystem : public System {
	Mesh *mesh;
	Shader *fragment_shader;
	Shader *vertex_shader;
	GraphicPipeline *pipeline;
	std::vector<mat4> transforms;
	~TracedRendererSystem();
public:
	void draw();
	TracedRendererSystem(Scene *scene);
};
