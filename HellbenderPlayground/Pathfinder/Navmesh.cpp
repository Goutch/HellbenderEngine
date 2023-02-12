#include "Navmesh.h"

void Navmesh::draw(RenderGraph* render_graph) {

	if (!vertices.empty()) {
		std::vector<vec2> mesh_vertices;
		std::vector<uint32_t> mesh_indices;

		for (const auto &v:vertices) {
			mesh_vertices.emplace_back(v.position.x.toFloat(), v.position.y.toFloat());
		}
		for (const auto &s:segments) {
			mesh_indices.emplace_back(s.v1);
			mesh_indices.emplace_back(s.v2);
		}
		if (mesh->getIndexCount() != mesh_indices.size() || mesh_indices.size() != mesh->getVertexCount()) {
			mesh->setBuffer(0, mesh_vertices.data(), mesh_vertices.size());
			mesh->setVertexIndices(mesh_indices.data(), mesh_indices.size());
		}
	}

	if (mesh->getVertexCount() > 0) {
		DrawCmdInfo draw_cmd{};
		draw_cmd.mesh = mesh;
		draw_cmd.pipeline_instance = material;
        render_graph->draw(draw_cmd);
	}
}

void Navmesh::update() {
	vertices.clear();
	segments.clear();
	createOutline();
}

Navmesh::Navmesh(const bool *map, uint32_t size_x, uint32_t size_y, f32 cell_size) {
	this->map = map;
	this->size_x = size_x;
	this->size_y = size_y;
	this->cell_size = cell_size;

	update();

	VertexAttributeInfo binding_info{};
	binding_info.size = sizeof(vec2);
	binding_info.binding = 0;
	binding_info.flags = VERTEX_ATTRIBUTE_FLAG_FAST_WRITE | VERTEX_ATTRIBUTE_FLAG_MULTIPLE_BUFFERS;
	MeshInfo mesh_info{};
	mesh_info.attribute_info_count = 1;
	mesh_info.attribute_infos = &binding_info;
	mesh = Resources::createMesh(mesh_info);


	ShaderInfo vertex_shader_info{};
	vertex_shader_info.path = "/shaders/defaults/Line.vert";
	vertex_shader_info.stage = SHADER_STAGE_VERTEX;

	ShaderInfo fragment_shader_info{};
	fragment_shader_info.path = "/shaders/defaults/Line.frag";
	fragment_shader_info.stage = SHADER_STAGE_FRAGMENT;

	Shader *vertex_shader;
	Shader *fragment_shader;
	vertex_shader = Resources::createShader(vertex_shader_info);
	fragment_shader = Resources::createShader(fragment_shader_info);

	GraphicPipelineInfo graphic_pipeline_info{};
	graphic_pipeline_info.topology = VERTEX_TOPOLOGY_LINE;
	graphic_pipeline_info.attribute_infos = &binding_info;
	graphic_pipeline_info.attribute_info_count = 1;
	graphic_pipeline_info.flags = GRAPHIC_PIPELINE_FLAG_IGNORE_DEPTH_TEST;
	graphic_pipeline_info.vertex_shader = vertex_shader;
	graphic_pipeline_info.fragement_shader = fragment_shader;
	pipeline = Resources::createGraphicPipeline(graphic_pipeline_info);

	GraphicPipelineInstanceInfo material_info{};
	material_info.graphic_pipeline = pipeline;
	material = Resources::createGraphicPipelineInstance(material_info);

	vec4 color = vec4(0, 1, 0, 1);
	material->setUniform("material", &color, -1);
	delete vertex_shader;
	delete fragment_shader;
}

