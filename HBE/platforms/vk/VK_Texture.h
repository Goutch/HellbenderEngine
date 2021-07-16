#pragma once

#include "core/resource/Texture.h"
#include "VK_Device.h"
namespace HBE {
    class VK_Texture : public Texture {
        const VK_Device* device;
        VkImage handle;
        HBE::TEXTURE_FORMAT format;
    public:
        VK_Texture(const VK_Device* device);
        ~VK_Texture()= default;

        void setData(unsigned char *data, int width, int height, TEXTURE_FORMAT format) override;

        void bind(unsigned int slot) const override;

        void unbind(unsigned int slot) const override;
    };
}



