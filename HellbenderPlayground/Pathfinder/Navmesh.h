#pragma once

#include "HBE.h"
//https://www.mathsisfun.com/geometry/triangle-centers.html
using namespace HBE;

class Navmesh {
	Mesh *mesh;
	GraphicPipeline *pipeline;
	GraphicPipelineInstance *material;
	const bool *map;
	uint32_t size_x;
	uint32_t size_y;
	f32 cell_size;
public:

	enum CORNER {
		BOT_LEFT = 0,
		BOT_RIGHT = 1,
		TOP_RIGHT = 2,
		TOP_LEFT = 3,
	};
	enum SIDE {
		LEFT = 0,
		BOT = 1,
		RIGHT = 2,
		TOP = 3,
	};
	struct Vertex {
		fixed2<f32> position;
	};
	struct Segment {
		size_t v1;
		size_t v2;
		fixed2<f32> normal;
	};
	struct Square {
		uint8_t config;
	};

	struct Triangle {
		size_t v1, v2, v3;

		Triangle(size_t v1, size_t v2, size_t v3) {

		}

		fixed2<f32> circumcenter;
		f32 radius;
	};

	std::vector<Vertex> vertices;
	std::vector<Segment> segments;
	std::vector<Triangle> triangles;

public:
	Navmesh(const bool *map, uint32_t size_x, uint32_t size_y, f32 cell_size);
	~Navmesh();
	void update();
	void draw(RenderGraph* render_graph);
private:
	void createOutline();
	std::vector<Triangle> triangulate();


	fixed2<f32> getCircumcenter(Triangle triangle);
};
