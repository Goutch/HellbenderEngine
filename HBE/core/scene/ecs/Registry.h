#pragma once

#include <vector>
#include "core/utility/RawVector.h"
#include "unordered_map"
#include "queue"
#include "typeinfo"
#include "map"
#include "bitset"
#include "unordered_set"


typedef uint32_t entity_handle;

namespace HBE {
#ifndef MAX_COMPONENT_TYPES
#define MAX_COMPONENT_TYPES 128
#endif

	template<typename T>
	void swapData(T &t1, T &t2) {
		T temp = t1;
		t1 = t2;
		t2 = temp;
	}

	class ComponentPool {
		RawVector data;
		std::unordered_map<entity_handle, uint32_t> index_of_entity;
		std::vector<entity_handle> entity_of_index;

	public:
		ComponentPool(size_t element_size) : data(RawVector(element_size)) {

		}

		uint8_t *attach(uint32_t entity, uint8_t *component) {
			index_of_entity.emplace(entity, data.size());
			entity_of_index.emplace_back(entity);
			data.add(component);
			return &data[data.size() - 1];
		}

		size_t size() const {
			return data.size();
		}

		std::vector<entity_handle> &getEntities() {
			return entity_of_index;
		}

		uint8_t *getComponent(entity_handle handle) {
			return &data[index_of_entity[handle]];
		}

		void reorder(entity_handle *entities_order, size_t count) {
			for (size_t i = 0; i < count; ++i) {
				size_t current_entity_index = index_of_entity[entities_order[i]];
				size_t moved_entity_index = i;
				if (moved_entity_index != current_entity_index) {
					data.swap(moved_entity_index, current_entity_index);

					entity_handle moved_entity = entity_of_index[i];
					entity_handle current_entity = entities_order[i];

					swapData(index_of_entity[current_entity], index_of_entity[moved_entity]);
					swapData(entity_of_index[current_entity_index], entity_of_index[moved_entity_index]);
				}
			}
		}

		bool hasComponent(uint32_t entity) {
			return index_of_entity.find(entity) == index_of_entity.end();
		}

		void detach(uint32_t entity) {
			uint32_t entity_index = index_of_entity[entity];
			uint32_t last_entity = entity_of_index[data.size() - 1];

			data.swap(entity_index, data.size() - 1);

			entity_of_index[entity_index] = last_entity;
			index_of_entity[last_entity] = entity_index;

			entity_of_index[entity_index] = last_entity;
			entity_of_index.pop_back();
			index_of_entity.erase(entity);
			data.remove(data.size() - 1);
		}

		uint8_t *raw() {
			return &data[0];
		}


	};

	template<typename ... Components>
	class ComponentGroup {
		size_t entity_count;
		entity_handle *entities;
		std::tuple<Components *...> components;
	public:
		ComponentGroup(uint32_t count, entity_handle *entities, Components *... args) {
			this->entity_count = count;
			this->components = std::forward_as_tuple(args...);
			this->entities = entities;
		}

		ComponentGroup() {
			this->entity_count = 0;
			this->entities = nullptr;
		}

		template<size_t ... seq>
		std::tuple<entity_handle, Components &...> create_tuple(size_t index) {
			return std::make_tuple(entities[index],std::get<seq>(components)[index]...);
		}

		std::tuple<entity_handle, Components &...> get(uint32_t index) {
			return create_tuple<std::index_sequence_for<Components...>()>(index);
		}

		struct iterator {
			size_t entity_count;
			entity_handle *entities;
			std::tuple<Components *...> components;
			uint32_t i = 0;

			iterator(entity_handle *entities, std::tuple<Components *...> components, size_t count, uint32_t i) {
				this->i = i;
				this->entities = entities;
				this->components = components;
				this->count = count;
			}

			std::tuple<entity_handle, Components &...> operator*() const {
				return std::make_tuple(entities[i],std::get<std::index_sequence_for<Components...>()>(components)[i]);
			}

			bool operator==(const iterator &other) const {
				return other.entities == entities &&
					   other.count == entity_count &&
					   other.i == i &&
					   other.components == components;
			}

			bool operator!=(const iterator &other) const {
				return other.entities != entities ||
					   other.count != entity_count ||
					   other.i != i ||
					   other.components != components;
			}

			iterator &operator++() {
				i++;
			}
		};


		size_t count() {
			return entity_count;
		}

		iterator begin() const {
			return iterator(entities, components, entity_count, 0);
		}

		iterator end() const {
			return iterator(entities, components, entity_count, entity_count);
		}

		iterator operator[](size_t i) {
			return iterator(entities, components, entity_count, i);;
		}
	};

	class Registry {
		//Entity
		std::vector<entity_handle> entities;
		std::queue<entity_handle> inactive;
		std::unordered_map<entity_handle, size_t> index_of_entity;

		std::unordered_map<size_t, ComponentPool *> components;

