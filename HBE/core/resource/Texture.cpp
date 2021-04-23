#include <core/utility/Log.h>
#include "Texture.h"
#include "fstream"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "ITexture.h"
#include "core/graphics/Graphics.h"
#include "core/graphics/Renderer.h"
#include "core/resource/IResourceFactory.h"

namespace HBE {
    Texture::~Texture() {
        delete instance;
    }

    Texture::Texture() {
        instance = Graphics::getRenderer()->getResourceFactory()->createTexture();
    }

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

    void Texture::setData(unsigned char *data, int width, int height, TEXTURE_FORMAT texture_type) {
        this->width = width;
        this->height = height;
        instance->setData(data, width, height, texture_type);
    }

    void Texture::bind(unsigned int slot) const {
        instance->bind(slot);
    }

    void Texture::unbind(unsigned int slot) const {
        instance->unbind(slot);
    }


}
