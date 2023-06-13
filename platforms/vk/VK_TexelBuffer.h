#pragma once

#include <core/resource/TexelBuffer.h>
#include "vulkan/vulkan.h"

namespace HBE {
	class VK_Device;

	class VK_Buffer;

	class VK_TexelBuffer : public TexelBuffer {
		uint32_t count;
		VkBufferView view;
		VK_Buffer *buffer;
	public:
		VK_TexelBuffer(VK_Device *device, const TexelBufferInfo &info);

		void update(const void *data) override;

		const VkBufferView &getView() const;

		~VK_TexelBuffer() override;
	};
}

