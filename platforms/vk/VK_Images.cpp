#include "VK_Images.h"

#include "VK_Context.h"

namespace HBE {
    ImageHandle VK_Images::createImage(VK_Context& context,const ImageInfo &info) {
        VK_Images& images = context.images;
        ImageHandle image = images.data.create();

        return image;
    }

    void VK_Images::releaseImage(VK_Context &context, const ImageHandle &handle) {

    }
}
