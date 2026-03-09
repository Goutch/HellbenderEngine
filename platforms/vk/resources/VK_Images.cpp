#include "VK_Images.h"
#include "VK_Image.h"
#include "../VK_Context.h"
#include "core/interface/ImageInterface.h"

#define IMAGE_FUNCTION_HEADER \
VK_Context *context = static_cast<VK_Context *>(context_ptr);   \
VK_Images &images = context->images;

namespace HBE
{
    void VK_Images::init(VK_Context* context)
    {
        this->context = context;
    }

    void VK_Images::release()
    {
        for (VK_Image& image : images_data)
        {
            if (image.allocated())
            {
                image.release();
                Log::warning("An Image was not released properly");
            }
        }
    }

    HBE_RESULT VK_Images::createImage(ImageHandle& handle, const ImageInfo& info)
    {
        handle = handle_provider.create();
        images_data.resize(handle_provider.size());
        images_data[handle_provider.index(handle)].alloc(context, info);
        return HBE_RESULT_SUCCESS;
    }

    HBE_RESULT VK_Images::releaseImage(ImageHandle handle)
    {
        if (!handle_provider.valid(handle)) return HBE_RESULT_INVALID_HANDLE;
        images_data[handle_provider.index(handle)].release();
        return HBE_RESULT_SUCCESS;
    }

    HBE_RESULT VK_Images::getImageSize(ImageHandle handle, uvec3& size_ref)
    {
        if (!handle_provider.valid(handle)) return HBE_RESULT_INVALID_HANDLE;
        size_ref = images_data[handle_provider.index(handle)].getSize();
        return HBE_RESULT_SUCCESS;
    }

    HBE_RESULT VK_Images::updateImage(ImageHandle handle, const void* data)
    {
        if (!handle_provider.valid(handle)) return HBE_RESULT_INVALID_HANDLE;
        images_data[handle_provider.index(handle)].update(data);
        return HBE_RESULT_SUCCESS;
    }
}
