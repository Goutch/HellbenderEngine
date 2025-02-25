#pragma once

#include "Core.h"
#include "Resource.h"
#include "core/graphics/Allocator.h"

namespace HBE
{
	typedef uint32_t STORAGE_BUFFER_FLAGS;

	enum STORAGE_BUFFER_FLAG
	{
		STORAGE_BUFFER_FLAG_NONE = 0,
	};

	struct StorageBufferInfo
	{
		uint32_t stride = 0;
		uint32_t count = 0;
		MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
		STORAGE_BUFFER_FLAGS flags = STORAGE_BUFFER_FLAG_NONE;
	};

	class HB_API StorageBuffer : public Resource
	{
	public :
		virtual void update(const void* data) = 0;

		virtual void update(const void* data, size_t size, size_t offset = 0) = 0;

		virtual uint32_t getCount() const = 0;

		virtual uint32_t getStride() const = 0;

		virtual ~StorageBuffer()
		{
		};
	};
}
