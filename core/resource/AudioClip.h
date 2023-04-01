
#pragma once

#include "Core.h"
#include "Resource.h"
#include "string"

namespace HBE {
	struct AudioClipInfo {
		std::string path;
	};

	class HB_API AudioClip : public Resource {
		friend class Resources;

		friend class AudioClipInstance;

		uint32_t buffer;
	private:
		AudioClip(AudioClipInfo info);

	public:
		~AudioClip() override;
	};
}