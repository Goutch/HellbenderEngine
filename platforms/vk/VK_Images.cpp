#include "VK_Images.h"
#include "VK_Image.h"
#include "VK_Context.h"
#include "core/interface/ImageInterface.h"

#define IMAGE_FUNCTION_HEADER \
VK_Context *context = static_cast<VK_Context *>(context_ptr);   \
VK_Images &images = context->images;                            \
if (!images.handle_provider.valid(handle)) return HBE_RESULT_INVALID_HANDLE;

namespace HBE {
    HBE_RESULT VK_Images::createImage(void *context_ptr, ImageHandle &handle, const ImageInfo &info) {
        IMAGE_FUNCTION_HEADER
        handle = images.handle_provider.create();
        images.images_data.resize(images.handle_provider.size());
        images.images_data[images.handle_provider.index(handle)].alloc(context,info);
        return HBE_RESULT_SUCCESS;
    }

    HBE_RESULT VK_Images::releaseImage(void *context_ptr, ImageHandle handle) {
        IMAGE_FUNCTION_HEADER
        images.images_data[images.handle_provider.index(handle)].release();
        return HBE_RESULT_SUCCESS;
    }

    HBE_RESULT VK_Images::getImageSize(void *context_ptr, ImageHandle handle,uvec3& size_ref) {
        IMAGE_FUNCTION_HEADER
        size_ref = images.images_data[images.handle_provider.index(handle)].getSize();
        return HBE_RESULT_SUCCESS;
    }

    HBE_RESULT VK_Images::updateImage(void *context_ptr, ImageHandle handle, const void *data) {
        IMAGE_FUNCTION_HEADER
        images.images_data[images.handle_provider.index(handle)].update(data);
        return HBE_RESULT_SUCCESS;
    }
}
