#pragma once

#include "Core.h"
#include "vector"
#include "bitset"
#include "queue"
#include "unordered_map"
#include "typeinfo"
#include <typeindex>
#include "unordered_set"

namespace HBE {


	class IDPool {
		std::queue<uint32_t> inactive;
		std::vector<uint32_t> active;//index->t
		std::vector<uint32_t> indices;//t->Index
	public:
		uint32_t add() {
			uint32_t index = active.size();
			indices.emplace_back(index);
			if (inactive.empty()) {
				active.emplace_back(index);
			} else {
				active.emplace_back(inactive.front());
				inactive.pop();
			}
			return active[index];
		}

		void remove(uint32_t id) {
			inactive.emplace(id);

			uint32_t index = indices[id];
			uint32_t last_index = active.size() - 1;
			uint32_t last_id = active[last_index];

			active[index] = last_id;
			indices[last_id] = index;

			indices.resize(indices.size() - 1);
			active.resize(active.size() - 1);
		}

		uint32_t indexOf(uint32_t id) {
			return indices[id];
		}

		size_t size() {
			return active.size();
		}

		uint32_t &operator[](uint32_t i) {
			return active[i];
		}
	};

	struct CArray {
	private:
		const size_t element_size;
		uint8_t *data = nullptr;
		size_t count = 0;
		void *temp;
		size_t capacity = 2;

	public:
		CArray(size_t element_size) : element_size(element_size) {
			data = (uint8_t *) malloc(element_size * capacity);
			temp = malloc(element_size);
		}

		~CArray() {
			free(data);
			free(temp);
		}


		void add(void *element) {
			int index = element_size * count;
			if (capacity == count) {
				capacity *= 2;
				uint8_t *new_data = (uint8_t *) malloc(element_size * count);
				memcpy(new_data, data, element_size * count);
				free(data);
				data = new_data;
			}
			uint8_t *ptr_to_element = data + index;
			memcpy(ptr_to_element, element, element_size);
			count++;
		}

		void swap(size_t i, size_t j) {
			size_t index_i = element_size * i;
			size_t index_j = element_size * j;

			memcpy(temp, data + index_i, element_size);

			memcpy(data + index_i, data + index_j, element_size);
			memcpy(data + index_j, temp, element_size);
		}


		void remove(size_t i) {
			int index = element_size * i;
			if (i != count - 1) {
				uint8_t *element_ptr = data + index;
				uint8_t *next_element_ptr = data + index + element_size;
				memmove(element_ptr, next_element_ptr, (count - i) - 1);
			}
			count--;
		}

		size_t size() {
			return count;
		}

		uint8_t *operator[](size_t i) {
			return data + (i * element_size);
		}
	};

	class ComponentArray {
		CArray array;
		std::unordered_map<uint32_t, uint32_t> indices;
		std::unordered_map<uint32_t, uint32_t> entities;

	public:
		ComponentArray(size_t element_size) : array(CArray(element_size)) {

		}

		uint8_t *attach(uint32_t entity, void *data) {
			indices.emplace(entity, array.size());
			entities.emplace(array.size(), entity);
			array.add(data);
		}

		bool hasCompontent(uint32_t entity) {
			return entities.find(entity) == entities.end();
		}

		void detach(uint32_t entity) {
			uint32_t entity_index = indices[entity];
			uint32_t last_entity = entities[array.size() - 1];

			array.swap(entity_index, array.size() - 1);

			entities[entity_index] = last_entity;
			indices[last_entity] = entity_index;

			entities.erase(array.size() - 1);
			indices.erase(entity);
			array.remove(array.size() - 1);
		}

		CArray &Array() {
			return array;
		}
	};

	class HB_API ECSScene {
		IDPool entities;
		std::unordered_map<std::type_index, ComponentArray> components;
		std::vector<std::unordered_set<std::type_index>> entity_components;

	public:
		uint32_t create() {
			uint32_t entity = entities.add();
			if (entity_components.size() < entities.size()) {
				entity_components.resize(entities.size());
			}
			return entity;
		}

		void destroy(uint32_t entity) {
			for (std::type_index component_type:entity_components[entities.indexOf(entity)]) {
				components[component_type].detach(entity);
			}
			entity_components[entities.indexOf(entity)].clear();
			uint32_t index = entities.indexOf(entity);
			entities.remove(entity);

			entity_components[index] = entity_components[entity_components.size() - 1];
			entity_components.resize(entity_components.size() - 1);
		}

		template<typename T>
		T *attach(uint32_t entity, T &t) {
			const std::type_index index = std::type_index(typeid(T));
			auto it = components.find(index);
			if (it == components.end()) {
				it = components.emplace(index, ComponentArray(sizeof(T))).first;
			}
			entity_components[entity].emplace(index);
			return (T *) it->second.attach(entity, (void *) (&t));
		}

		template<typename T>
		void detach(uint32_t entity) {
			const auto index = std::type_index(typeid(T));
			auto it = components.find(index);
			if (it == components.end()) {
				it = components.emplace(index, ComponentArray(sizeof(T))).first;
			}
			entity_components[entity].erase(index);
			it->second.detach(entity);
		}
	};
}


