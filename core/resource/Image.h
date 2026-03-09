#pragma once

#include "Core.h"
#include <string>
#include "HBETypes.h"
#include "core/interface/ImageInterface.h"

namespace HBE {
	class Context;

	class HB_API Image {
		ImageHandle handle;
		Context *context = nullptr;
	public:
		explicit Image(const ImageInfo &info);
		Image(Image &&other) noexcept;
		Image(const Image &other) = delete;
		~Image();

		void alloc(const ImageInfo &info);
		void release();
		vec3u getSize();

		void update(const void *data);

		void load(const char* path,IMAGE_FORMAT expected_format);

		static int getFormatNumberOfChannels(IMAGE_FORMAT format);
	};
}
