
/*
 * ideas
 * When grouping check registry if type exist then create an empty group if not
 *
 *
 */
#include "Core.h"
#include "queue"
#include "list"
#include "unordered_map"
#include "unordered_set"
#include "map"
#include "bitset"
#include "typeinfo"
#include "set"
#include "core/utility/Log.h"
#include <cstring>
#include "algorithm"

#ifndef REGISTRY_PAGE_SIZE
#define REGISTRY_PAGE_SIZE 128
#endif
#ifndef REGISTRY_MAX_COMPONENT_TYPES
#define REGISTRY_MAX_COMPONENT_TYPES 128
#endif
namespace HBE {
	typedef uint32_t entity_handle;

	struct ComponentTypeInfo {
		size_t hash;
		uint32_t signature_bit;
		size_t size;

		bool operator==(const size_t hash) const {
			return this->hash == hash;
		}

		bool operator==(const ComponentTypeInfo &other) const {
			return hash == other.hash;
		}
	};

	class RawComponentPool {
	public:
		ComponentTypeInfo info;
		char *data;
		bool valid[REGISTRY_PAGE_SIZE];
		std::vector<entity_handle> handles;
		size_t offset;

		RawComponentPool(RawComponentPool &&other) {
			data = other.data;
			memcpy(valid, other.valid, REGISTRY_PAGE_SIZE * sizeof(bool));
			info = other.info;
			offset = other.offset;
		}

		RawComponentPool(ComponentTypeInfo info, size_t offset) : info(info) {
			data = static_cast<char *>(malloc(info.size * REGISTRY_PAGE_SIZE));

			memset(data, 0, info.size * REGISTRY_PAGE_SIZE);
			memset(&valid, 0, sizeof(bool) * REGISTRY_PAGE_SIZE);

			this->offset = offset;
		}

		~RawComponentPool() {
			delete[] data;
		}

		char *attach(entity_handle handle, char *component) {
			size_t i = handle - offset;
			if (!valid[i]) {
				handles.emplace_back(handle);
				valid[i] = true;
				std::sort(handles.begin(), handles.end());
			}
			memcpy(data + (i * info.size), component, info.size);
			return &data[i * info.size];
		}

		template<typename Component>
		Component &getAs(entity_handle handle) {
			size_t i = handle - offset;
			return *reinterpret_cast<Component *>(&data[i * info.size]);
		}

		void detach(entity_handle handle) {
			size_t i = handle - offset;
			if (valid[i]) {
				memset(data + (i * info.size), 0, info.size);
				valid[i] = false;
				handles.erase(std::find(handles.begin(), handles.end(), handle));
				std::sort(handles.begin(), handles.end());
			}
		}

		bool has(entity_handle handle) {
			return valid[handle - offset];
		}
	};

	template<typename Component>
	struct ComponentPool {
		Component *components = nullptr;
		size_t offset = 0;
		entity_handle *handles = nullptr;
		std::size_t count = 0;

		ComponentPool() {

		};

		ComponentPool(RawComponentPool &page) : handles(page.handles.data()) {
			components = reinterpret_cast<Component *>(&page.data[0]);
			count = page.handles.size();
			offset = page.offset;
		}

		Component &operator[](size_t handle) {
			return components[handle - offset];
		}
	};

	class RegistryPage {
	public:
		size_t offset;
		uint32_t count = 0;

		bool valid_entities[REGISTRY_PAGE_SIZE];
		std::bitset<REGISTRY_MAX_COMPONENT_TYPES> components_of_entity[REGISTRY_PAGE_SIZE];
		std::unordered_map<size_t, RawComponentPool *> component_pages;
		std::bitset<REGISTRY_MAX_COMPONENT_TYPES> components_signature = 0;

		RegistryPage(size_t page_index) : offset(page_index * REGISTRY_PAGE_SIZE) {
			for (int i = 0; i < REGISTRY_PAGE_SIZE; ++i) {
				valid_entities[i] = false;
				components_of_entity[i] = 0;
			}
		};

		size_t handleToIndex(entity_handle handle) {
			return handle - offset;
		}

		RawComponentPool *getRawPool(size_t hash) {
			auto it = component_pages.find(hash);
			return it == component_pages.end() ? nullptr : it->second;
		}

		template<typename Component>
		ComponentPool<Component> getPool() {
			return ComponentPool<Component>(component_pages[typeid(Component).hash_code()]);
		}

