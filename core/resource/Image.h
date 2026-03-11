#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/interface/ImageInterface.h"
#include "core/Graphics.h"
namespace HBE {
	class HB_API Image {
		ImageHandle handle;
		Context &context;
	public:
		explicit Image(const ImageInfo &info);
		Image();
		Image(Image &&other) noexcept;
		Image(const Image &other) = delete;
		~Image();

		void alloc(const ImageInfo &info);
		void release();
		vec3u getSize();

		ImageHandle getHandle();
		void update(const void *data);

		void load(const char* path,IMAGE_FORMAT expected_format);

		static int getFormatNumberOfChannels(IMAGE_FORMAT format);
	};
}
