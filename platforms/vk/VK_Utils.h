
#pragma once
namespace HBE{
	class VK_Utils{

	public:
		static uint32_t getFormatStride(IMAGE_FORMAT format) {
			uint32_t byte_per_pixel = 0;
			switch (format) {
				case IMAGE_FORMAT_R8:
				case IMAGE_FORMAT_SR8_NON_LINEAR:
					return 1;
					break;
				case IMAGE_FORMAT_RG8:
				case IMAGE_FORMAT_SRG8_NON_LINEAR:

					return 2;
					break;
				case IMAGE_FORMAT_RGB8:
				case IMAGE_FORMAT_SRGB8_NON_LINEAR:
					return 3;

				case IMAGE_FORMAT_RGBA8:
				case IMAGE_FORMAT_SRGBA8_NON_LINEAR:
				case IMAGE_FORMAT_SBGRA8_NON_LINEAR:
				case IMAGE_FORMAT_R32F:
					return 4;
				case IMAGE_FORMAT_RG32F:
					return 8;
					break;
				case IMAGE_FORMAT_RGB32F:
					return 12;
					break;
				case IMAGE_FORMAT_RGBA32F:
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

			}
			return 0;
		}

		static VkFormat getVkFormat(IMAGE_FORMAT format) {
			VkFormat vk_format = VK_FORMAT_UNDEFINED;
			switch (format) {
				case IMAGE_FORMAT_R8:
					vk_format = VK_FORMAT_R8_UINT;
					break;
				case IMAGE_FORMAT_RG8:
					vk_format = VK_FORMAT_R8G8_UINT;
					break;
				case IMAGE_FORMAT_RGB8:
					vk_format = VK_FORMAT_R8G8B8_UINT;
					break;
				case IMAGE_FORMAT_RGBA8:
					vk_format = VK_FORMAT_R8G8B8A8_UINT;
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
			}
			return vk_format;
		}
	};
}