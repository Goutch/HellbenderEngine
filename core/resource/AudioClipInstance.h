
#pragma once

#include "Core.h"
#include "Resource.h"
#include "AudioClip.h"

namespace HBE {
	struct AudioClipInstanceInfo {
		AudioClip *clip;
		float volume = 1;
		float pitch = 1;
		bool looping = false;
	};

	enum AUDIO_CLIP_INSTANCE_STATE {
		AUDIO_CLIP_INSTANCE_STATE_PLAYING,
		AUDIO_CLIP_INSTANCE_STATE_PAUSED,
		AUDIO_CLIP_INSTANCE_STATE_STOPPED
	};

	/// <summary>
	/// Can be played and that have one AUDIO_CLIP_INSTANCE_STATE:
	/// - AUDIO_CLIP_INSTANCE_STATE_PLAYING
	//	- AUDIO_CLIP_INSTANCE_STATE_PAUSED
	//	- AUDIO_CLIP_INSTANCE_STATE_STOPPED
	/// </summary>

	class HB_API AudioClipInstance  {
		friend class Resources;

		uint32_t source;
		AudioClip *audioClip;
		float volume;
		float pitch;
		bool is_looping;

		AudioClipInstance(AudioClipInstanceInfo info);

	public :
		~AudioClipInstance();

		void play();
		void pause();
		void stop();
		AUDIO_CLIP_INSTANCE_STATE getState();

		void resetPosition();



		void setVolume(float volume);

		float getVolume();

		void setPitch(float pitch);

		float getPitch();

		void setLooping(bool looping);

		bool isLooping();

		float getDuration();

		void updatePostion(vec3 position, vec3 listenerPosition);
	};
}


