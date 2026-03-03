#include "Image.h"
#include "fstream"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "core/utility/Log.h"
#include "core/Application.h"

namespace HBE {
	Image::Image(const ImageInfo &info) : context(*Application::instance->getContext()) {
		this->handle = context->createImage(info);
	}

	vec3u Image::getSize() {
		return context.getImageSize(handle);
	}

	void Image::release() {
		context.releaseImage(handle);
	}

	void Image::update(const void *data) {
		context.updateImage(handle, data);
	}

	int Image::getFormatNumberOfChannels(IMAGE_FORMAT format) {
		switch (format) {
			case IMAGE_FORMAT_R32F:
			case IMAGE_FORMAT_R8:
				return 1;
			case IMAGE_FORMAT_RG32F:
			case IMAGE_FORMAT_RG8:
				return 2;
			case IMAGE_FORMAT_RGB32F:
			case IMAGE_FORMAT_RGB8:
				return 3;
			case IMAGE_FORMAT_RGBA32F:
			case IMAGE_FORMAT_RGBA8:
				return 4;
			default:
				return 4;
		}
	}


	Image Image::load(const std::string &path, ImageInfo &info) {
		FILE *file = fopen((RESOURCE_PATH + path).c_str(), "rb");

		if (file == nullptr) {
			Log::error("Failed to open file: " + (RESOURCE_PATH + path));
		}
		int32_t width, height;
		unsigned char *buffer;

		stbi_set_flip_vertically_on_load(true);
		int nb_channels;
		int expected_channels = Image::getFormatNumberOfChannels(info.format);
		buffer = stbi_load_from_file(file, &width, &height, &nb_channels, expected_channels);

		if (nb_channels != expected_channels) {
			Log::error(
					"Texture format missmatch. Expected " + std::to_string(expected_channels) + " channels, got " + std::
					to_string(nb_channels));
		}
		if (buffer == nullptr) {
			Log::error("Failed to load texture: " + path);
		}
		info.width = width;
		info.height = height;
		info.data = buffer;
		info.data_format = info.format;
		texture = context->createImage(info);
		stbi_image_free(buffer);
		stbi_set_flip_vertically_on_load(true);
		fclose(file);
		info.data = nullptr;
		return texture;
	}
	Image::Image(Image &&other)  noexcept {
		handle = other.handle;
		context = other.context;

		other.handle = HBE_NULL_HANDLE;
	}

	Image::~Image() {
		if (handle != HBE_NULL_HANDLE)
			context.releaseImage(handle);
	}




}
