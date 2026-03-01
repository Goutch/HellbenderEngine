#pragma once

#include "Core.h"
#include <string>
#include "Resource.h"

namespace HBE {
	enum IMAGE_FORMAT : int {
		IMAGE_FORMAT_UNDEFINED = -1,
		IMAGE_FORMAT_R8 = 0,
		IMAGE_FORMAT_RG8 = 1,
		IMAGE_FORMAT_RGB8 = 2,
		IMAGE_FORMAT_RGBA8 = 3,
		IMAGE_FORMAT_R32F = 4,
		IMAGE_FORMAT_RG32F = 5,
		IMAGE_FORMAT_RGB32F = 6,
		IMAGE_FORMAT_RGBA32F = 7,
		IMAGE_FORMAT_DEPTH32F = 9,
		IMAGE_FORMAT_DEPTH32f_STENCIL8U = 10,
		IMAGE_FORMAT_DEPTH24f_STENCIL8U = 11,
		IMAGE_FORMAT_SBGRA8_NON_LINEAR = 12,
		IMAGE_FORMAT_SR8_NON_LINEAR = 13,
		IMAGE_FORMAT_SRG8_NON_LINEAR = 14,
		IMAGE_FORMAT_SRGB8_NON_LINEAR = 15,
		IMAGE_FORMAT_SRGBA8_NON_LINEAR = 16,
		IMAGE_FORMAT_R8_UINT = 17,
		IMAGE_FORMAT_RG8_UINT = 18,
		IMAGE_FORMAT_RGB8_UINT = 19,
		IMAGE_FORMAT_RGBA8_UINT = 20,
		IMAGE_FORMAT_R32_UINT = 21,
		IMAGE_FORMAT_RG32_UINT = 22,
		IMAGE_FORMAT_RGB32_UINT = 23,
		IMAGE_FORMAT_RGBA32_UINT = 24,
		IMAGE_FORMAT_R8_UNORM = 25,
		IMAGE_FORMAT_RG8_UNORM = 26,
		IMAGE_FORMAT_RGB8_UNORM = 27,
		IMAGE_FORMAT_RGBA8_UNORM = 28,
		IMAGE_FORMAT_R8_SNORM = 29,
		IMAGE_FORMAT_RG8_SNORM = 30,
		IMAGE_FORMAT_RGB8_SNORM = 31,
		IMAGE_FORMAT_RGBA8_SNORM = 32,
		IMAGE_FORMAT_R16F = 33,
		IMAGE_FORMAT_RG16F = 34,
		IMAGE_FORMAT_RGB16F = 35,
		IMAGE_FORMAT_RGBA16F = 36,
		IMAGE_FORMAT_R16_UINT = 37,
		IMAGE_FORMAT_RG16_UINT = 38,
		IMAGE_FORMAT_RGB16_UINT = 39,
		IMAGE_FORMAT_RGBA16_UINT = 40,
		IMAGE_FORMAT_R16_UNORM = 41,
		IMAGE_FORMAT_RG16_UNORM = 42,
		IMAGE_FORMAT_RGB16_UNORM = 43,
		IMAGE_FORMAT_RGBA16_UNORM = 44,
		IMAGE_FORMAT_R16_SNORM = 45,
		IMAGE_FORMAT_RG16_SNORM = 46,
		IMAGE_FORMAT_RGB16_SNORM = 47,
		IMAGE_FORMAT_RGBA16_SNORM = 48,
	};

	typedef uint32_t IMAGE_SAMPLER_FLAGS;

	enum IMAGE_SAMPLER_FLAG {
		IMAGE_SAMPLER_FLAG_NONE = 0,
	};

	enum IMAGE_SAMPLER_FILTER_TYPE {
		IMAGE_SAMPLER_FILTER_TYPE_NEAREST = 0,
		IMAGE_SAMPLER_FILTER_TYPE_LINEAR = 1,
	};

	enum IMAGE_SAMPLER_ADDRESS_MODE {
		IMAGE_SAMPLER_ADDRESS_MODE_REPEAT = 0,
		IMAGE_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT = 1,
		IMAGE_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE = 2,
		IMAGE_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER = 3,
		IMAGE_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE = 4,
	};

	struct ImageSamplerInfo {
		IMAGE_SAMPLER_FLAGS flags = IMAGE_SAMPLER_FLAG_NONE;
		IMAGE_SAMPLER_FILTER_TYPE filter = IMAGE_SAMPLER_FILTER_TYPE_LINEAR;
		IMAGE_SAMPLER_ADDRESS_MODE address_mode = IMAGE_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	};

	typedef uint32_t IMAGE_FLAGS;

	enum IMAGE_FLAG {
		IMAGE_FLAG_NONE = 0,
		IMAGE_FLAG_SHADER_WRITE = 1 << 0,
		IMAGE_FLAG_RENDER_TARGET = 1 << 1,
		IMAGE_FLAG_DEPTH = 1 << 2,
		IMAGE_FLAG_NO_SAMPLER = 1 << 3, // image2D in glsl accessed with imageLoad and imageStore.
	};

	/**
	 * @param data can be nullptr
	 * @param width must be greater than 0
	 * @param height must be greater than 0
	 * @param depth must be greater than 0
	 * @param min_lod must be greater than 0
	 */
	struct ImageInfo {
		const void *data = nullptr;
		uint32_t width = 1;
		uint32_t height = 1;
		uint32_t depth = 1;

		bool generate_mip_maps = false;
		IMAGE_FORMAT format = IMAGE_FORMAT_RGBA32F;
		IMAGE_FORMAT data_format = IMAGE_FORMAT_UNDEFINED;
		IMAGE_FLAGS flags = IMAGE_FLAG_NONE;
		ImageSamplerInfo sampler_info = {};
	};

	struct ImageRegionUpdateInfo {
		uint32_t data_texel_offset;
		vec3i offset;
		vec3u size;
	};

	class HB_API Image  {
	public:
		virtual ~Image() = default;

		virtual uint32_t getWidth() const = 0;

		virtual uint32_t getHeight() const = 0;

		virtual uint32_t getDepth() const = 0;

		virtual vec3u getSize() const = 0;

		virtual void update(const void *data, IMAGE_FORMAT format) = 0;

		virtual void updateRegion(const void *data, uint32_t data_texel_count, ImageRegionUpdateInfo *update_infos,uint32_t update_count) = 0;

		static int getFormatNumberOfChannels(IMAGE_FORMAT format);

		static Image *load(const std::string &path, ImageInfo& info);
	};
}