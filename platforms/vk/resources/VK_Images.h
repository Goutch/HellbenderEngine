#pragma once
#include "HBETypes.h"
#include "VK_Image.h"
#include "data-structure/RawVector.h"
#include "data-structure/HandleProvider.h"

namespace HBE {
    class VK_Context;

    class VK_Images {
        VK_Context* context;
        HandleProvider handle_provider;
        std::vector<VK_Image> images_data;
        RawVector<ImageInfo> creation_infos;

    public:
        void init(VK_Context* context);
        void release();

        inline HBE_RESULT createImage(ImageHandle &handle, const ImageInfo &info);

        inline HBE_RESULT releaseImage(ImageHandle handle);

        inline HBE_RESULT getImageSize( ImageHandle handle, uvec3 &size_ref);

        inline HBE_RESULT updateImage(ImageHandle handle, const void *data);
    };
}
