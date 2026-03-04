#pragma once
#include "data-structure/StableHandleContainer.h"
namespace HBE {
    class VK_Context;
    class VK_Images {

        StableHandleContainer<VK_Image, 128> data;

    public:
        static ImageHandle createImage(VK_Context& context,const ImageInfo &info);
        static void releaseImage(VK_Context& context,ImageHandle handle);
        static uvec3 getImageSize(VK_Context& context,ImageHandle handle);
        static void updateImage(VK_Context& context,ImageHandle handle,);
    };
}
