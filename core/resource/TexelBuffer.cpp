#include "TexelBuffer.h"
#include "core/Application.h"

namespace HBE {
	TexelBuffer::TexelBuffer(const TexelBufferInfo &info) : context(*Application::instance->getContext()) {
		alloc(info);
	}

	TexelBuffer::TexelBuffer() : context(*Application::instance->getContext()) {

	}

	TexelBuffer::TexelBuffer(TexelBuffer &&other) noexcept : context(other.context), handle(other.handle) {
		other.handle = HBE_NULL_HANDLE;
	}

	TexelBuffer::~TexelBuffer() {
		release();
	}

	void TexelBuffer::alloc(const TexelBufferInfo &info) {
		context.createTexelBuffer(handle, info);
	}

	bool TexelBuffer::allocated() {
		return handle != HBE_NULL_HANDLE;
	}

	void TexelBuffer::release() {
		context.releaseTexelBuffer(handle);
	}

	TexelBufferHandle TexelBuffer::getHandle() {
		return handle;
	}

	TexelBufferHandle &TexelBuffer::getHandleRef() {
		return handle;
	}

	void TexelBuffer::update(const void *data) {

	}
}