		//Maps
		std::vector<std::bitset<MAX_COMPONENT_TYPES>> component_flags_of_entity;
		std::vector<std::unordered_set<size_t>> component_hash_codes_of_entity;
		std::unordered_map<std::size_t, uint32_t> bit_of_hash;

		uint32_t current_bit = 0;
		//todo cached groups

	public:
		~Registry() {
			for (auto kv:components) {
				delete kv.second;
			}
		}

		entity_handle create() {

			entity_handle handle;
			if (!inactive.empty()) {
				handle = inactive.front();
				inactive.pop();

				component_flags_of_entity[handle] = 0;
				component_hash_codes_of_entity[handle].clear();
			} else {
				handle = entities.size();
				component_flags_of_entity.emplace_back(0);
				component_hash_codes_of_entity.emplace_back();
			}
			entities.emplace_back(handle);
			component_flags_of_entity[handle] = 0;

			return handle;
		}

		void destroy(entity_handle handle) {
			inactive.emplace(handle);
			swapData(index_of_entity[handle], entities.back());
			entities.pop_back();


			auto hash_codes = component_hash_codes_of_entity[handle];
			for (size_t hash:hash_codes) {
				components[hash]->detach(handle);
			}

			component_flags_of_entity[handle] = component_flags_of_entity[component_flags_of_entity.size() - 1];
			component_flags_of_entity.pop_back();
			component_hash_codes_of_entity[handle] = component_hash_codes_of_entity[component_flags_of_entity.size() - 1];
		}


		template<typename Component>
		Component &attach(entity_handle handle, Component &component) {
			const size_t hash = typeid(Component).hash_code();
			if (components.find(hash) == components.end()) {
				bit_of_hash.emplace(hash, current_bit);
				components.emplace(hash, new ComponentPool(sizeof(Component)));
				current_bit++;
			}

			Component &result = *(reinterpret_cast<Component *>(components[hash]->attach(handle, reinterpret_cast<uint8_t *>(&component))));
			component_flags_of_entity[handle].set(bit_of_hash[hash], true);
			component_hash_codes_of_entity[handle].emplace(hash);
			return result;
		}

		template<typename Component>
		Component &attach(entity_handle handle) {
			Component component{};
			return attach < Component > (handle, component);
		}

		template<typename Component>
		void detach(entity_handle handle) {
			const size_t hash = typeid(Component).hash_code();
			components[hash]->detach(handle);
			component_flags_of_entity[handle].set(bit_of_hash[hash], false);
			component_hash_codes_of_entity[handle].erase(component_hash_codes_of_entity[handle].find(hash));
		}

		template<typename Component>
		bool has() {
			const size_t hash = typeid(Component).hash_code();
			return component_flags_of_entity[hash].test(bit_of_hash[hash]);
		}

		bool valid(entity_handle handle) {
			return index_of_entity.find(handle) == index_of_entity.end();
		}


		//get all entities in component with lesser size
		//filter out rest of entities for each components with bitfield
		//sort xyz component array replicate the entity array indicies in the count range
		//create group with entity[] and x[] y[] z[]
		template<typename ... Components>
		ComponentGroup<Components...> group() {
			constexpr std::size_t component_count = sizeof...(Components);
			const size_t hashes[component_count] = {typeid(Components).hash_code()...};

			std::bitset<MAX_COMPONENT_TYPES> flags;

			size_t min_size_index = 0;

			for (size_t i = 0; i < component_count; ++i) {
				if (components.find(hashes[i]) == components.end())
					return ComponentGroup<Components...>();

				if (i != 0 && components[hashes[i]]->size() < components[hashes[min_size_index]]->size())
					min_size_index = i;
				flags |= 1 << bit_of_hash[hashes[i]];
			}

			std::vector<entity_handle> entity_handle_group(components[hashes[min_size_index]]->getEntities().begin(), components[hashes[min_size_index]]->getEntities().end());
			size_t entity_count = entity_handle_group.size();
			for (size_t i = 0; i < entity_count; ++i) {
				while ((component_flags_of_entity[entity_handle_group[i]] & flags) != flags && i < entity_count) {
					entity_count--;
					swapData(entity_handle_group[i], entity_handle_group[entity_count]);
				}
			}
			entity_handle_group.resize(entity_count);
			for (size_t i = 0; i < component_count; ++i) {
				components[hashes[i]]->reorder(entity_handle_group.data(), entity_count);
			}

			return ComponentGroup<Components...>(entity_count, entity_handle_group.data(), get<Components>()...);
		}

		template<class Component>
		Component *get() {
			const size_t hash = typeid(Component).hash_code();
			return components.find(hash) == components.end() ? nullptr : (Component *) components[hash]->raw();
		};

		template<class Component>
		Component &get(entity_handle handle) {
			const size_t hash = typeid(Component).hash_code();
			return *((Component *) components[hash]->getComponent(handle));
		};

	};
}