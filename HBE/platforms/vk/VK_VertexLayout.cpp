#include "VK_VertexLayout.h"
#include "vulkan/vulkan.h"

namespace HBE {

	VK_VertexLayout::VK_VertexLayout(const VertexLayoutInfo &info) {
		this->layout.assign(info.layout_types,info.layout_types+info.layout_types_count);
		binding_description.binding = 0;
		binding_description.stride = getBytesPerVertex();
		binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		int offset = 0;
		int location = 0;
		attribute_descriptions.resize(layout.size());
		for (size_t i = 0; i < layout.size(); ++i) {
			attribute_descriptions[i].binding = 0;
			attribute_descriptions[i].location = location;
			attribute_descriptions[i].offset = offset;
			switch (layout[i]) {
				case GLSL_TYPE::FLOAT:
					attribute_descriptions[i].format = VK_FORMAT_R32_SFLOAT;
					offset += 4;
					location += 1;
					break;
				case GLSL_TYPE::VEC2F:
					attribute_descriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
					offset += 8;
					location += 1;
					break;
				case GLSL_TYPE::VEC3F:
					attribute_descriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
					offset += 12;
					location += 1;
					break;
				case GLSL_TYPE::VEC4F:
					attribute_descriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
					offset += 16;
					location += 1;
					break;
				case GLSL_TYPE::MAT3F:
					attribute_descriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
					offset += 36;
					location += 3;
					break;
				case GLSL_TYPE::MAT4F:
					attribute_descriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
					offset += 64;
					location += 4;
					break;
				case GLSL_TYPE::INT:
					attribute_descriptions[i].format = VK_FORMAT_R32_SINT;
					location += 1;
					offset += 4;
					break;
				case GLSL_TYPE::VEC2I:
					attribute_descriptions[i].format = VK_FORMAT_R32G32_SINT;
					location += 1;
					offset += 8;
					break;
				case GLSL_TYPE::VEC3I:
					attribute_descriptions[i].format = VK_FORMAT_R32G32B32_SINT;
					location += 1;
					offset += 12;
					break;
				case GLSL_TYPE::VEC4I:
					attribute_descriptions[i].format = VK_FORMAT_R32_SINT;
					location += 16;
					offset += 16;
					break;
				case GLSL_TYPE::MAT3I:
					attribute_descriptions[i].format = VK_FORMAT_R32_SINT;
					location += 3;
					offset += 36;
					break;
				case GLSL_TYPE::MAT4I:
					attribute_descriptions[i].format = VK_FORMAT_R32G32B32A32_SINT;
					location += 4;
					offset += 64;
					break;
				case GLSL_TYPE::UINT:
					attribute_descriptions[i].format = VK_FORMAT_R32_UINT;
					location += 1;
					offset += 4;
					break;
				case GLSL_TYPE::VEC2UI:
					attribute_descriptions[i].format = VK_FORMAT_R32G32_UINT;
					location += 1;
					offset += 8;
					break;
				case GLSL_TYPE::VEC3UI:
					attribute_descriptions[i].format = VK_FORMAT_R32G32B32_UINT;
					location += 1;
					offset += 12;
					break;
				case GLSL_TYPE::VEC4UI:
					attribute_descriptions[i].format = VK_FORMAT_R32G32B32A32_UINT;
					location += 1;
					offset += 16;
					break;
				case GLSL_TYPE::MAT3UI:
					attribute_descriptions[i].format = VK_FORMAT_R32G32B32_UINT;
					location += 3;
					offset += 36;
					break;
				case GLSL_TYPE::MAT4UI:
					attribute_descriptions[i].format = VK_FORMAT_R32G32B32A32_UINT;
					location += 4;
					offset += 64;
					break;
			}
		}
	}

	const VkVertexInputBindingDescription &VK_VertexLayout::getBindingDescription() const {
		return binding_description;
	}

	const std::vector<VkVertexInputAttributeDescription> &VK_VertexLayout::getAttributeDescriptions() const {
		return attribute_descriptions;
	}

}


