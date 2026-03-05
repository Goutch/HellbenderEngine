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
		Image(const ImageInfo &info);

		Image(Image &&other);

		~Image();

		Image(const Image &other) = delete;

		vec3u getSize();

		void release();

		void update(const void *data);

		void load(const std::string &path, ImageInfo &info);

		static int getFormatNumberOfChannels(IMAGE_FORMAT format);
	};
}
