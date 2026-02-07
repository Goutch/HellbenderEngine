//
// Created by User on 3/24/2023.
//

#include "ComponentPool.h"

namespace HBE {
	RawComponentPool::RawComponentPool(RawComponentPool &&other) {
		data = other.data;
		memcpy(valid, other.valid, REGISTRY_PAGE_SIZE * sizeof(bool));
		info = other.info;
		offset = other.offset;
	}

	RawComponentPool::RawComponentPool(ComponentTypeInfo info, size_t offset) : info(info) {
		data = static_cast<char *>(malloc(info.size * REGISTRY_PAGE_SIZE));


		memset(data, 0, info.size * REGISTRY_PAGE_SIZE);
		memset(&valid, 0, sizeof(bool) * REGISTRY_PAGE_SIZE);
		this->offset = offset;
	}

	RawComponentPool::~RawComponentPool() {
		delete[] data;
	}

	char *RawComponentPool::getMemory(entity_handle handle) {
		size_t i = handle - offset;
		if (!valid[i]) {
			handles.emplace_back(handle);
			valid[i] = true;
			std::sort(handles.begin(), handles.end());
		}
		return &data[i * info.size];
	}

	void RawComponentPool::detach(entity_handle handle) {
		size_t i = handle - offset;
		if (valid[i]) {
			memset(data + (i * info.size), 0, info.size);
			valid[i] = false;
			for (int j = handles.size() - 1; j <= 0; --j) {
				if (handles[j] == handle) {
					handles.erase(j);
					break;
				}
			}
			std::sort(handles.begin(), handles.end());
		}
	}

	bool RawComponentPool::has(entity_handle handle) {
		return valid[handle - offset];
	}
}
