#include "Registry.h"

namespace HBE {
	uint32_t ComponentTypeRegistry::current_bit = 0;

	bool Registry::valid(HBE::entity_handle handle) {
		size_t page = getPage(handle);
		if (page < pages.size() && pages[page] != nullptr) {
			return pages[page]->valid(handle);
		}
		return false;
	}

	void Registry::destroy(entity_handle handle) {
		inactive.push(handle);

		pages[getPage(handle)]->setInvalid(handle);

	}

	entity_handle Registry::create() {
		entity_handle handle;
		if (inactive.empty()) {
			handle = current_handle;
			current_handle++;
		} else {
			handle = inactive.front();
			inactive.pop();
		}
		size_t page = getPage(handle);
		if (pages.size() <= page) {
			pages.resize(page + 1);
		}
		if (pages[page] == nullptr) {
			pages[page] = new RegistryPage(page);
		}
		pages[page]->setValid(handle);
		return handle;
	}

	Registry::~Registry() {
		for (auto page: pages) {
			delete page;
		}
	}

	void Registry::group(std::vector<entity_handle> &entities, size_t signature_bit) {
		for (auto page: pages) {
			RawComponentPool *pool = page->getRawPool(signature_bit);
			if (pool != nullptr)
				entities.insert(entities.end(), pool->handles.begin(), pool->handles.end());
		}
	}

	std::bitset<128> &Registry::getSignature(entity_handle handle) {
		return pages[getPage(handle)]->getSignature(handle);
	};
}

