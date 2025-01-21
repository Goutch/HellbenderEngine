#pragma once

#include "core/utility/Log.h"

namespace HBE {
	class VK_Utils {
	public:
		static std::string samplerModeToString(IMAGE_SAMPLER_ADDRESS_MODE mode) {
			switch (mode) {
				case IMAGE_SAMPLER_ADDRESS_MODE_REPEAT:
					return "REPEAT";
				case IMAGE_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:
					return "MIRRORED_REPEAT";
				case IMAGE_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:
					return "CLAMP_TO_EDGE";
				case IMAGE_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:
					return "CLAMP_TO_BORDER";
				case IMAGE_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE:
					return "MIRROR_CLAMP_TO_EDGE";
				default:
					return "UNKNOWN";
			}
		}

		static std::string samplerFilterTypeToString(IMAGE_SAMPLER_FILTER_TYPE filter) {
			switch (filter) {
				case IMAGE_SAMPLER_FILTER_TYPE_NEAREST:
					return "NEAREST";
				case IMAGE_SAMPLER_FILTER_TYPE_LINEAR:
					return "LINEAR";
				default:
					return "UNKNOWN";
			}
		}

		static std::string layoutToString(VkImageLayout layout) {
			switch (layout) {
				case VK_IMAGE_LAYOUT_UNDEFINED:
					return "UNDEFINED";
				case VK_IMAGE_LAYOUT_GENERAL:
					return "GENERAL";
				case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
					return "COLOR_ATTACHMENT_OPTIMAL";
				case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
					return "DEPTH_STENCIL_ATTACHMENT_OPTIMAL";
				case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
					return "DEPTH_STENCIL_READ_ONLY_OPTIMAL";
				case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
					return "SHADER_READ_ONLY_OPTIMAL";
				case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
					return "TRANSFER_SRC_OPTIMAL";
				case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
					return "TRANSFER_DST_OPTIMAL";
				case VK_IMAGE_LAYOUT_PREINITIALIZED:
					return "PREINITIALIZED";
				case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
					return "PRESENT_SRC_KHR";
				case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
					return "SHARED_PRESENT_KHR";
				case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
					return "DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL";
				case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
					return "DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL";
				case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
					return "DEPTH_ATTACHMENT_OPTIMAL";
				case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
					return "STENCIL_ATTACHMENT_OPTIMAL";
				case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
					return "STENCIL_READ_ONLY_OPTIMAL";
				case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
					return "DEPTH_READ_ONLY_OPTIMAL";
				default:
					return "UNKOWN";
			}
		}

		static uint32_t getFormatStride(IMAGE_FORMAT format) {
			uint32_t byte_per_pixel = 0;
			switch (format) {
				case IMAGE_FORMAT_R8_UINT:
				case IMAGE_FORMAT_R8:
				case IMAGE_FORMAT_SR8_NON_LINEAR:
				case IMAGE_FORMAT_R8_UNORM:
				case IMAGE_FORMAT_R8_SNORM:
					return 1;
					break;
				case IMAGE_FORMAT_RG8_UINT:
				case IMAGE_FORMAT_RG8:
				case IMAGE_FORMAT_SRG8_NON_LINEAR:
				case IMAGE_FORMAT_RG8_UNORM:
				case IMAGE_FORMAT_RG8_SNORM:
				case IMAGE_FORMAT_R16F:
				case IMAGE_FORMAT_R16_UINT:
				case IMAGE_FORMAT_R16_UNORM:
				case IMAGE_FORMAT_R16_SNORM:
					return 2;
				case IMAGE_FORMAT_RGB8_UINT:
				case IMAGE_FORMAT_RGB8:
				case IMAGE_FORMAT_SRGB8_NON_LINEAR:
				case IMAGE_FORMAT_RGB8_UNORM:
				case IMAGE_FORMAT_RGB8_SNORM:
					return 3;
				case IMAGE_FORMAT_RGBA8_UINT:
				case IMAGE_FORMAT_RGBA8:
				case IMAGE_FORMAT_SRGBA8_NON_LINEAR:
				case IMAGE_FORMAT_SBGRA8_NON_LINEAR:
				case IMAGE_FORMAT_R32_UINT:
				case IMAGE_FORMAT_R32F:
				case IMAGE_FORMAT_RGBA8_SNORM:
				case IMAGE_FORMAT_RGBA8_UNORM:
				case IMAGE_FORMAT_RG16F:
				case IMAGE_FORMAT_RG16_UINT:
				case IMAGE_FORMAT_RG16_UNORM:
				case IMAGE_FORMAT_RG16_SNORM:
					return 4;
				case IMAGE_FORMAT_RG32F:
				case IMAGE_FORMAT_RG32_UINT:
				case IMAGE_FORMAT_RGBA16F:
				case IMAGE_FORMAT_RGBA16_UINT:
				case IMAGE_FORMAT_RGBA16_UNORM:
				case IMAGE_FORMAT_RGBA16_SNORM:
					return 8;
					break;
				case IMAGE_FORMAT_RGB32F:
					return 12;
					break;
				case IMAGE_FORMAT_RGBA32F:
				case IMAGE_FORMAT_RGBA32_UINT:
					return 16;
				case IMAGE_FORMAT_DEPTH32F:
					return 4;
					break;
				case IMAGE_FORMAT_DEPTH32f_STENCIL8U:
					return 5;
					break;
				case IMAGE_FORMAT_DEPTH24f_STENCIL8U:
					return 4;
					break;
				case IMAGE_FORMAT_RGB16F:
				case IMAGE_FORMAT_RGB16_UINT:
				case IMAGE_FORMAT_RGB16_UNORM:
				case IMAGE_FORMAT_RGB16_SNORM:
					return 6;
					break;
			}
			Log::error("Format not supported");
			return 0;
		}

