#include "Geometry.h"
#include <algorithm>

namespace HBE {

	void Geometry::createQuad(Mesh &mesh, float size_x, float size_y, VERTEX_FLAGS flags, PIVOT pivot) {
		vec2 pivot_offset = vec2(0, 0);
		switch (pivot) {
			case PIVOT_CENTER:
				pivot_offset = vec2(0, 0);
				break;
			case PIVOT_TOP_LEFT:
				pivot_offset = vec2(-size_x / 2, size_y / 2);
				break;
			case PIVOT_TOP_RIGHT:
				pivot_offset = vec2(size_x / 2, size_y / 2);
				break;
			case PIVOT_BOTTOM_LEFT:
				pivot_offset = vec2(-size_x / 2, -size_y / 2);
				break;
			case PIVOT_BOTTOM_RIGHT:
				pivot_offset = vec2(size_x / 2, -size_y / 2);
				break;
			case PIVOT_CENTER_LEFT:
				pivot_offset = vec2(-size_x / 2, 0);
				break;
			case PIVOT_CENTER_RIGHT:
				pivot_offset = vec2(size_x / 2, 0);
				break;
			case PIVOT_TOP_CENTER:
				pivot_offset = vec2(0, size_y / 2);
				break;
			case PIVOT_BOTTOM_CENTER:
				pivot_offset = vec2(0, -size_y / 2);
				break;
		}
		switch (flags) {
			case 0://VERTEX_FLAG_NONE
			{
				std::vector<float> vertex_positions = {
						(0.5f * size_x) + pivot_offset.x, (-0.5f * size_y) + pivot_offset.y, 0,
						(0.5f * size_x) + pivot_offset.x, (0.5f * size_y) + pivot_offset.y, 0,
						(-0.5f * size_x) + pivot_offset.x, (0.5f * size_y) + pivot_offset.y, 0,
						(-0.5f * size_x) + pivot_offset.x, (-0.5f * size_y + pivot_offset.y), 0,
				};
				mesh.setBuffer(0, vertex_positions.data(), 4);
				break;
			}
			case 1://VERTEX_FLAG_UV
			{
				std::vector<float> vertex_positions = {
						(0.5f * size_x) + pivot_offset.x, (-0.5f * size_y) + pivot_offset.y, 0, 1.0f, 0.0f,
						(0.5f * size_x) + pivot_offset.x, (0.5f * size_y) + pivot_offset.y, 0, 1.0f, 1.0f,
						(-0.5f * size_x) + pivot_offset.x, (0.5f * size_y) + pivot_offset.y, 0, 0.0f, 1.0f,
						(-0.5f * size_x) + pivot_offset.x, (-0.5f * size_y + pivot_offset.y), 0, .0f, 0.0f,
				};
				mesh.setBuffer(0, vertex_positions.data(), 4);
				break;
			}
			case 2://VERTEX_FLAG_NORMAL
			{
				std::vector<float> vertex_positions = {
						(0.5f * size_x) + pivot_offset.x, (-0.5f * size_y) + pivot_offset.y, 0, 0, -1.0f,
						(0.5f * size_x) + pivot_offset.x, (0.5f * size_y) + pivot_offset.y, 0, 0, -1.0f,
						(-0.5f * size_x) + pivot_offset.x, (0.5f * size_y) + pivot_offset.y, 0, 1.0f, 0, 0, -1.0f,
						(-0.5f * size_x) + pivot_offset.x, (-0.5f * size_y + pivot_offset.y), 0, 0.0f, 0, 0, -1.0
				};
				mesh.setBuffer(0, vertex_positions.data(), 4);
				break;
			}
			case 3://VERTEX_FLAG_NORMAL&VERTEX_FLAG_UV
			{
				std::vector<float> vertex_positions = {
						(0.5f * size_x) + pivot_offset.x, (-0.5f * size_y) + pivot_offset.y, 0, -1.0f,
						(0.5f * size_x) + pivot_offset.x, (0.5f * size_y) + pivot_offset.y, 0, -1.0f,
						(-0.5f * size_x) + pivot_offset.x, (0.5f * size_y) + pivot_offset.y, 0, -1.0f,
						(-0.5f * size_x) + pivot_offset.x, (-0.5f * size_y + pivot_offset.y), 0, -1.0
				};
				mesh.setBuffer(0, vertex_positions.data(), 4);
				break;
			}
		}
		//1---2
		//| / |
		//0---3
		std::vector<unsigned int> indices = {
				0, 3, 2,
				2, 1, 0,
		};
		mesh.setVertexIndices(indices.data(), indices.size());
	}

