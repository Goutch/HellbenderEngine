#pragma once

#include "Core.h"
#include "Resource.h"

namespace HBE {
	enum IMAGE_FORMAT {
		IMAGE_R8,
		IMAGE_RG8,
		IMAGE_RGB8,
		IMAGE_RGBA8,
		IMAGE_R32F,
		IMAGE_RG32F,
		IMAGE_RGB32F,
		IMAGE_RGBA32F,
	};

	enum IMAGE_FLAGS {
		IMAGE_FLAG_NONE=0,
		IMAGE_FLAG_SHADER_WRITE=1,
	};


	class HB_API Image : public Resource {
	public:
		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;
		virtual uint32_t getDepth() const = 0;
		virtual void setData(void *data, uint32_t width = 1, uint32_t height = 1, uint32_t depth = 1, IMAGE_FORMAT format = IMAGE_RGBA8, IMAGE_FLAGS flags=IMAGE_FLAG_NONE) = 0;
		void load(std::string path);
	};
}