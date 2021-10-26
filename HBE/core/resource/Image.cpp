#include "Image.h"
#include "fstream"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
namespace HBE{
	void Image::load(std::string path) {
		std::ifstream file(path.c_str());
		int32_t width,height;
		if (file.good()) {
			unsigned char *buffer;
			int nb_channels;
			stbi_set_flip_vertically_on_load(true);
			buffer = stbi_load(path.c_str(), &width, &height, &nb_channels, 4);
			setData(buffer, width, height,1, IMAGE_FORMAT::IMAGE_RGBA8);
			if (buffer) {
				stbi_image_free(buffer);
			}
		} else {
			Log::error("Failed to load texture:" + path);
		}
	}
}
