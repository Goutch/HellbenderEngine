#include "VK_VertexLayout.h"
#include "vulkan/vulkan.h"

namespace HBE {

	/*VK_VertexLayout::VK_VertexLayout(const MeshLayoutInfo &info) {
		this->vertex_layout.assign(info.vertex_layout_types, info.vertex_layout_types + info.vertex_layout_types_count);
		this->instanced_layout.assign(info.instance_layout_types, info.instance_layout_types + info.instance_layout_types_count);
		vertex_binding_description.binding = info.vertex_binding;
		vertex_binding_description.stride = getBytesPerVertex();
		vertex_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


		int offset = 0;
		int location = 0;
		vertex_attribute_descriptions.resize(vertex_layout.size());
		instance_attribute_descriptions.resize(instanced_layout.size());
		for (size_t i = 0; i < vertex_layout.size(); ++i) {
			vertex_attribute_descriptions[i].binding = info.vertex_binding;
			vertex_attribute_descriptions[i].location = location;
			vertex_attribute_descriptions[i].offset = offset;
			switch (vertex_layout[i]) {
				case GLSL_TYPE::FLOAT:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32_SFLOAT;
					offset += 4;
					location += 1;
					break;
				case GLSL_TYPE::VEC2F:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
					offset += 8;
					location += 1;
					break;
				case GLSL_TYPE::VEC3F:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
					offset += 12;
					location += 1;
					break;
				case GLSL_TYPE::VEC4F:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
					offset += 16;
					location += 1;
					break;
				case GLSL_TYPE::MAT3F:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
					offset += 36;
					location += 3;
					break;
				case GLSL_TYPE::MAT4F:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
					offset += 64;
					location += 4;
					break;
				case GLSL_TYPE::INT:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32_SINT;
					location += 1;
					offset += 4;
					break;
				case GLSL_TYPE::VEC2I:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32G32_SINT;
					location += 1;
					offset += 8;
					break;
				case GLSL_TYPE::VEC3I:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32G32B32_SINT;
					location += 1;
					offset += 12;
					break;
				case GLSL_TYPE::VEC4I:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32_SINT;
					location += 16;
					offset += 16;
					break;
				case GLSL_TYPE::MAT3I:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32_SINT;
					location += 3;
					offset += 36;
					break;
				case GLSL_TYPE::MAT4I:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32G32B32A32_SINT;
					location += 4;
					offset += 64;
					break;
				case GLSL_TYPE::UINT:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32_UINT;
					location += 1;
					offset += 4;
					break;
				case GLSL_TYPE::VEC2UI:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32G32_UINT;
					location += 1;
					offset += 8;
					break;
				case GLSL_TYPE::VEC3UI:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32G32B32_UINT;
					location += 1;
					offset += 12;
					break;
				case GLSL_TYPE::VEC4UI:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32G32B32A32_UINT;
					location += 1;
					offset += 16;
					break;
				case GLSL_TYPE::MAT3UI:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32G32B32_UINT;
					location += 3;
					offset += 36;
					break;
				case GLSL_TYPE::MAT4UI:
					vertex_attribute_descriptions[i].format = VK_FORMAT_R32G32B32A32_UINT;
					location += 4;
					offset += 64;
					break;
			}
		}
	}

	const VkVertexInputBindingDescription &VK_VertexLayout::getBindingDescription() const {
		return vertex_binding_description;
	}

	const std::vector<VkVertexInputAttributeDescription> &VK_VertexLayout::getAttributeDescriptions() const {
		return vertex_attribute_descriptions;
	}
*/
}


