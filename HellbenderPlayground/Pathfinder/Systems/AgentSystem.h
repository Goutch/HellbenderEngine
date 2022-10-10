#pragma once

#include "HBE.h"


using namespace HBE;

struct Agent {
	f32 speed = f32(10);
	f32 size = f32(1);
	fixed3<f32> target_position;
};

class AgentSystem : public System {
	Mesh *mesh;
	GraphicPipeline *pipeline;
	GraphicPipelineInstance *material;
	Entity camera_entity;

public:
	void onUpdate(float delta);

	void onDraw();

	AgentSystem(Scene *scene);

	~AgentSystem();
};

