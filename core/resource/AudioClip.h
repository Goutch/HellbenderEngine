
#pragma once

#include "Core.h"
#include "Resource.h"
#include "string"

namespace HBE {
	struct AudioClipInfo {
		std::string path;
	};

	class HB_API AudioClip  {
		friend class AudioClipInstance;

		uint32_t buffer;
	public:
		AudioClip() = default;
		void alloc(const AudioClipInfo& info);
		void release();
		~AudioClip();
	};
}