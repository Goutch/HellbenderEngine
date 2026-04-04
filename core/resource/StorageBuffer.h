#pragma once

#include "Core.h"
#include "core/Graphics.h"

namespace HBE {

	class HB_API StorageBuffer {
		BufferHandle handle = HBE_NULL_HANDLE;
		Context &context;
	public:
		explicit StorageBuffer(const BufferInfo &info);

		StorageBuffer();

		StorageBuffer(StorageBuffer &&other) noexcept;

		StorageBuffer(const StorageBuffer &other) = delete;

		~StorageBuffer();

		void alloc(const BufferInfo &info);

		void release();

		BufferHandle getHandle();

		BufferHandle &getHandleRef();

		void update(const void *data);
	};
}
