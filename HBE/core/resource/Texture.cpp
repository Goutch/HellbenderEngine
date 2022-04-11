#include "Texture.h"
#include "fstream"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "Resources.h"

namespace HBE {
	Texture *Texture::load(std::string path) {
		std::ifstream file((RESOURCE_PATH+path).c_str());
		if (file.good()) {
			TextureInfo info{};
			Texture *texture;
			int32_t width, height;
			unsigned char *buffer;
			int nb_channels;
			stbi_set_flip_vertically_on_load(true);
			buffer = stbi_load(path.c_str(), &width, &height, &nb_channels, 4);
			info.width = width;
			info.height = height;
			info.format = IMAGE_FORMAT_RGBA8;
			info.flags = IMAGE_FLAG_NONE;
			info.data=buffer;
			texture = Resources::createTexture(info);
			if (buffer) {
				stbi_image_free(buffer);
			}
			return texture;
		}
		Log::error("Failed to load texture:" + path);
		return nullptr;

	}
}
