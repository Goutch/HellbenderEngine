#pragma once

#include "core/resource/StorageBuffer.h"

namespace HBE {
	class VK_Device;

	class VK_Buffer;

	class VK_StorageBuffer : public StorageBuffer {
		uint32_t size;
		uint32_t stride;
		uint32_t count;
		VK_Buffer *buffer;
	public:
		VK_StorageBuffer(VK_Device *device, const StorageBufferInfo &info);
		~VK_StorageBuffer() override;
		void update(const void *data) override;

		uint32_t getStride() const;
		uint32_t getCount() const;
		const VK_Buffer &getBuffer() const;
	};

}
