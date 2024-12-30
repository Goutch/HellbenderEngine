#include "Texture.h"
#include "fstream"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "Resources.h"

namespace HBE {
	int Texture::getFormatNumberOfChannels(IMAGE_FORMAT format) {
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


	Texture *Texture::load(const std::string &path, IMAGE_FORMAT format, IMAGE_FLAGS flags) {
		FILE *file = fopen((RESOURCE_PATH + path).c_str(), "rb");

		if (file == nullptr) {
			Log::error("Failed to open file: " + path);
		}
		TextureInfo info{};
		Texture *texture;
		int32_t width, height;
		unsigned char *buffer;

		stbi_set_flip_vertically_on_load(true);
		int nb_channels;
		int expected_channels = Texture::getFormatNumberOfChannels(format);
		buffer = stbi_load_from_file(file, &width, &height, &nb_channels, expected_channels);

		if (nb_channels != expected_channels) {
			Log::error("Texture format missmatch. Expected " + std::to_string(expected_channels) + " channels, got " + std::to_string(nb_channels));
		}
		if (buffer == nullptr) {
			Log::error("Failed to load texture: " + path);
		}
		info.width = width;
		info.height = height;
		info.data_format = format;
		info.format = format;
		info.flags = flags;
		info.data = buffer;
		texture = Resources::createTexture(info, path);
		stbi_image_free(buffer);
		stbi_set_flip_vertically_on_load(true);
		fclose(file);
		return texture;
	}

}
