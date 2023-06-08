#include "Geometry.h"
#include "core/resource/Font.h"
#include <algorithm>
#include "core/resource/Resources.h"

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


	void Geometry::updateText(Mesh &mesh, const std::string &text, Font &font, float line_height, float space_width, TEXT_ALIGNMENT alignment, PIVOT pivot, float &total_width, float &total_height) {
		uint32_t vertex_size = 3 + 2;
		uint32_t quad_size = 4 * vertex_size;
		int text_length = text.size();
		std::vector<float> vertex_buffer;
		vertex_buffer.reserve(quad_size * text_length);
		std::vector<uint32_t> index_buffer;
		index_buffer.reserve(6 * text_length);
		std::vector<float> line_widths;
		std::vector<uint32_t> line_character_counts;
		line_character_counts.push_back(0);
		int line = 1;
		float horizontal_offset = 0;
		total_width = 0;
		total_height = 0;
		int vertex_count = 0;
		for (int i = 0; i < text_length; ++i) {
			if (text[i] == '\n') {
				line++;
				line_widths.push_back(horizontal_offset);
				line_character_counts.push_back(0);
				total_width = std::max(total_width, horizontal_offset);
				horizontal_offset = 0;
				continue;
			} else if (text[i] == ' ') {
				horizontal_offset += space_width;
				continue;
			}
			line_character_counts.back()++;
			Glyph glyph = font.getCharacterGlyph(text[i]);

			size_t vertices_index = vertex_buffer.size();
			vertex_buffer.resize(vertex_buffer.size() + quad_size);

			//1---2
			//| / |
			//0---3

			//v0 bottom left
			//position
			vertex_buffer[vertices_index] = horizontal_offset;
			vertex_buffer[vertices_index + 1] = (line * -line_height) - glyph.offset.y;
			vertex_buffer[vertices_index + 2] = 0;

			//uv
			vertex_buffer[vertices_index + 3] = glyph.uv_min.x;
			vertex_buffer[vertices_index + 4] = glyph.uv_min.y;

			//v1 top left
			//position
			vertex_buffer[vertices_index + 5] = horizontal_offset;
			vertex_buffer[vertices_index + 6] = (line * -line_height) + glyph.size.y - glyph.offset.y;
			vertex_buffer[vertices_index + 7] = 0;

			//uv
			vertex_buffer[vertices_index + 8] = glyph.uv_min.x;
			vertex_buffer[vertices_index + 9] = glyph.uv_max.y;

			//v2 top right
			//position
			vertex_buffer[vertices_index + 10] = horizontal_offset + glyph.size.x;
			vertex_buffer[vertices_index + 11] = (line * -line_height) + glyph.size.y - glyph.offset.y;
			vertex_buffer[vertices_index + 12] = 0;
			//uv
			vertex_buffer[vertices_index + 13] = glyph.uv_max.x;
			vertex_buffer[vertices_index + 14] = glyph.uv_max.y;

			//v3 bottom right
			//position
			vertex_buffer[vertices_index + 15] = horizontal_offset + glyph.size.x;
			vertex_buffer[vertices_index + 16] = (line * -line_height) - glyph.offset.y;
			vertex_buffer[vertices_index + 17] = 0;
			//uv
			vertex_buffer[vertices_index + 18] = glyph.uv_max.x;
			vertex_buffer[vertices_index + 19] = glyph.uv_min.y;

			size_t indices_index = index_buffer.size();
			index_buffer.resize(index_buffer.size() + 6);
			index_buffer[indices_index] = vertex_count;
			index_buffer[indices_index + 1] = vertex_count + 1;
			index_buffer[indices_index + 2] = vertex_count + 2;
			index_buffer[indices_index + 3] = vertex_count + 2;
			index_buffer[indices_index + 4] = vertex_count + 3;
			index_buffer[indices_index + 5] = vertex_count;
			horizontal_offset += glyph.size.x;
			vertex_count += 4;
		}
		line_widths.push_back(horizontal_offset);
		line_character_counts.push_back(0);
		total_width = std::max(total_width, horizontal_offset);
		total_height = static_cast<float>(line) * line_height;


		int current_line = 0;
		int vertex_index = 0;
		for (int i = 0; i < vertex_buffer.size() / 5; ++i) {
			if (line_character_counts[current_line] * 4 == vertex_index) {
				current_line++;
				vertex_index = 0;
			}

			switch (alignment) {
				case TEXT_ALIGNMENT_LEFT:

					break;
				case TEXT_ALIGNMENT_RIGHT:
					vertex_buffer[i * 5] += total_width - line_widths[current_line];
					break;
				case TEXT_ALIGNMENT_CENTER:
					vertex_buffer[i * 5] += (total_width - line_widths[current_line]) * 0.5f;
					break;
			}
			switch (pivot) {
				case PIVOT_TOP_LEFT:
					break;
				case PIVOT_TOP_CENTER:
					vertex_buffer[i * 5] -= total_width * 0.5f;
					break;
				case PIVOT_TOP_RIGHT:
					vertex_buffer[i * 5] -= total_width;
					break;
				case PIVOT_CENTER_LEFT:
					vertex_buffer[i * 5 + 1] += total_height * 0.5f;
					break;
				case PIVOT_CENTER:
					vertex_buffer[i * 5] -= total_width * 0.5f;
					vertex_buffer[i * 5 + 1] += total_height * 0.5f;
					break;
				case PIVOT_CENTER_RIGHT:
					vertex_buffer[i * 5] -= total_width;
					vertex_buffer[i * 5 + 1] += total_height * 0.5f;
					break;
				case PIVOT_BOTTOM_LEFT:
					vertex_buffer[i * 5 + 1] += total_height;
					break;
				case PIVOT_BOTTOM_CENTER:
					vertex_buffer[i * 5] -= total_width * 0.5f;
					vertex_buffer[i * 5 + 1] -= total_height;
					break;
				case PIVOT_BOTTOM_RIGHT:
					vertex_buffer[i * 5] -= total_width;
					vertex_buffer[i * 5 + 1] += total_height;
					break;
			}
			vertex_index++;
		}


		if (vertex_buffer.size() > 0) {
			mesh.setBuffer(0, vertex_buffer.data(), vertex_count);
			mesh.setVertexIndices(index_buffer.data(), index_buffer.size());
		}
	}

	Mesh *Geometry::createText(const std::string &text,
							   Font &font,
							   float line_height,
							   float space_width,
							   TEXT_ALIGNMENT alignment,
							   PIVOT pivot,
							   float &total_width,
							   float &total_height) {
		VertexAttributeInfo vertex_attribute_info{};
		vertex_attribute_info.size = sizeof(float) * 5;
		vertex_attribute_info.binding = 0;
		vertex_attribute_info.flags = 0;

		MeshInfo mesh_info{};
		mesh_info.flags = MESH_FLAG_NONE;
		mesh_info.attribute_infos = &vertex_attribute_info;
		mesh_info.attribute_info_count = 1;

		Mesh *mesh = Resources::createMesh(mesh_info);
		updateText(*mesh, text, font, line_height, space_width, alignment, pivot, total_width, total_height);
		return mesh;
	}

}