void Navmesh::createOutline() {

	uint8_t *squares_config = new uint8_t[size_x * size_y];
	size_t *square_vertices = new size_t[size_x * size_y * 4];
	for (size_t i = 0; i < size_x * size_y * 4; ++i) {
		square_vertices[i] = -1;
	}

	for (size_t y = 0; y < size_y - 1; ++y) {
		for (size_t x = 0; x < size_x - 1; ++x) {

			size_t i = (y * size_x) + x;
			bool bot_left = map[i];
			bool bot_right = map[i + 1];
			bool top_left = map[i + size_x];
			bool top_right = map[(i + size_x) + 1];
			squares_config[i] = static_cast<uint8_t>(((bot_left ? 1 : 0) << BOT_LEFT) | ((bot_right ? 1 : 0) << BOT_RIGHT) | ((top_right ? 1 : 0) << TOP_RIGHT) | ((top_left ? 1 : 0) << TOP_LEFT));

			size_t v1;
			size_t v2;
			size_t v3;
			size_t v4;
			f32 pos_x = f32(static_cast<int32_t>(x));
			f32 pos_y = f32(static_cast<int32_t>(y));

			size_t prev_vertex_index_x = (i - 1) * 4;
			size_t prev_vertex_index_y = (i - size_x) * 4;

			size_t vertex_index = (i * 4);
			switch (squares_config[i]) {
				case 1:
					if (prev_vertex_index_y >= 0 && square_vertices[prev_vertex_index_y + TOP] != -1)
						v1 = square_vertices[prev_vertex_index_y + TOP];
					else {
						v1 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>((pos_x + f32::HALF) * cell_size, (pos_y * cell_size))});
					}
					if (prev_vertex_index_x >= 0 && square_vertices[prev_vertex_index_x + RIGHT] != -1)
						v2 = square_vertices[prev_vertex_index_x + RIGHT];
					else {
						v2 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>((pos_x * cell_size), (pos_y + f32::HALF) * cell_size)});

					}

					square_vertices[vertex_index + BOT] = v1;
					square_vertices[vertex_index + LEFT] = v2;

					segments.emplace_back(Segment{v1, v2, fixed2<f32>(f32::HALF, f32::HALF)});
					break;
				case 2:
					if (prev_vertex_index_y >= 0 && square_vertices[prev_vertex_index_y + TOP] != -1)
						v1 = square_vertices[prev_vertex_index_y + TOP];
					else {
						v1 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>((pos_x + f32::HALF) * cell_size, (pos_y * cell_size))});
					}
					v2 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + 1) * cell_size), (pos_y + f32::HALF) * cell_size)});
					square_vertices[vertex_index + BOT] = v1;
					square_vertices[vertex_index + RIGHT] = v2;
					segments.emplace_back(Segment{v1, v2, fixed2<f32>(-f32::HALF, f32::HALF)});
					break;
				case 3:
					if (prev_vertex_index_x >= 0 && square_vertices[prev_vertex_index_x + RIGHT] != -1)
						v1 = square_vertices[prev_vertex_index_x + RIGHT];
					else {
						v1 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>((pos_x * cell_size), ((pos_y + f32::HALF) * cell_size))});
					}
					v2 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>((pos_x + 1) * cell_size, (pos_y + f32::HALF) * cell_size)});
					square_vertices[vertex_index + LEFT] = v1;
					square_vertices[vertex_index + RIGHT] = v2;

					segments.emplace_back(Segment{v1, v2, fixed2<f32>(0, f32::ONE)});
					break;
				case 4:
					v1 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>((pos_x + f32::HALF) * cell_size, (pos_y + 1) * cell_size)});
					v2 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + 1) * cell_size), (pos_y + f32::HALF) * cell_size)});

					square_vertices[vertex_index + TOP] = v1;
					square_vertices[vertex_index + RIGHT] = v2;

					segments.emplace_back(Segment{v1, v2, fixed2<f32>(-f32::HALF, -f32::HALF)});
					break;
				case 5:
					if (prev_vertex_index_x >= 0 && square_vertices[prev_vertex_index_x + RIGHT] != -1)
						v1 = square_vertices[prev_vertex_index_x + RIGHT];
					else {
						v1 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>((pos_x) * cell_size, ((pos_y) + f32::HALF) * cell_size)});
					}
					v2 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + f32::HALF) * cell_size), (pos_y + 1) * cell_size)});

					if (prev_vertex_index_y >= 0 && square_vertices[prev_vertex_index_y + TOP] != -1)
						v3 = square_vertices[prev_vertex_index_y + TOP];
					else {
						v3 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>((pos_x + f32::HALF) * cell_size, ((pos_y) * cell_size))});
					}
					v4 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + 1) * cell_size), (pos_y + f32::HALF) * cell_size)});

					square_vertices[vertex_index + LEFT] = v1;
					square_vertices[vertex_index + TOP] = v2;
					square_vertices[vertex_index + BOT] = v3;
					square_vertices[vertex_index + RIGHT] = v4;

					segments.emplace_back(Segment{v1, v2, fixed2<f32>(-f32::HALF, -f32::HALF)});
					segments.emplace_back(Segment{v3, v4, fixed2<f32>(-f32::HALF, -f32::HALF)});
					break;
				case 6:
					v1 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + f32::HALF) * cell_size), (pos_y + 1) * cell_size)});
					if (prev_vertex_index_y >= 0 && square_vertices[prev_vertex_index_y + TOP] != -1) v2 = square_vertices[prev_vertex_index_y + TOP];
					else {
						v2 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + f32::HALF) * cell_size), (pos_y) * cell_size)});
					}

					square_vertices[vertex_index + TOP] = v1;
					square_vertices[vertex_index + BOT] = v2;

					segments.emplace_back(Segment{v1, v2, fixed2<f32>(-f32::HALF, -f32::HALF)});
					break;
				case 7:
					if (prev_vertex_index_x >= 0 && square_vertices[prev_vertex_index_x + RIGHT] != -1) v1 = square_vertices[prev_vertex_index_x + RIGHT];
					else {
						v1 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>((pos_x) * cell_size, ((pos_y) + f32::HALF) * cell_size)});
					}

					v2 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + f32::HALF) * cell_size), (pos_y + 1) * cell_size)});
					square_vertices[vertex_index + LEFT] = v1;
					square_vertices[vertex_index + TOP] = v2;

					segments.emplace_back(Segment{v1, v2, fixed2<f32>(-f32::HALF, -f32::HALF)});
					break;
				case 8:
					if (prev_vertex_index_x >= 0 && square_vertices[prev_vertex_index_x + RIGHT] != -1) v1 = square_vertices[prev_vertex_index_x + RIGHT];
					else {
						v1 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>((pos_x) * cell_size, ((pos_y) + f32::HALF) * cell_size)});
					}
					v2 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + f32::HALF) * cell_size), (pos_y + 1) * cell_size)});

					square_vertices[vertex_index + LEFT] = v1;
					square_vertices[vertex_index + TOP] = v2;

					segments.emplace_back(Segment{v1, v2, fixed2<f32>(-f32::HALF, -f32::HALF)});
					break;
				case 9:
					v1 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + f32::HALF) * cell_size), (pos_y + 1) * cell_size)});
					if (prev_vertex_index_y >= 0 && square_vertices[prev_vertex_index_y + TOP] != -1) v2 = square_vertices[prev_vertex_index_y + TOP];
					else {
						v2 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + f32::HALF) * cell_size), (pos_y) * cell_size)});
					}

					square_vertices[vertex_index + TOP] = v1;
					square_vertices[vertex_index + BOT] = v2;

					segments.emplace_back(Segment{v1, v2, fixed2<f32>(-f32::HALF, -f32::HALF)});
					break;
				case 10:
					if (prev_vertex_index_x >= 0 && square_vertices[prev_vertex_index_x + RIGHT] != -1) v1 = square_vertices[prev_vertex_index_x + RIGHT];
					else {
						v1 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>((pos_x) * cell_size, ((pos_y) + f32::HALF) * cell_size)});
					}
					if (prev_vertex_index_y >= 0 && square_vertices[prev_vertex_index_y + TOP] != -1) v2 = square_vertices[prev_vertex_index_y + TOP];
					else {
						v2 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + f32::HALF) * cell_size), (pos_y) * cell_size)});
					}
					v3 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>((pos_x + f32::HALF) * cell_size, ((pos_y + 1) * cell_size))});
					v4 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + 1) * cell_size), (pos_y + f32::HALF) * cell_size)});


					square_vertices[vertex_index + LEFT] = v1;
					square_vertices[vertex_index + BOT] = v2;
					square_vertices[vertex_index + TOP] = v3;
					square_vertices[vertex_index + RIGHT] = v4;

					segments.emplace_back(Segment{v1, v2, fixed2<f32>(-f32::HALF, -f32::HALF)});
					segments.emplace_back(Segment{v3, v4, fixed2<f32>(-f32::HALF, -f32::HALF)});
					break;
				case 11:
					v1 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>((pos_x + f32::HALF) * cell_size, ((pos_y + 1) * cell_size))});
					v2 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + 1) * cell_size), (pos_y + f32::HALF) * cell_size)});


					square_vertices[vertex_index + TOP] = v1;
					square_vertices[vertex_index + RIGHT] = v2;

					segments.emplace_back(Segment{v1, v2, fixed2<f32>(-f32::HALF, -f32::HALF)});
					break;
				case 12:
					if (prev_vertex_index_x >= 0 && square_vertices[prev_vertex_index_x + RIGHT] != -1) v1 = square_vertices[prev_vertex_index_x + RIGHT];
					else {
						v1 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>((pos_x) * cell_size, ((pos_y) + f32::HALF) * cell_size)});
					}
					v2 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + 1) * cell_size), (pos_y + f32::HALF) * cell_size)});

					square_vertices[vertex_index + LEFT] = v1;
					square_vertices[vertex_index + RIGHT] = v2;

					segments.emplace_back(Segment{v1, v2, fixed2<f32>(-f32::HALF, -f32::HALF)});
					break;
				case 13:
					if (prev_vertex_index_y >= 0 && square_vertices[prev_vertex_index_y + TOP] != -1) v1 = square_vertices[prev_vertex_index_y + TOP];
					else {
						v1 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>((pos_x + f32::HALF) * cell_size, ((pos_y) * cell_size))});
					}
					v2 = vertices.size();
					vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + 1) * cell_size), (pos_y + f32::HALF) * cell_size)});
					square_vertices[vertex_index + BOT] = v1;
					square_vertices[vertex_index + RIGHT] = v2;

					segments.emplace_back(Segment{v1, v2, fixed2<f32>(-f32::HALF, -f32::HALF)});
					break;
				case 14:
					if (prev_vertex_index_x >= 0 && square_vertices[prev_vertex_index_x + RIGHT] != -1) v1 = square_vertices[prev_vertex_index_x + RIGHT];
					else {
						v1 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>((pos_x) * cell_size, ((pos_y) + f32::HALF) * cell_size)});
					}
					if (prev_vertex_index_y >= 0 && square_vertices[prev_vertex_index_y + TOP] != -1) v2 = square_vertices[prev_vertex_index_y + TOP];
					else {
						v2 = vertices.size();
						vertices.emplace_back(Vertex{fixed2<f32>(((pos_x + f32::HALF) * cell_size), (pos_y) * cell_size)});
					}

					square_vertices[vertex_index + LEFT] = v1;
					square_vertices[vertex_index + BOT] = v2;

					segments.emplace_back(Segment{v1, v2, fixed2<f32>(-f32::HALF, -f32::HALF)});
					break;
				default:
					break;

			}
		}
	}
	delete[] squares_config;
	delete[] square_vertices;
}

