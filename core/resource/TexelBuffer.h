#pragma once
#include "Core.h"
#include "core/Graphics.h"

namespace HBE {
	class HB_API TexelBuffer {
		TexelBufferHandle handle = HBE_NULL_HANDLE;
		Context &context;

	public:
		explicit TexelBuffer(const TexelBufferInfo &info);
		TexelBuffer();
		TexelBuffer(TexelBuffer &&other) noexcept;
		TexelBuffer(const TexelBuffer &other) = delete;
		~TexelBuffer();
		void alloc(const TexelBufferInfo &info);
		bool allocated();
		void release();
		TexelBufferHandle getHandle();
		TexelBufferHandle &getHandleRef();
		void update(const void *data);
	};
}
