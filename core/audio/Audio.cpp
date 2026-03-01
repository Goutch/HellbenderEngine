//
// Created by User on 2/19/2023.
//

#include "Audio.h"
#define AL_LIBTYPE_STATIC
#include "AL/alc.h"
#include "sndfile.h"
#include "core/utility/Log.h"
#include "string.h"
namespace HBE {


	void Audio::init() {

		device = alcOpenDevice(nullptr);
		if (device == nullptr) {
			Log::error("Failed to initialize OpenAL audio device");
		}
		audio_context = alcCreateContext(device, nullptr);
		alcMakeContextCurrent(audio_context);
		Log::status("Using audio device: " + getAudioDevices().front());
	}

	const std::vector<std::string> &Audio::getAudioDevices() {
		devices.clear();
		const ALCchar *device_names = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
		Log::status("Available audio devices:");
		while (*device_names != '\0') {
			devices.emplace_back(device_names+15);
			Log::status(" - " + devices.back());
			device_names += strlen(device_names) + 1;
		}

		return devices;
	}

	void Audio::release() {
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(audio_context);
		alcCloseDevice(device);
	}

}

