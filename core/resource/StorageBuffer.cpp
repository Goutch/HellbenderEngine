
#include "StorageBuffer.h"
namespace HBE{

	StorageBuffer::StorageBuffer(const BufferInfo &info) {

	}

	StorageBuffer::StorageBuffer() {

	}

	StorageBuffer::StorageBuffer(StorageBuffer &&other) noexcept {

	}

	StorageBuffer::~StorageBuffer() {

	}

	void StorageBuffer::alloc(const BufferInfo &info) {

	}

	void StorageBuffer::release() {

	}

	uint32_t StorageBuffer::getSize() {
		return 0;
	}

	BufferHandle StorageBuffer::getHandle() {
		return nullptr;
	}

	BufferHandle &StorageBuffer::getHandleRef() {
		return <#initializer#>;
	}

	void StorageBuffer::update(const void *data) {

	}
}