#pragma once

#include <cstdint>
#include <cstdlib>
#include "string"
struct ByteVector {
private:
	const size_t element_size;
	uint8_t *data = nullptr;
	uint8_t *temp = nullptr;
	size_t count;
	size_t capacity;

public:
	ByteVector(const ByteVector &) = delete;

	ByteVector(size_t element_size) : element_size(element_size) {
		count = 0;
		capacity = 2;
		data = (uint8_t *) malloc(element_size * capacity);
		temp = (uint8_t *)malloc(element_size);
	}

	~ByteVector() {
		if (data != nullptr)
			delete[] data;
		if (temp != nullptr)
			delete[] temp;
	}


	void add(uint8_t *element) {
		int index = element_size * count;
		if (capacity == count) {
			capacity *= 2;
			uint8_t *new_data = (uint8_t *) malloc(element_size * capacity);
			memcpy(new_data, data, element_size * count);
			free(data);
			data = new_data;
		}
		uint8_t *ptr_to_element = data + index;
		memcpy(ptr_to_element, element, element_size);
		count++;
	}

	void swap(size_t i, size_t j) {
		size_t byte_i = element_size * i;
		size_t byte_j = element_size * j;

		memcpy(temp, data + byte_i, element_size);

		memmove(data + byte_i, data + byte_j, element_size);
		memmove(data + byte_j, temp, element_size);
	}


	void remove(size_t i) {
		size_t byte_index = element_size * i;
		if (i != count - 1) {
			uint8_t *element_ptr = data + byte_index;
			uint8_t *next_element_ptr = data + byte_index + element_size;
			memmove(element_ptr, next_element_ptr, (count - i) - 1);
		}
		count--;
	}

	size_t size() const {
		return count;
	}

	uint8_t &operator[](size_t i) {
		return data[i * element_size];
	}
};
