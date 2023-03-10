//
// Created by User on 2/19/2023.
//

#include "Audio.h"
#define AL_LIBTYPE_STATIC
#include "AL/al.h"
#include "AL/alc.h"
#include "sndfile.h"
#include "core/utility/Log.h"

namespace HBE {
	ALCdevice *Audio::device = nullptr;
	ALCcontext *Audio::context = nullptr;
	std::vector<std::string> Audio::devices;

	void Audio::init() {

		device = alcOpenDevice(nullptr);
		if (device == nullptr) {
			Log::error("Failed to initialize OpenAL audio device");
		}
		context = alcCreateContext(device, nullptr);
		alcMakeContextCurrent(context);
		Log::status("Using audio device: " + getAudioDevices().front());
	}

	const std::vector<std::string> &Audio::getAudioDevices() {
		devices.clear();
		const ALCchar *device_names = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
		Log::status("Available audio devices:");
		while (*device_names != '\0') {
			devices.emplace_back(device_names+15);
			Log::status(" - " + devices.back());
			device_names += std::strlen(device_names) + 1;
		}

		return devices;
	}

	void Audio::terminate() {
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context);
		alcCloseDevice(device);
	}

}

