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

		void reorder(std::vector<entity_handle> &entities_order) {
			for (size_t i = 0; i < entities_order.size(); ++i) {
				size_t current_entity_index = index_of_entity[entities_order[i]];
				size_t moved_entity_index = i;
				if (moved_entity_index != current_entity_index) {
					data.swap(moved_entity_index, current_entity_index);
					entity_handle moved_entity = entity_of_index[i];
					entity_handle current_entity = entities_order[i];


					index_of_entity[current_entity] = moved_entity_index;
					index_of_entity[moved_entity] = current_entity_index;
					entity_of_index[current_entity_index] = moved_entity;
					entity_of_index[moved_entity_index] = current_entity;

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
			size_t entity_index = index_of_entity[handle];
			entities[entity_index] = entities[entities.size() - 1];
			index_of_entity[entities[entity_index]] = entity_index;
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
			size_t hash = typeid(Component).hash_code();
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
			size_t hash = typeid(Component).hash_code();
			components[hash]->detach(handle);
			component_flags_of_entity[handle].set(bit_of_hash[hash], false);
			component_hash_codes_of_entity[handle].erase(component_hash_codes_of_entity[handle].find(hash));
		}

		template<typename Component>
		bool has() {
			size_t hash = typeid(Component).hash_code();
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
		std::vector<entity_handle> group() {
			std::vector<std::size_t> hashes = {typeid(Components).hash_code()...};

			std::bitset<MAX_COMPONENT_TYPES> flags;


			size_t min_size_index = 0;

			for (size_t i = 0; i < hashes.size(); ++i) {
				if (components.find(hashes[i]) == components.end())
					return std::vector<entity_handle>();

				if (i != 0 && components[hashes[i]]->size() < components[hashes[min_size_index]]->size())
					min_size_index = i;
				flags |= 1 << bit_of_hash[hashes[i]];
			}

			std::vector<entity_handle> entity_handle_group(components[hashes[min_size_index]]->getEntities().begin(), components[hashes[min_size_index]]->getEntities().end());
			size_t size = entity_handle_group.size();
			for (size_t i = 0; i < size; ++i) {
				while ((component_flags_of_entity[entity_handle_group[i]] & flags) != flags && i < size) {
					size--;
					entity_handle temp = entity_handle_group[i];
					entity_handle_group[i] = entity_handle_group[size];
					entity_handle_group[size] = temp;
				}
			}
			entity_handle_group.resize(size);

			for (size_t i = 0; i < hashes.size(); ++i) {
				components[hashes[i]]->reorder(entity_handle_group);
			}

			return entity_handle_group;
		}

		template<class Component>
		Component *get() {
			size_t hash = typeid(Component).hash_code();
			return components.find(hash) == components.end() ? nullptr : (Component *) components[hash]->raw();
		};

		template<class Component>
		Component &get(entity_handle handle) {
			return *((Component *) components[typeid(Component).hash_code()]->getComponent(handle));
		};

	};
}