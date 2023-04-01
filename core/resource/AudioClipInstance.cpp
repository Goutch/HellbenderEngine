//
// Created by User on 2/24/2023.
//

#include "AudioClipInstance.h"
#include "al.h"

namespace HBE {

	AudioClipInstance::AudioClipInstance(AudioClipInstanceInfo info) {
		this->audioClip = info.clip;

		alGenSources(1, &source);
		alSourcei(source, AL_BUFFER, info.clip->buffer);
		setVolume(info.volume);
		setPitch(info.pitch);
		setLooping(info.looping);
	}

	AudioClipInstance::~AudioClipInstance() {
		stop();
		alDeleteSources(1, &source);
	}

	void AudioClipInstance::stop() {
		alSourceStop(source);
	}

	void AudioClipInstance::play() {
		alSourcePlay(source);
	}

	AUDIO_CLIP_INSTANCE_STATE AudioClipInstance::getState() {
		ALint state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);
		switch (state) {
			case AL_PLAYING:
				return AUDIO_CLIP_INSTANCE_STATE_PLAYING;
			case AL_PAUSED:
				return AUDIO_CLIP_INSTANCE_STATE_PAUSED;
			case AL_STOPPED:
			case AL_INITIAL:
			default:
				return AUDIO_CLIP_INSTANCE_STATE_STOPPED;
		}
	}

	void AudioClipInstance::resetPosition() {
		alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
	}

	void AudioClipInstance::updatePostion(vec3 position, vec3 listenerPosition) {
		vec3 p = position - listenerPosition;
		alSourcefv(source, AL_POSITION, &p[0]);
	}

	void AudioClipInstance::pause() {
		alSourcePause(source);

	}

	void AudioClipInstance::setVolume(float volume) {
		this->volume = volume;
		alSourcef(source, AL_GAIN, volume);
	}

	float AudioClipInstance::getVolume() {
		return volume;
	}

	float AudioClipInstance::getPitch() {
		return pitch;
	}

	void AudioClipInstance::setPitch(float pitch) {
		this->pitch = pitch;
		alSourcef(source, AL_PITCH, pitch);
	}

	void AudioClipInstance::setLooping(bool looping) {
		is_looping = looping;
		alSourcei(source, AL_LOOPING, looping);
	}

	bool AudioClipInstance::isLooping() {
		return is_looping;
	}

	float AudioClipInstance::getDuration() {
		ALint sample_rate, num_samples;
		alGetBufferi(audioClip->buffer, AL_FREQUENCY, &sample_rate);
		alGetBufferi(audioClip->buffer, AL_SIZE, &num_samples);
		float duration = (num_samples) / static_cast<float>(sample_rate);
		duration *= pitch;
		return duration;
	}


}

