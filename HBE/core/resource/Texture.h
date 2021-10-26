#pragma once

#include "Core.h"
#include <string>
#include "core/serialization/Serializable.h"
#include "Resource.h"
#include "Image.h"

namespace HBE {

	class HB_API Texture : public Image {

	public:
		virtual ~Texture() = default;
		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;
		virtual uint32_t getDepth() const = 0;
		virtual void setData(void *data, uint32_t width = 1, uint32_t height = 1, uint32_t depth = 1, IMAGE_FORMAT format = IMAGE_RGBA8, IMAGE_FLAGS flags = IMAGE_FLAG_NONE) = 0;

	};

}