		template<typename Component>
		Component &attach(entity_handle handle, ComponentTypeInfo &type) {
			size_t i = handleToIndex(handle);
			if (!valid_entities[i]) Log::error("Enitty#" + std::to_string(handle) + "is not valid");


			components_of_entity[i] |= 1 << type.signature_bit;
			auto component_page_it = component_pages.find(type.hash);
			if (component_page_it == component_pages.end()) {
				component_pages.emplace(type.hash, new RawComponentPool(type, offset));
				components_signature.set(type.signature_bit, true);
			}
			Component component{};
			char *raw_ptr = component_pages[type.hash]->attach(handle, reinterpret_cast<char *>(&component));
			return *reinterpret_cast<Component *>(raw_ptr);
		};

		void detach(entity_handle handle, ComponentTypeInfo &type) {
			size_t i = handleToIndex(handle);

			components_of_entity[i] |= 1 << type.signature_bit;
			if (valid_entities[i]) {
				auto component_page_it = component_pages.find(type.hash);
				if (component_page_it == component_pages.end()) {
					return;
				}
				RawComponentPool &component_page = *component_page_it->second;
				component_page.detach(handle);
				if (component_page.handles.size() == 0) {
					components_signature.set(component_page.info.signature_bit, false);
					component_pages.erase(component_page_it->first);
				}
			}
		};

		void setValid(entity_handle handle) {
			valid_entities[handleToIndex(handle)] = true;
		}

		void setInvalid(entity_handle handle) {
			size_t i = handleToIndex(handle);
			valid_entities[i] = false;
			std::list<size_t> obsolete_types_hash;
			for (auto component_page_it: component_pages) {
				RawComponentPool &component_page = *component_page_it.second;
				component_page.detach(i);
				if (component_page.handles.size() == 0) {
					obsolete_types_hash.emplace_back(component_page_it.first);
				}
			}
			for (size_t hash: obsolete_types_hash) {
				component_pages.erase(hash);
			}
		}

		bool valid(entity_handle handle) {
			return valid_entities[handle - offset];
		}

	};

	template<typename ... Components>
	class Group {
		std::vector<RegistryPage *> &pages;
		ComponentTypeInfo types[sizeof...(Components)];
	public:
		Group(std::vector<RegistryPage *> &pages, ComponentTypeInfo types[sizeof...(Components)]) : pages(pages) {
			this->pages = pages;
			for (size_t i = 0; i < sizeof...(Components); i++) {
				this->types[i] = types[i];
			}
		}

		class iterator {
			std::vector<RegistryPage *> &pages;
			std::vector<entity_handle> entities;
			std::tuple<ComponentPool<Components>...> current_pools;
			ComponentTypeInfo types[sizeof...(Components)];
			std::bitset<REGISTRY_MAX_COMPONENT_TYPES> signature;
			size_t current_entity = 0;
			size_t current_page = 0;
			bool isEnd = false;
		private:

			template<std::size_t index, typename Component>
			void replacePools() {
				std::get<index>(current_pools) = ComponentPool<Component>(*pages[current_page]->component_pages[types[index].hash]);
			}

			template<typename... Parms>
			void func(Parms ...p) {}

			template<std::size_t... indices>
			void fillPages(std::index_sequence<indices...>) {
				//template black magic
				//https://stackoverflow.com/questions/6941176/variadic-template-parameter-pack-expanding-for-function-calls
				func((replacePools<indices, Components>(), 0)...);
			}

			template<std::size_t... indices>
			std::tuple<entity_handle, Components &...> createTuple(entity_handle handle, std::index_sequence<indices...>) {
				return std::forward_as_tuple(handle, std::get<indices>(current_pools)[entities[current_entity]]...);
			}

			bool currentPageHasSignature() {
				return (pages[current_page]->components_signature & signature) == signature;
			};

		public:
			iterator(std::vector<RegistryPage *> &pages, ComponentTypeInfo types[sizeof...(Components)], bool end) :
					pages(pages),
					current_pools(
							std::tuple<ComponentPool<Components>...>(ComponentPool<Components>()...)) {
				this->current_entity = 0;
				isEnd = end;

				for (size_t i = 0; i < sizeof...(Components); i++) {
					this->types[i] = types[i];
					signature.set(types[i].signature_bit);
				}

				if (isEnd)current_page = pages.size();
				else {
					getCurrentPageEntities();
					updatePage();
				}

			}

			iterator(iterator &&other) : pages(other.pages) {
				this->current_entity = other.current_entity;
				this->current_page = other.current_page;
				this->components_signature = other.components_signature;
				this->current_pools = other.current_pools;
				this->entities = other.entities;
				this->pages = other.pages;
			}

			iterator(iterator &other) : pages(other.pages) {
				this->current_entity = other.current_entity;
				this->current_page = other.current_page;
				this->components_signature = other.components_signature;
				this->current_pools = other.current_pools;
				this->entities = other.entities;
				this->pages = other.pages;
			}