		static VkFormat getVkFormat(IMAGE_FORMAT format) {
			VkFormat vk_format = VK_FORMAT_UNDEFINED;
			switch (format) {
				case IMAGE_FORMAT_R8_UINT:
					vk_format = VK_FORMAT_R8_UINT;
					break;
				case IMAGE_FORMAT_RG8_UINT:
					vk_format = VK_FORMAT_R8G8_UINT;
					break;
				case IMAGE_FORMAT_RGB8_UINT:
					vk_format = VK_FORMAT_R8G8B8_UINT;
					break;
				case IMAGE_FORMAT_RGBA8_UINT:
					vk_format = VK_FORMAT_R8G8B8A8_UINT;
					break;
				case IMAGE_FORMAT_R8:
					vk_format = VK_FORMAT_R8_SINT;
					break;
				case IMAGE_FORMAT_RG8:
					vk_format = VK_FORMAT_R8G8_SINT;
					break;
				case IMAGE_FORMAT_RGB8:
					vk_format = VK_FORMAT_R8G8B8_SINT;
					break;
				case IMAGE_FORMAT_RGBA8:
					vk_format = VK_FORMAT_R8G8B8A8_SINT;
					break;
				case IMAGE_FORMAT_R32F:
					vk_format = VK_FORMAT_R32_SFLOAT;
					break;
				case IMAGE_FORMAT_RG32F:
					vk_format = VK_FORMAT_R32G32_SFLOAT;
					break;
				case IMAGE_FORMAT_RGB32F:
					vk_format = VK_FORMAT_R32G32B32_SFLOAT;
					break;
				case IMAGE_FORMAT_RGBA32F:
					vk_format = VK_FORMAT_R32G32B32A32_SFLOAT;
					break;
				case IMAGE_FORMAT_DEPTH32F:
					// look into choosing depth with device format properties: https://vulkan-tutorial.com/Depth_buffering
					vk_format = VK_FORMAT_D32_SFLOAT;
					break;
				case IMAGE_FORMAT_DEPTH32f_STENCIL8U:
					vk_format = VK_FORMAT_D32_SFLOAT_S8_UINT;
					break;
				case IMAGE_FORMAT_DEPTH24f_STENCIL8U:
					vk_format = VK_FORMAT_D24_UNORM_S8_UINT;
					break;
				case IMAGE_FORMAT_SBGRA8_NON_LINEAR:
					vk_format = VK_FORMAT_B8G8R8A8_SRGB;
					break;
				case IMAGE_FORMAT_SR8_NON_LINEAR:
					vk_format = VK_FORMAT_R8_SRGB;
					break;
				case IMAGE_FORMAT_SRG8_NON_LINEAR:
					vk_format = VK_FORMAT_R8G8_SRGB;
					break;
				case IMAGE_FORMAT_SRGB8_NON_LINEAR:
					vk_format = VK_FORMAT_R8G8B8_SRGB;
					break;
				case IMAGE_FORMAT_SRGBA8_NON_LINEAR:
					vk_format = VK_FORMAT_R8G8B8A8_SRGB;
					break;
				case IMAGE_FORMAT_R32_UINT:
					vk_format = VK_FORMAT_R32_UINT;
					break;
				case IMAGE_FORMAT_RG32_UINT:
					vk_format = VK_FORMAT_R32G32_UINT;
					break;
				case IMAGE_FORMAT_RGB32_UINT:
					vk_format = VK_FORMAT_R32G32B32_UINT;
					break;
				case IMAGE_FORMAT_RGBA32_UINT:
					vk_format = VK_FORMAT_R32G32B32A32_UINT;
					break;
				case IMAGE_FORMAT_R8_UNORM:
					vk_format = VK_FORMAT_R8_UNORM;
					break;
				case IMAGE_FORMAT_RG8_UNORM:
					vk_format = VK_FORMAT_R8G8_UNORM;
					break;
				case IMAGE_FORMAT_RGB8_UNORM:
					vk_format = VK_FORMAT_R8G8B8_UNORM;
					break;
				case IMAGE_FORMAT_RGBA8_UNORM:
					vk_format = VK_FORMAT_R8G8B8A8_UNORM;
					break;
				case IMAGE_FORMAT_R8_SNORM:
					vk_format = VK_FORMAT_R8_SNORM;
					break;
				case IMAGE_FORMAT_RG8_SNORM:
					vk_format = VK_FORMAT_R8G8_SNORM;
					break;
				case IMAGE_FORMAT_RGB8_SNORM:
					vk_format = VK_FORMAT_R8G8B8_SNORM;
					break;
				case IMAGE_FORMAT_RGBA8_SNORM:
					vk_format = VK_FORMAT_R8G8B8A8_SNORM;
					break;
				case IMAGE_FORMAT_R16F:
					vk_format = VK_FORMAT_R16_SFLOAT;
					break;
				case IMAGE_FORMAT_RG16F:
					vk_format = VK_FORMAT_R16G16_SFLOAT;
					break;
				case IMAGE_FORMAT_RGB16F:
					vk_format = VK_FORMAT_R16G16B16_SFLOAT;
					break;
				case IMAGE_FORMAT_RGBA16F:
					vk_format = VK_FORMAT_R16G16B16A16_SFLOAT;
					break;
				case IMAGE_FORMAT_R16_UINT:
					vk_format = VK_FORMAT_R16_UINT;
					break;
				case IMAGE_FORMAT_RG16_UINT:
					vk_format = VK_FORMAT_R16G16_UINT;
					break;
				case IMAGE_FORMAT_RGB16_UINT:
					vk_format = VK_FORMAT_R16G16B16_UINT;
					break;
				case IMAGE_FORMAT_RGBA16_UINT:
					vk_format = VK_FORMAT_R16G16B16A16_UINT;
					break;
				case IMAGE_FORMAT_R16_UNORM:
					vk_format = VK_FORMAT_R16_UNORM;
					break;
				case IMAGE_FORMAT_RG16_UNORM:
					vk_format = VK_FORMAT_R16G16_UNORM;
					break;
				case IMAGE_FORMAT_RGB16_UNORM:
					vk_format = VK_FORMAT_R16G16B16_UNORM;
					break;
				case IMAGE_FORMAT_RGBA16_UNORM:
					vk_format = VK_FORMAT_R16G16B16A16_UNORM;
					break;
				case IMAGE_FORMAT_R16_SNORM:
					vk_format = VK_FORMAT_R16_SNORM;
					break;
				case IMAGE_FORMAT_RG16_SNORM:
					vk_format = VK_FORMAT_R16G16_SNORM;
					break;
				case IMAGE_FORMAT_RGB16_SNORM:
					vk_format = VK_FORMAT_R16G16B16_SNORM;
					break;
				case IMAGE_FORMAT_RGBA16_SNORM:
					vk_format = VK_FORMAT_R16G16B16A16_SNORM;
					break;
			}
			return vk_format;
		}
	};
}
