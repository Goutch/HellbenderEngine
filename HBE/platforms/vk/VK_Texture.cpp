

#include "VK_Texture.h"
namespace HBE{
    void VK_Texture::setData(unsigned char *data, int width, int height, TEXTURE_FORMAT format) {

    }

    void VK_Texture::bind(unsigned int slot) const {

    }

    void VK_Texture::unbind(unsigned int slot) const {

    }

    VK_Texture::VK_Texture(const HBE::VK_Device *device) {
        this->device = device;

    }
}

