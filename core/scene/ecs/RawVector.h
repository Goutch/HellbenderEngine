
#pragma once

#include <cstdint>
#include <cstdlib>
#include "string"

namespace HBE {
	template<typename T>
	class RawVector {
		static_assert(std::is_trivially_copyable_v<T>,
		              "RawVector only supports trivially copyable types");
	private:
		const uint32_t CAPACITY_GROWTH_FACTOR = 2;
		const size_t ELEMENT_SIZE = sizeof(T);
		T *m_data = nullptr;
		size_t m_count = 0;
		size_t m_capacity = 0;

	public:


		RawVector() = default;

		RawVector(const RawVector &) = delete;

		RawVector &operator=(const RawVector &) = delete;

		~RawVector() {
			if (m_data != nullptr)
				delete[] m_data;
		}

		inline bool empty() {
			return m_count == 0;
		}

		inline void push_back(T element) {
			add(element);
		}

		inline void emplace_back(T element) {
			add(element);
		}

		void add(T element) {
			int index = m_count;
			if (m_count >= m_capacity) {
				reserve(m_capacity == 0 ? 1 : m_capacity * CAPACITY_GROWTH_FACTOR);
			}

			m_data[index] = element;
			m_count++;
		}

		size_t size() const noexcept { return m_count; }

		size_t capacity() const noexcept { return m_capacity; }

		bool empty() const noexcept { return m_count == 0; }

		void clear() {
			if (m_data != nullptr) {
				delete[] m_data;
				m_count = 0;
				m_capacity = 0;
			}
		}

		void swap(size_t i, size_t j) {
			T temp = m_data[i];
			m_data[i] = m_data[j];
			m_data[j] = temp;
		}


		void copy(const RawVector<T> &other) {
			clear();
			reserve(other.size());
			memcpy(m_data, other.data_ptr(), ELEMENT_SIZE * other.size());
			m_count = other.size();
		}

		void resize(size_t new_count) {
			reserve(new_count);
			m_count = new_count;
		}

		void resize(size_t new_count, T default_value) {
			uint32_t old_count = m_count;
			resize(new_count);

			for (size_t i = old_count; i < new_count; ++i) {
				m_data[i] = default_value;
			}
		}

		void addRange(T* data,uint32_t count) {
			if (count == 0)
				return;

			reserve(size() + count);

			memcpy(
				m_data + m_count,
				data,
				count * sizeof(T)
			);

			m_count += count;
		}

		void reserve(size_t new_capacity) {
			if (new_capacity <= m_capacity)
				return;

			T *new_data = new T[new_capacity];

			if (m_data) {
				std::memmove(new_data, m_data, ELEMENT_SIZE * m_count);
				delete[] m_data;
			}

			m_data = new_data;
			m_capacity = new_capacity;
		}

		void erase(size_t i) {
			remove(i);
		}

		void remove(size_t i) {
			if (i + 1 < m_count) {
				std::memmove(
						&m_data[i],
						&m_data[i + 1],
						ELEMENT_SIZE * (m_count - i - 1)
				);
			}
			--m_count;
		}


		T &operator[](size_t i) {
			return m_data[i];
		}

		T *data_ptr() { return m_data; }

		const T *data_ptr() const { return m_data; }
		// =========================
		// ITERATOR IMPLEMENTATION
		// =========================
		using iterator = T *;
		using const_iterator = const T *;

		iterator begin() { return m_data; }

		iterator end() { return m_data + m_count; }

		const_iterator begin() const { return m_data; }

		const_iterator end() const { return m_data + m_count; }

		const_iterator cbegin() const { return m_data; }

		const_iterator cend() const { return m_data + m_count; }

	};
};
