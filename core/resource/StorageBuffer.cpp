#include "StorageBuffer.h"
#include "core/Application.h"

namespace HBE {
	StorageBuffer::StorageBuffer(const BufferInfo &info) : context(*Application::instance->getContext()) {
		alloc(info);
	}

	StorageBuffer::StorageBuffer() : context(*Application::instance->getContext()) {
	}

	StorageBuffer::StorageBuffer(StorageBuffer &&other) noexcept: context(other.context) {
		handle = other.handle;
		other.handle = HBE_NULL_HANDLE;
	}

	StorageBuffer::~StorageBuffer() {
		release();
	}

	void StorageBuffer::alloc(const BufferInfo &info) {
		context.createBuffer(handle, info);
	}

	bool StorageBuffer::allocated() {
		return handle != HBE_NULL_HANDLE;
	}

	void StorageBuffer::release() {
		context.releaseBuffer(handle);
		handle = HBE_NULL_HANDLE;
	}

	BufferHandle StorageBuffer::getHandle() {
		return handle;
	}

	BufferHandle &StorageBuffer::getHandleRef() {
		return handle;
	}

	void StorageBuffer::update(const void *data) {
		context.updateBuffer(handle, data);
	}
}
