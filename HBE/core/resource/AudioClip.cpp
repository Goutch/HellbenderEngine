//
// Created by User on 2/19/2023.
//

#include "AudioClip.h"
#include "AL/al.h"
#include "sndfile.h"
#include "core/utility/Log.h"

namespace HBE {
	AudioClip::AudioClip(AudioClipInfo info) {

		std::string path = RESOURCE_PATH + info.path;
		// Open the audio file with libsndfile
		SF_INFO sfInfo{};
		SNDFILE *file = sf_open(path.c_str(), SFM_READ, &sfInfo);
		if (!file) {
			printf("Failed to open file %s\n", path.c_str());
		}


		// Set the audio format for OpenAL
		ALenum format = 0;
		switch (sfInfo.channels) {
			case 1:
				format = AL_FORMAT_MONO16;
				break;
			case 2:
				format = AL_FORMAT_STEREO16;
				break;
			default:
				Log::error("Unsupported audio format:" + std::to_string(sfInfo.channels) + " channels");
				sf_close(file);
		}

		// Allocate memory to store the audio data
		int frames = sfInfo.frames;
		int numSamples = frames * sfInfo.channels;
		short *data = (short *) malloc(numSamples * sizeof(short));

		// Read the audio data into memory
		int count = sf_read_short(file, data, numSamples);
		if (count != numSamples) {
			Log::error("Failed to read data from file " + info.path);
			free(data);
			sf_close(file);
		}

		// Create an OpenAL buffer and fill it with the audio data

		alGenBuffers(1, &buffer);
		alBufferData(buffer, format, data, numSamples * sizeof(short), sfInfo.samplerate);
		free(data);
		sf_close(file);
	}

	AudioClip::~AudioClip() {
		// Cleanup and exit
		alDeleteBuffers(1, &buffer);
	}
}

