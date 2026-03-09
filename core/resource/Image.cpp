#include "Image.h"
#include "fstream"
#include "core/utility/Log.h"
#include "core/Application.h"
#include "core/Graphics.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
namespace HBE
{
    Image::Image(const ImageInfo& info) : context(*Application::instance->getContext())
    {
        alloc(info);
    }

    Image::Image(): context(*Application::instance->getContext())
    {
    }

    vec3u Image::getSize()
    {
        HB_ASSERT(handle != HBE_NULL_HANDLE,"allocate the image before requesting the size");
        vec3u size;
        context.getImageSize(handle, size);
        return size;
    }

    void Image::release()
    {
        context.releaseImage(handle);
        handle = HBE_NULL_HANDLE;
    }

    void Image::update(const void* data)
    {
        HB_ASSERT(handle != HBE_NULL_HANDLE,"allocate the image before updating it");
        context.updateImage(handle, data);
    }

    int Image::getFormatNumberOfChannels(IMAGE_FORMAT format)
    {
        switch (format)
        {
        case IMAGE_FORMAT_R32F:
        case IMAGE_FORMAT_R8:
            return 1;
        case IMAGE_FORMAT_RG32F:
        case IMAGE_FORMAT_RG8:
            return 2;
        case IMAGE_FORMAT_RGB32F:
        case IMAGE_FORMAT_RGB8:
            return 3;
        case IMAGE_FORMAT_RGBA32F:
        case IMAGE_FORMAT_RGBA8:
            return 4;
        default:
            return 4;
        }
    }


    void Image::load(const char* path, IMAGE_FORMAT expected_format)
    {
        std::string path_str = RESOURCE_PATH + std::string(path);

        FILE* file = fopen(path_str.c_str(), "rb");

        if (file == nullptr)
        {
            Log::error("Failed to open file: " + path_str);
        }
        int32_t width, height;
        unsigned char* buffer;

        stbi_set_flip_vertically_on_load(true);
        int nb_channels;
        int expected_channels = getFormatNumberOfChannels(expected_format);
        buffer = stbi_load_from_file(file, &width, &height, &nb_channels, expected_channels);

        if (nb_channels != expected_channels)
        {
            Log::error(
                "Texture format missmatch. Expected " + std::to_string(expected_channels) + " channels, got " + std::
                to_string(nb_channels));
        }
        if (buffer == nullptr)
        {
            Log::error("Failed to load texture: " + path_str);
        }
        ImageInfo info{};
        info.width = width;
        info.height = height;
        info.optional_data = buffer;
        info.data_format = expected_format;
        alloc(info);
        stbi_image_free(buffer);
        stbi_set_flip_vertically_on_load(true);
        fclose(file);
        info.optional_data = nullptr;
    }

    Image::Image(Image&& other)  noexcept:context(other.context)
    {
        handle = other.handle;

        other.handle = HBE_NULL_HANDLE;
    }

    Image::~Image()
    {
        if (handle != HBE_NULL_HANDLE)
            context.releaseImage(handle);
    }

    void Image::alloc(const ImageInfo& info)
    {
        HB_ASSERT(handle == HBE_NULL_HANDLE,"trying to allocate an image that is already allocated, this will cause a memory leak");
        context.createImage(handle, info);
    }
}
