#pragma once
#include "HBETypes.h"
#include "StableVector.h"
#include "VK_Image.h"
#include "data-structure/RawVector.h"
#include "data-structure/HandleProvider.h"

namespace HBE {
    class VK_Context;

    class VK_Images {
        HandleProvider handle_provider;
        std::vector<VK_Image> images_data;
        RawVector<ImageInfo> creation_infos;

    public:
        void release();

        static inline HBE_RESULT createImage(void *context_ptr, ImageHandle &handle, const ImageInfo &info);

        static inline HBE_RESULT releaseImage(void *context_ptr, ImageHandle handle);

        static inline HBE_RESULT getImageSize(void *context_ptr, ImageHandle handle, uvec3 &size_ref);

        static inline HBE_RESULT updateImage(void *context_ptr, ImageHandle handle, const void *data);
    };
}
