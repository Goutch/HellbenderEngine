//
// Created by User on 03-Jun.-2020.
//

#include "Geometry.h"
#include <algorithm>

namespace HBE {
	void Geometry::createQuad(Mesh &mesh, float size_x, float size_y, VERTEX_FLAGS flags) {
		if (true) {
			std::vector<vec3> vertex_positions = {
					vec3(0.5f * size_x, -0.5f * size_y, .0f),
					vec3(0.5f * size_x, 0.5f * size_y, .0f),
					vec3(-0.5f * size_x, 0.5f * size_y, .0f),
					vec3(-0.5f * size_x, -0.5f * size_y, .0f)
			};
			mesh.setBuffer(0, vertex_positions);
		}
		if (true) {
			std::vector<vec2> vertex_uvs = {
					vec2(1.0f, 0.0f),
					vec2(1.0f, 1.0f),
					vec2(0.0f, 1.0f),
					vec2(0.0f, 0.0f),
			};
			mesh.setBuffer(1, vertex_uvs);
		}
		if (true) {
			std::vector<vec3> vertex_normals = {
					vec3(0, 0, -1.0f),
					vec3(0, 0, -1.0f),
					vec3(0, 0, -1.0f),
					vec3(0, 0, -1.0f),
			};
			mesh.setBuffer(2, vertex_normals);
		}
		std::vector<unsigned int> indices = {
				0, 1, 2,
				2, 3, 0,
		};
		mesh.setIndices(indices);
	}

	void Geometry::createCube(Mesh &mesh, float size_x, float size_y, float size_z, VERTEX_FLAGS flags) {
		//Back   Front
		//1---2  2---1
		//| / |  | / |
		//0---3  3---0
		vec3 size = vec3(0.5f * size_x, 0.5f * size_y, 0.5f * size_z);
		std::vector<float> vertices;
		switch (flags) {
			case 0x0://VERTEX_FLAG_NONE
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

				mesh.setVertices(0, vertices.data(), vertices.size() / 3);
				break;

			case 0x1://VERTEX_FLAG_UV
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
				mesh.setVertices(0, vertices.data(), vertices.size() / 5);
				break;

			case 0x10://VERTEX_FLAG_NORMAL
				break;
			case 0x11://VERTEX_FLAG_NORMAL&VERTEX_FLAG_UV
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
		mesh.setIndices(indices);
	}

}