			bool operator==(const iterator &other) const {
				return current_entity == other.current_entity &&
					   current_page == other.current_page &&
					   isEnd == other.isEnd &&
					   other.signature == signature;
			}

			bool operator!=(const iterator &other) const {
				return current_entity != other.current_entity ||
					   current_page != other.current_page ||
					   isEnd != other.isEnd ||
					   other.signature != signature;
			}

			std::tuple<entity_handle, Components &...> operator*() {
				return std::move(createTuple(entities[current_entity], std::index_sequence_for<Components...>{}));
			}


			void getCurrentPageEntities() {

				RawComponentPool *raw_pages[] = {pages[current_page]->getRawPool(typeid(Components).hash_code())...};
				for (size_t i = 0; i < sizeof...(Components); ++i) {
					if (raw_pages[i] == nullptr) {
						entities.clear();
						return;
					}
				}
				size_t min_size_index = 0;
				for (size_t i = 1; i < sizeof...(Components); ++i) {

					if (raw_pages[min_size_index]->handles.size() > raw_pages[i]->handles.size()) {
						min_size_index = i;
					}
				}
				entities = raw_pages[min_size_index]->handles;
				for (size_t i = 0; i < sizeof...(Components); ++i) {
					if (i == min_size_index)
						continue;
					size_t entity_count = entities.size();
					for (size_t j = 0; j < entity_count; ++j) {
						while (!raw_pages[i]->has(entities[j])) {
							entity_count--;
							std::swap(entities[j], entities.back());
							entities.resize(entity_count);
							if (j < entity_count) break;
						}
					}

				}
				if (entities.size() != 0) {
					fillPages(std::index_sequence_for<Components...>());
					std::sort(entities.begin(), entities.end());
				}
			}

			void updatePage() {
				while (!isEnd && current_entity >= entities.size()) {
					nextPage();
					if (!isEnd)
						getCurrentPageEntities();
				}
			}

			void nextPage() {
				do {
					current_page++;
				} while (current_page != pages.size() && (pages[current_page] == nullptr || !currentPageHasSignature()));
				current_entity = 0;
				if (current_page == pages.size()) {
					isEnd = true;
				}
			}

			iterator &operator++() {
				current_entity++;
				updatePage();

				return *this;
			}

		};

	public:
		iterator begin() {
			return iterator(pages, types, false);
		};

		iterator end() {
			return iterator(pages, types, true);
		}
	};

	class Registry {
		uint32_t current_handle = 0;
		std::queue<entity_handle> inactive;
		std::vector<RegistryPage *> pages = std::vector<RegistryPage *>();
		std::unordered_map<size_t, ComponentTypeInfo> types;
		uint32_t current_bit = 0;

		size_t getPage(entity_handle handle) {
			return (handle - (handle % REGISTRY_PAGE_SIZE)) / REGISTRY_PAGE_SIZE;
		}

	public:
		Registry() {

		}

		~Registry() {
			for (auto page: pages) {
				delete page;
			}
		}

		entity_handle create() {
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

		template<typename ... Components>
		Group<Components...> group() {

			ComponentTypeInfo ts[sizeof...(Components)] = {types[typeid(Components).hash_code()]...};
			return Group<Components...>(pages, ts);
		};

		template<typename Component>
		Component &get(entity_handle handle) {
			return pages[getPage(handle)]->component_pages[typeid(Component).hash_code()]->getAs<Component>(handle);
		}

		template<typename Component>
		bool has(entity_handle handle) {
			const size_t hash = typeid(Component).hash_code();
			auto page= pages[getPage(handle)];
			auto comp_page_it= page->component_pages.find(hash);

			if(comp_page_it!=page->component_pages.end())
				return comp_page_it->second->has(handle);
			else
				return false;
		}

		void destroy(entity_handle handle) {
			inactive.push(handle);
			pages[getPage(handle)]->setInvalid(handle);
		}

		bool valid(entity_handle handle) {
			size_t page = getPage(handle);
			if (pages[page] != nullptr) {
				return pages[page]->valid(handle);
			}
			return false;
		}

		template<typename Component>
		Component &attach(entity_handle handle) {

			const size_t hash = typeid(Component).hash_code();
			auto type_it = types.find(hash);
			if (type_it == types.end()) {
				types.emplace(hash, ComponentTypeInfo{hash, current_bit, sizeof(Component)});
				current_bit++;
			}
			size_t page = getPage(handle);
			return pages[page]->attach<Component>(handle, types[hash]);
		}

		template<typename Component>
		void detach(entity_handle handle) {
			const size_t hash = typeid(Component).hash_code();
			ComponentTypeInfo &type = types[hash];
			size_t page = getPage(handle);

		}

	};
}
