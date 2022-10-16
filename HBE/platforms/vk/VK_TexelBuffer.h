#pragma once

#include <core/resource/TexelBuffer.h>
namespace HBE {
	class VK_Device;
	class VK_Buffer;
	class VK_TexelBuffer: public TexelBuffer {

		VK_Buffer *buffer;
	public:
		VK_TexelBuffer(VK_Device* device, const TexelBufferInfo& info);
		void update(const void *data) override;
		~VK_TexelBuffer() override;
	};
}

