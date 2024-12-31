#pragma once

#include "core/resource/StorageBuffer.h"

namespace HBE {
	class VK_Device;

	class VK_Buffer;

	class VK_StorageBuffer : public StorageBuffer {
		bool deleteOnDestruction = true;
		uint32_t size;
		uint32_t stride;
		uint32_t count;
		VK_Buffer *buffer;
	public:
		VK_StorageBuffer(VK_Device *device, const StorageBufferInfo &info);

		VK_StorageBuffer(VK_Device *device, VK_Buffer *buffer, uint32_t stride, uint32_t count, bool deleteOnDestruction);

		~VK_StorageBuffer() override;

		void update(const void *data) override;

		uint32_t getStride() const override;

		const VK_Buffer &getBuffer() const;

		void update(const void *data, size_t size, size_t offset) override;

		uint32_t getCount() const override;
	};

}
