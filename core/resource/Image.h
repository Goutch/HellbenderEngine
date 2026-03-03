#pragma once

#include "Core.h"
#include <string>
#include "Handle.h"
#include "HBETypes.h"
#include "core/interface/ImageInterface.h"

namespace HBE {
    class HB_API Image {
        ImageHandle handle;
        const ImageInterface &image_interface;
    public:
        explicit Image(ImageHandle handle);
        ~Image() = default;
        vec3u getSize();
        void release();
        void update(const void* data);

        static int getFormatNumberOfChannels(IMAGE_FORMAT format);
        static Image *load(const std::string &path, ImageInfo &info);
    };
}