	void Geometry::createCube(Mesh &mesh, float size_x, float size_y, float size_z, VERTEX_FLAGS flags) {
		//Back   Front
		//1---2  2---1
		//| / |  | / |
		//0---3  3---0
		vec3 size = vec3(0.5f * size_x, 0.5f * size_y, 0.5f * size_z);
		std::vector<float> vertices;
		switch (flags) {
			case 0://VERTEX_FLAG_NONE
				vertices.assign({
										size.x, -size.y, size.z,
										size.x, size.y, size.z,
										-size.x, size.y, size.z,
										-size.x, -size.y, size.z,

										size.x, -size.y, -size.z,
										size.x, size.y, -size.z,
										size.x, size.y, size.z,
										size.x, -size.y, size.z,

										-size.x, -size.y, -size.z,
										-size.x, size.y, -size.z,
										size.x, size.y, -size.z,
										size.x, -size.y, -size.z,

										-size.x, -size.y, size.z,
										-size.x, size.y, size.z,
										-size.x, size.y, -size.z,
										-size.x, -size.y, -size.z,

										-size.x, size.y, -size.z,
										-size.x, size.y, size.z,
										size.x, size.y, size.z,
										size.x, size.y, -size.z,

										size.x, -size.y, size.z,
										-size.x, -size.y, size.z,
										-size.x, -size.y, -size.z,
										size.x, -size.y, -size.z,
								});

				mesh.setBuffer(0, vertices.data(), vertices.size() / 3);
				break;

			case 1://VERTEX_FLAG_UV
				vertices.assign({
										size.x, -size.y, size.z, 0.0f, 0.0f,
										size.x, size.y, size.z, 0.0f, 1.0f,
										-size.x, size.y, size.z, 1.0f, 1.0f,
										-size.x, -size.y, size.z, 1.0f, 0.0f,

										size.x, -size.y, -size.z, 0.0f, 0.0f,
										size.x, size.y, -size.z, 0.0f, 1.0f,
										size.x, size.y, size.z, 1.0f, 1.0f,
										size.x, -size.y, size.z, 1.0f, 0.0f,

										-size.x, -size.y, -size.z, 0.0f, 0.0f,
										-size.x, size.y, -size.z, 0.0f, 1.0f,
										size.x, size.y, -size.z, 1.0f, 1.0f,
										size.x, -size.y, -size.z, 1.0f, 0.0f,

										-size.x, -size.y, size.z, 0.0f, 0.0f,
										-size.x, size.y, size.z, 0.0f, 1.0f,
										-size.x, size.y, -size.z, 1.0f, 1.0f,
										-size.x, -size.y, -size.z, 1.0f, 0.0f,

										-size.x, size.y, -size.z, 0.0f, 0.0f,
										-size.x, size.y, size.z, 0.0f, 1.0f,
										size.x, size.y, size.z, 1.0f, 1.0f,
										size.x, size.y, -size.z, 1.0f, 0.0f,

										size.x, -size.y, size.z, 0.0f, 0.0f,
										-size.x, -size.y, size.z, 0.0f, 1.0f,
										-size.x, -size.y, -size.z, 1.0f, 1.0f,
										size.x, -size.y, -size.z, 1.0f, 0.0f,

								});
				mesh.setBuffer(0, vertices.data(), vertices.size() / 5);
				break;

			case 2://VERTEX_FLAG_NORMAL
				vertices.assign({
										size.x, -size.y, size.z, 0.5, 0.5, 0.5,
										size.x, size.y, size.z, 0.5, 0.5, 0.5,
										-size.x, size.y, size.z, 0.5, 0.5, 0.5,
										-size.x, -size.y, size.z, 0.5, 0.5, 0.5,
										size.x, -size.y, -size.z, 0.5, 0.5, 0.5,
										size.x, size.y, -size.z, 0.5, 0.5, 0.5,
										size.x, size.y, size.z, 0.5, 0.5, 0.5,
										size.x, -size.y, size.z, 0.5, 0.5, 0.5,
										-size.x, -size.y, -size.z, 0.5, 0.5, 0.5,
										-size.x, size.y, -size.z, 0.5, 0.5, 0.5,
										size.x, size.y, -size.z, 0.5, 0.5, 0.5,
										size.x, -size.y, -size.z, 0.5, 0.5, 0.5,
										-size.x, -size.y, size.z, 0.5, 0.5, 0.5,
										-size.x, size.y, size.z, 0.5, 0.5, 0.5,
										-size.x, size.y, -size.z, 0.5, 0.5, 0.5,
										-size.x, -size.y, -size.z, 0.5, 0.5, 0.5,
										-size.x, size.y, -size.z, 0.5, 0.5, 0.5,
										-size.x, size.y, size.z, 0.5, 0.5, 0.5,
										size.x, size.y, size.z, 0.5, 0.5, 0.5,
										size.x, size.y, -size.z, 0.5, 0.5, 0.5,
										size.x, -size.y, size.z, 0.5, 0.5, 0.5,
										-size.x, -size.y, size.z, 0.5, 0.5, 0.5,
										-size.x, -size.y, -size.z, 0.5, 0.5, 0.5,
										size.x, -size.y, -size.z, 0.5, 0.5, 0.5,

								});
				mesh.setBuffer(0, vertices.data(), vertices.size() / 6);
				break;
			case 3://VERTEX_FLAG_NORMAL&VERTEX_FLAG_UV
				vertices.assign({
										size.x, -size.y, size.z, 0.0f, 0.0f, 0.5, 0.5, 0.5,
										size.x, size.y, size.z, 0.0f, 1.0f, 0.5, 0.5, 0.5,
										-size.x, size.y, size.z, 1.0f, 1.0f, 0.5, 0.5, 0.5,
										-size.x, -size.y, size.z, 1.0f, 0.0f, 0.5, 0.5, 0.5,
										size.x, -size.y, -size.z, 0.0f, 0.0f, 0.5, 0.5, 0.5,
										size.x, size.y, -size.z, 0.0f, 1.0f, 0.5, 0.5, 0.5,
										size.x, size.y, size.z, 1.0f, 1.0f, 0.5, 0.5, 0.5,
										size.x, -size.y, size.z, 1.0f, 0.0f, 0.5, 0.5, 0.5,
										-size.x, -size.y, -size.z, 0.0f, 0.0f, 0.5, 0.5, 0.5,
										-size.x, size.y, -size.z, 0.0f, 1.0f, 0.5, 0.5, 0.5,
										size.x, size.y, -size.z, 1.0f, 1.0f, 0.5, 0.5, 0.5,
										size.x, -size.y, -size.z, 1.0f, 0.0f, 0.5, 0.5, 0.5,
										-size.x, -size.y, size.z, 0.0f, 0.0f, 0.5, 0.5, 0.5,
										-size.x, size.y, size.z, 0.0f, 1.0f, 0.5, 0.5, 0.5,
										-size.x, size.y, -size.z, 1.0f, 1.0f, 0.5, 0.5, 0.5,
										-size.x, -size.y, -size.z, 1.0f, 0.0f, 0.5, 0.5, 0.5,
										-size.x, size.y, -size.z, 0.0f, 0.0f, 0.5, 0.5, 0.5,
										-size.x, size.y, size.z, 0.0f, 1.0f, 0.5, 0.5, 0.5,
										size.x, size.y, size.z, 1.0f, 1.0f, 0.5, 0.5, 0.5,
										size.x, size.y, -size.z, 1.0f, 0.0f, 0.5, 0.5, 0.5,
										size.x, -size.y, size.z, 0.0f, 0.0f, 0.5, 0.5, 0.5,
										-size.x, -size.y, size.z, 0.0f, 1.0f, 0.5, 0.5, 0.5,
										-size.x, -size.y, -size.z, 1.0f, 1.0f, 0.5, 0.5, 0.5,
										size.x, -size.y, -size.z, 1.0f, 0.0f, 0.5, 0.5, 0.5,

								});
				mesh.setBuffer(0, vertices.data(), vertices.size() / 8);
				break;
		}

		std::vector<uint32_t> indices = {
				0, 1, 2,
				2, 3, 0,
				0 + 4, 1 + 4, 2 + 4,
				2 + 4, 3 + 4, 0 + 4,
				0 + 8, 1 + 8, 2 + 8,
				2 + 8, 3 + 8, 0 + 8,
				0 + 12, 1 + 12, 2 + 12,
				2 + 12, 3 + 12, 0 + 12,
				0 + 16, 1 + 16, 2 + 16,
				2 + 16, 3 + 16, 0 + 16,
				0 + 20, 1 + 20, 2 + 20,
				2 + 20, 3 + 20, 0 + 20,
		};
		mesh.setVertexIndices(indices.data(), indices.size());
	}

}