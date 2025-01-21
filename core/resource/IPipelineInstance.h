#pragma once

#include "Resource.h"
#include "string"
#include "Core.h"

namespace HBE {
	class Image;

	class StorageBuffer;

	class TexelBuffer;
	class HB_API IPipelineInstance : public Resource {
	public:
		virtual void bind() const = 0;

		virtual void unbind() const = 0;

		/*frame: The frame to set the data for. -1 for all frames */
		virtual void setUniform(uint32_t binding, const void *data, int32_t frame = -1) = 0;
		virtual void setUniform(const std::string &name, const void *data, int32_t frame = -1) = 0;

		virtual void setImage(uint32_t binding, const Image *texture, int32_t frame = -1, uint32_t mip_level = 0) = 0;
		virtual void setImageArray(uint32_t binding, Image **texture, uint32_t texture_count, int32_t frame = -1, uint32_t mip_level = 0) = 0;
		virtual void setImage(const std::string &name, const Image *texture, int32_t frame = -1, uint32_t mip_level = 0) = 0;
		virtual void setImageArray(const std::string &name, Image **texture, uint32_t texture_count, int32_t frame = -1, uint32_t mip_level = 0) = 0;

		virtual void setStorageBuffer(uint32_t binding, StorageBuffer *buffer, size_t count, size_t offset, int32_t frame) = 0;
		virtual void setStorageBufferArray(uint32_t binding, StorageBuffer **buffers, uint32_t count, int32_t frame = -1) = 0;
		virtual void setStorageBuffer(const std::string &name, StorageBuffer *buffer, size_t count, size_t offset, int32_t frame = -1) = 0;
		virtual void setStorageBufferArray(const std::string &name, StorageBuffer **buffers, uint32_t count, int32_t frame = -1) = 0;

		virtual void setTexelBuffer(uint32_t binding,TexelBuffer *buffer, int32_t frame = -1) = 0;
		virtual void setTexelBufferArray(uint32_t binding, TexelBuffer **buffers, uint32_t count, int32_t frame = -1) = 0;
		virtual void setTexelBuffer(const std::string &name, TexelBuffer *buffer, int32_t frame = -1) = 0;
		virtual void setTexelBufferArray(const std::string &name, TexelBuffer **buffers, uint32_t count, int32_t frame = -1) = 0;

	};
}
