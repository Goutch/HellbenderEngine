#pragma once

#include <string>
#include "core/serialization/Serializable.h"
#include "Core.h"
#include "Resource.h"
namespace HBE {
    enum TEXTURE_FORMAT {
        RGB8,
        RGBA8,
        R8,
        DEPTH32,
    };
    class ITexture;
    class HB_API Texture final : public Resource {
        ITexture* instance;
        std::string path;
        int width = 1;
        int height = 1;
    public:
        Texture();
        ~Texture();

        void load(std::string path);

        virtual void setData(unsigned char *data, int width, int height, TEXTURE_FORMAT texture_type);

        virtual void bind(unsigned int slot = 0) const ;

        virtual void unbind(unsigned int slot = 0) const ;
    };

}