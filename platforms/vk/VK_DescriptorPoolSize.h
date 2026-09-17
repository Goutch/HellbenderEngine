
#pragma once

#include "vulkan/vulkan.h"

namespace HBE {
	//VK_PoolSize is just a standardized way to store VkDescriptorPoolSize with the engine supported descriptor types.
	struct VK_DescriptorPoolSize {
		static const uint32_t DESCRIPTOR_TYPES_COUNT = 10; // not including the raytracing descriptor types
		static constexpr VkDescriptorType DESCRIPTOR_TYPES[DESCRIPTOR_TYPES_COUNT] = {
				VK_DESCRIPTOR_TYPE_SAMPLER,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
				VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
				VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
				VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
				VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR
		};
		VkDescriptorPoolSize sizes[DESCRIPTOR_TYPES_COUNT] = {
				{VK_DESCRIPTOR_TYPE_SAMPLER,                    0},
				{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,     0},
				{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,              0},
				{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,              0},
				{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,       0},
				{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,       0},
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,             0},
				{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,             0},
				{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,           0},
				{VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 0}
		};
		uint32_t non_zero_sizes_count = 0;
		VkDescriptorPoolSize non_zero_sizes[DESCRIPTOR_TYPES_COUNT] = {
				{VK_DESCRIPTOR_TYPE_MAX_ENUM,                    0},
				{VK_DESCRIPTOR_TYPE_MAX_ENUM,     0},
				{VK_DESCRIPTOR_TYPE_MAX_ENUM,              0},
				{VK_DESCRIPTOR_TYPE_MAX_ENUM,              0},
				{VK_DESCRIPTOR_TYPE_MAX_ENUM,       0},
				{VK_DESCRIPTOR_TYPE_MAX_ENUM,       0},
				{VK_DESCRIPTOR_TYPE_MAX_ENUM,             0},
				{VK_DESCRIPTOR_TYPE_MAX_ENUM,             0},
				{VK_DESCRIPTOR_TYPE_MAX_ENUM,           0},
				{VK_DESCRIPTOR_TYPE_MAX_ENUM, 0}
		};

		VkDescriptorPoolSize &operator[](VkDescriptorType type) {
			switch (type) {
				case VK_DESCRIPTOR_TYPE_SAMPLER :
					return sizes[0];
				case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER :
					return sizes[1];
				case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE :
					return sizes[2];
				case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE :
					return sizes[3];
				case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER :
					return sizes[4];
				case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER :
					return sizes[5];
				case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER :
					return sizes[6];
				case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER :
					return sizes[7];
				case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT :
					return sizes[8];
				case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR :
					return sizes[9];
			}
			return sizes[0]; //default return, should never happen
		}

		void operator-=(const VK_DescriptorPoolSize &other) {
			for (int i = 0; i < DESCRIPTOR_TYPES_COUNT; ++i) {
				sizes[i].descriptorCount -= other.sizes[i].descriptorCount;
			}
		}

		void operator+=(const VK_DescriptorPoolSize &other) {
			for (int i = 0; i < DESCRIPTOR_TYPES_COUNT; ++i) {
				sizes[i].descriptorCount += other.sizes[i].descriptorCount;
			}
		}

		void operator+=(VkDescriptorPoolSize other) {
			(*this)[other.type].descriptorCount += other.descriptorCount;
		}

		void operator-=(VkDescriptorPoolSize other) {
			(*this)[other.type].descriptorCount -= other.descriptorCount;
		}

		VkDescriptorPoolSize* getNonZeroSizes() {
			non_zero_sizes_count = 0;
			for (int i = 0; i < DESCRIPTOR_TYPES_COUNT; ++i) {
				if (sizes[i].descriptorCount > 0) {
					non_zero_sizes[non_zero_sizes_count] = sizes[i];
					non_zero_sizes_count++;
				}
			}
			return non_zero_sizes;
		}

		uint32_t getNonZeroSizesCount() {
			return non_zero_sizes_count;
		}

		void initCore(uint32_t default_count) {
			for (int i = 0; i < DESCRIPTOR_TYPES_COUNT-1; ++i) {
				sizes[i].descriptorCount = default_count;
			}
		}
		void initRaytracing(uint32_t default_count) {
				(*this)[VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR].descriptorCount = default_count;
		}

		bool fit(const VK_DescriptorPoolSize &other) const {
			for (int i = 0; i < DESCRIPTOR_TYPES_COUNT; ++i) {
				if (sizes[i].descriptorCount < other.sizes[i].descriptorCount) {
					return false;
				}
			}
			return true;
		}

		VkDescriptorPoolSize biggestSize() const {
			VkDescriptorPoolSize max_size = sizes[0];
			for (int i = 1; i < DESCRIPTOR_TYPES_COUNT; ++i) {
				if (sizes[i].descriptorCount > max_size.descriptorCount) {
					max_size = sizes[i];
				}
			}
			return max_size;
		}
	};

}