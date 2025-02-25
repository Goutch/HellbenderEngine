#pragma once

#include "Core.h"
#include "Image.h"

namespace HBE
{
	typedef uint32_t TEXEL_BUFFER_FLAGS;

	enum TEXEL_BUFFER_FLAG
	{
		TEXEL_BUFFER_FLAG_NONE = 0,
	};

	struct TexelBufferInfo
	{
		IMAGE_FORMAT format = IMAGE_FORMAT_R8;
		uint32_t count = 0;
		TEXEL_BUFFER_FLAGS flags = TEXEL_BUFFER_FLAG_NONE;
		MEMORY_TYPE_FLAGS preferred_memory_type_flags = MEMORY_TYPE_FLAG_GPU_LOCAL;
	};

	class HB_API TexelBuffer : public Resource
	{
	public:
		virtual void update(const void* data) = 0;

		virtual ~TexelBuffer()
		{
		};
	};
}
