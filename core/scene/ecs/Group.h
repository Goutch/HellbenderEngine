
#pragma once
#include "Core.h"
#include "RegistryPage.h"
#include "ComponentPool.h"
namespace HBE {
	template<typename ... Components>
	class Group{
			std::vector < RegistryPage * > &pages;
			ComponentTypeInfo types[sizeof...(Components)];
			bool empty = false;
			public:
			Group(std::vector<RegistryPage *> &pages) : pages(pages) {
				empty = true;
			}

			Group(std::vector<RegistryPage *> &pages, ComponentTypeInfo types[sizeof...(Components)]) : pages(pages) {
				for (size_t i = 0; i < sizeof...(Components); i++) {
					this->types[i] = types[i];
				}
			}

			class iterator {
				std::vector < RegistryPage * > &pages;
				std::vector <entity_handle> entities;
				std::tuple < ComponentPool < Components >...> current_pools;
				ComponentTypeInfo types[sizeof...(Components)];
				std::bitset <REGISTRY_MAX_COMPONENT_TYPES> signature;
				size_t current_entity = 0;
				size_t current_page = 0;
				bool isEnd = false;
				private:

				template<std::size_t index, typename Component>
				void replacePools() {
					std::get<index>(current_pools) = ComponentPool<Component>(*pages[current_page]->getRawPool(types[index].signature_bit));
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
				std::tuple < entity_handle, Components &...> createTuple(entity_handle
				handle, std::index_sequence < indices...>) {
					return std::forward_as_tuple(handle, std::get<indices>(current_pools)[entities[current_entity]]...);
				}

				inline bool currentPageHasSignature() {
					return (pages[current_page]->components_signature & signature) == signature;
				};

				public:
				iterator(std::vector < RegistryPage * > &pages) :
				pages(pages),
						current_pools(std::tuple < ComponentPool < Components > ... >(ComponentPool<Components>()...)) {
					this->current_entity = 0;
					isEnd = true;
					current_page = pages.size();
				}

				iterator(std::vector < RegistryPage * > &pages, ComponentTypeInfo
				types[sizeof...(Components)], bool
				end) :
				pages(pages),
						current_pools(
								std::tuple < ComponentPool < Components > ... >(ComponentPool<Components>()...)) {
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

				iterator(iterator && other) : pages(other.pages)
				{
					this->current_entity = other.current_entity;
					this->current_page = other.current_page;
					this->components_signature = other.components_signature;
					this->current_pools = other.current_pools;
					this->entities = other.entities;
					this->pages = other.pages;
				}

				iterator(iterator & other) : pages(other.pages)
				{
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

				std::tuple < entity_handle, Components &...> operator*()
				{
					return std::move(createTuple(entities[current_entity], std::index_sequence_for < Components... > {}));
				}


				void getCurrentPageEntities() {
					RawComponentPool *raw_pools[sizeof...(Components)];
					for (size_t i = 0; i < sizeof...(Components); ++i) {
						raw_pools[i] = pages[current_page]->getRawPool(types[i].signature_bit);
						if (raw_pools[i] == nullptr) {
							entities.clear();
							return;
						}
					}
					size_t min_size_index = 0;
					for (size_t i = 1; i < sizeof...(Components); ++i) {
						if (raw_pools[min_size_index]->handles.size() > raw_pools[i]->handles.size()) {
							min_size_index = i;
						}
					}
					entities = raw_pools[min_size_index]->handles;
					for (size_t i = 0; i < sizeof...(Components); ++i) {
						if (i == min_size_index)
							continue;
						size_t entity_count = entities.size();
						for (size_t j = 0; j < entity_count; ++j) {
							while (!raw_pools[i]->has(entities[j])) {
								entity_count--;
								std::swap(entities[j], entities.back());
								entities.resize(entity_count);
								if (j < entity_count) break;
							}
						}

					}
					if (!entities.empty()) {
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
				if (empty)return iterator(pages);
				else return iterator(pages, types, false);
			};

			iterator end() {
				if (empty)return iterator(pages);
				else return iterator(pages, types, true);
			}
	};
}