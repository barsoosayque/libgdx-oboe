#pragma once
#include <oboe/Oboe.h>
#include "mixer.hpp"
#include "../sound/soundpool.hpp"
#include "../music/music.hpp"

class audio_engine : protected oboe::AudioStreamCallback {
    private:
        std::unique_ptr<oboe::AudioStream> m_stream;
        std::unique_ptr<mixer> m_mixer;
        int8_t m_channels;

        oboe::DataCallbackResult onAudioReady(oboe::AudioStream*, void*, int32_t);
    public:
        /// Opening audio stream with specified number of channels
        audio_engine(int8_t);

        /// Closing audio stream
        ~audio_engine();

        /// Request audio stream to start
        void resume();

        /// Request audio stream to stop streaming audio data
        void stop();

        void play(renderable_audio* p_audio);
};