
#include "AgentSystem.h"
#include "SimTransformSystem.h"

void AgentSystem::onUpdate(float delta) {
	auto group = scene->group<SimTransform, Agent>();
	for (auto[entity, simTransform, agent] :group) {
		std::string target_string = agent.target_position.toString();
		fixed3<f32> distance = agent.target_position - simTransform.position;
		std::string dist_string = distance.toString();
		if (distance.pow2Magnitude() != f32(0)) {
			f32 distance_mag = distance.magnitude();
			if (distance_mag != f32(0)) {
				fixed3<f32> direction = distance.normalized();
				if (f32(agent.speed * f32(delta)) >= distance_mag) {
					simTransform.position.x += distance.x;
					simTransform.position.y += distance.y;
				} else {
					std::string dir_string = direction.toString();
					simTransform.position.x += direction.x * agent.speed * f32(delta);
					simTransform.position.y += direction.y * agent.speed * f32(delta);
				}

				simTransform.rotation = f32::atan2(direction.y, direction.x) - f32::HALF_PI;

			}
		}
	}
}

void AgentSystem::onDraw(RenderGraph* render_graph) {
	std::vector<mat4> transforms;
	uint32_t count = 0;
	auto group = scene->group<Agent, Transform>();
	for (auto[entity, agent, transform] :group) {
		count++;
		transforms.emplace_back(transform.world());
	}

	mesh->setInstanceBuffer(1, transforms.data(), count);
	DrawCmdInfo draw_cmd{};
	draw_cmd.mesh = mesh;
	draw_cmd.pipeline_instance = material;
    render_graph->draw(draw_cmd);
}

AgentSystem::AgentSystem(Scene *scene) : System(scene) {
	camera_entity = scene->getCameraEntity();
	ShaderInfo frag_info{SHADER_STAGE_FRAGMENT, "/shaders/defaults/VK.frag"};
	ShaderInfo vert_info{SHADER_STAGE_VERTEX, "/shaders/defaults/VK_instanced.vert"};
	auto frag = Resources::createShader(frag_info, "frag");
	auto vert = Resources::createShader(vert_info, "vert");

	std::vector<VertexAttributeInfo> attribute_infos;
	//vertex binding
	attribute_infos.emplace_back();
	attribute_infos[0].binding = 0;
	attribute_infos[0].size = sizeof(vec3) + sizeof(vec2);
	//instance binding
	attribute_infos.emplace_back();
	attribute_infos[1].binding = 1;
	attribute_infos[1].size = sizeof(mat4);
	attribute_infos[1].flags = VERTEX_ATTRIBUTE_FLAG_PER_INSTANCE |
							   VERTEX_ATTRIBUTE_FLAG_FAST_WRITE | //Use host visible memory.
							   VERTEX_ATTRIBUTE_FLAG_MULTIPLE_BUFFERS;//We are updating this binding every frame so having multiple buffers is nice.

	GraphicPipelineInfo pipeline_info{};
	pipeline_info.attribute_infos = attribute_infos.data();
	pipeline_info.attribute_info_count = attribute_infos.size();
	pipeline_info.fragement_shader = frag;
	pipeline_info.vertex_shader = vert;
	pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_NONE;

	pipeline = Resources::createGraphicPipeline(pipeline_info, "pipeline");

	GraphicPipelineInstanceInfo material_info{};
	material_info.graphic_pipeline = pipeline;
	material_info.flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;
	material = Resources::createGraphicPipelineInstance(material_info, "material");

	MeshInfo mesh_info{};
	mesh_info.attribute_infos = attribute_infos.data();
	mesh_info.attribute_info_count = attribute_infos.size();
	mesh_info.flags = MESH_FLAG_NONE;

	mesh = Resources::createMesh(mesh_info, "mesh");
	float size_x = 1;
	float size_y = 1;
	std::vector<float> vertices =
			{
					-0.5f * size_x, (-1.0f * size_y) * (sqrt(3.0f) / 4), 0.0f, 1.0f, 0.0f,
					0.5f * size_x, (-1.0f * size_y) * (sqrt(3.0f) / 4), 0.0f, 1.0f, 1.0f,
					0.0f, 1.0f * size_y * (sqrt(3.0f) / 4.0f), 0.0f, 0.0f, 1.0f,
			};
	mesh->setBuffer(0, vertices.data(), 3);
	std::vector<uint32_t> indices = {0, 1, 2};
	mesh->setVertexIndices(indices.data(), indices.size());

	scene->onUpdate.subscribe(this, &AgentSystem::onUpdate);
	scene->onDraw.subscribe(this, &AgentSystem::onDraw);
}

AgentSystem::~AgentSystem() {
	scene->onUpdate.unsubscribe(this);
	scene->onDraw.unsubscribe(this);
}

