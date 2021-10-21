#pragma once

#include "core/resource/Texture.h"
#include "VK_Device.h"
#include "vulkan/vulkan.h"
#include "VK_Allocator.h"


namespace HBE {
	class VK_Buffer;

	class VK_Renderer;

	class VK_Texture : public Texture {
		VK_Device *device;
		VkImage handle = VK_NULL_HANDLE;
		VkImageView view_hanlde=VK_NULL_HANDLE;
		VkSampler sampler_handle=VK_NULL_HANDLE;
		HBE::TEXTURE_FORMAT format;
		Allocation *allocation;
	public:
		VK_Texture(VK_Device *device);
		~VK_Texture();

		void setData(unsigned char *data, int width, int height, TEXTURE_FORMAT format) override;

		const VkSampler &getSampler() const;
		const VkImageView &getImageView() const;
		const VkImage &getImage() const;
	};
}



