#pragma once

#include "Core.h"
#include "vector"
#include "string"
struct ALCdevice;
struct ALCcontext;
namespace HBE {
	class AudioClip;
	class HB_API Audio {
		static ALCdevice *device;
		static ALCcontext *context;
		static std::vector<std::string> devices;
	public:
		static void init();

		static void terminate();

		static const std::vector<std::string> &getAudioDevices();
	};
}
