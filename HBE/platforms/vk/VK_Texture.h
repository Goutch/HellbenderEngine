#pragma once

#include "core/resource/ITexture.h"
#include "VK_Device.h"
namespace HBE {
    class VK_Texture : public ITexture {
        const VK_Device* device;
    public:
        VK_Texture(const VK_Device* device);
        ~VK_Texture() override = default;

        void setData(unsigned char *data, int width, int height, TEXTURE_FORMAT format) override;

        void bind(unsigned int slot) const override;

        void unbind(unsigned int slot) const override;
    };
}