Navmesh::~Navmesh() {
	delete mesh;
	delete material;
	delete pipeline;
}

std::vector<Navmesh::Triangle> Navmesh::triangulate() {
	return std::vector<Triangle>();
}

fixed2<f32> Navmesh::getCircumcenter(Triangle triangle) {

	size_t v1 = triangle.v1, v2 = triangle.v2, v3 = triangle.v3;
	fixed2<f32> middle_1 = (vertices[v1].position + vertices[v2].position) / f32(2);
	f32 delta_y1 = vertices[v2].position.y - vertices[v1].position.y;
	f32 delta_x1 = vertices[v2].position.x - vertices[v1].position.x;
	f32 slope_1 = delta_y1 / delta_x1;
	f32 perpendicular_slope_1 = f32(-1) / slope_1;

	//y = perpendicular_slope_1*x - perpendicular_slope_1*middle_1.x - middle_1.y

	fixed2<f32> middle_2 = (vertices[v1].position + vertices[v3].position) / f32(2);
	f32 delta_y2 = vertices[v3].position.y - vertices[v1].position.y;
	f32 delta_x2 = vertices[v3].position.x - vertices[v1].position.x;
	f32 slope_2 = delta_y2 / delta_x2;
	f32 perpendicular_slope_2 = f32(-1) / slope_2;

	f32 x = (-perpendicular_slope_2 * middle_2.x + perpendicular_slope_1 * middle_1.x - middle_2.y + middle_1.y) / (perpendicular_slope_1 - perpendicular_slope_2);
	f32 y = perpendicular_slope_1 * x - perpendicular_slope_1 * middle_1.x - middle_1.y;

	return fixed2<f32>(x, y);

}




