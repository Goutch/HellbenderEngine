#include <core/utility/Log.h>
#include "Texture.h"
#include "fstream"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "core/graphics/Graphics.h"
#include "core/graphics/Renderer.h"
#include "core/resource/ResourceFactory.h"

namespace HBE {

    void Texture::load(std::string path) {
        this->path = path;
        std::ifstream file(path.c_str());
        if (file.good()) {
            unsigned char *buffer;
            int nb_channels;
            stbi_set_flip_vertically_on_load(true);
            buffer = stbi_load(path.c_str(), &width, &height, &nb_channels, 4);
            setData(buffer, width, height, TEXTURE_FORMAT::RGBA8);
            if (buffer) {
                stbi_image_free(buffer);
            }
        } else {
            Log::error("Failed to load texture:" + path);
        }
    }

    void Texture::setData(unsigned char *data, int width, int height, TEXTURE_FORMAT format) {
        this->width = width;
        this->height = height;
        setData(data,width,height,format);
    }
}
