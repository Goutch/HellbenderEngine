#pragma once

#include <string>
#include "core/serialization/Serializable.h"
#include "Core.h"
namespace HBE {
    class HB_API Texture : Serializable {
    protected:
        std::string path;
        int width = 1;
        int height = 1;
    public:
        enum TEXTURE_TYPE {
            RGB8,
            RGBA8,
            R8,
            DEPTH32,
        };

        static void load(std::string path, unsigned char *buffer, int &width, int &height);

        static Texture *create();

        Texture *load(std::string path);

        virtual unsigned int getTextureID() const = 0;

        virtual void setData(unsigned char *data, int width, int height, TEXTURE_TYPE texture_type) = 0;

        virtual ~Texture() {};

        virtual void bind(unsigned int slot = 0) const = 0;

        virtual void unbind(unsigned int slot = 0) const = 0;

        void serialize(Serializer *serializer) const override;

        void deserialize(Deserializer *deserializer) override;

    };

}