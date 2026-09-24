#pragma once

#include "Core.h"
#include "vector"
#include "string"
struct ALCdevice;
struct ALCcontext;

namespace HBE
{
    class AudioClip;

    class HB_API Audio
    {
        ALCdevice* device = nullptr;
        ALCcontext* audio_context = nullptr;
        std::vector<std::string> devices;

    public:
        Audio() = default;

        void init();

        void release();

        const std::vector<std::string>& getAudioDevices();
    };
}
