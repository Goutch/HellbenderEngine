#include <core/utility/Log.h>
#include "Texture.h"
#include <graphics/gl/GL_Texture.h>

#include "fstream"
#include "core/serialization/Serializer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
Texture* Texture::load(std::string path) {
    this->path=path;
    std::ifstream file(path.c_str());
    if (file.good()) {
        unsigned char *buffer;
        int nb_channels;
        stbi_set_flip_vertically_on_load(true);
        buffer = stbi_load(path.c_str(), &width, &height, &nb_channels, 4);
        setData(buffer, width, height,TEXTURE_TYPE::RGBA8);
        if (buffer) {
            stbi_image_free(buffer);
        }
    } else {
        Log::error("Failed to load texture:" + path);
    }
    return this;
}

Texture *Texture::create() {
#if RENDERER == OPENGL_RENDERER
    return new GL_Texture();
#elif RENDERER == VULKAN_RENDERER

#endif
    return nullptr;
}

void Texture::load(std::string path, unsigned char *buffer,int& width,int& height) {
    std::ifstream file(path.c_str());
    if (file.good()) {
        int nb_channels;
        stbi_set_flip_vertically_on_load(true);
        buffer = stbi_load(path.c_str(), &width, &height, &nb_channels, 4);
    } else {
        Log::error("Failed to load texture:" + path);
    }
}

void Texture::serialize(Serializer *serializer) const {
    serializer->begin("Texture");
    serializer->addField("path",path);
    serializer->end();
}
