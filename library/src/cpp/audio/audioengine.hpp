#pragma once
#include <oboe/Oboe.h>
#include "mixer.hpp"
#include "../sound/soundpool.hpp"
#include "../music/music.hpp"
#include <vector>
#include <deque>
#include <atomic>

class audio_engine : protected oboe::AudioStreamCallback {
    private:
        void connect_to_device();
        enum class mode { mix, stream };

        std::unique_ptr<oboe::AudioStream> m_stream;
        std::unique_ptr<mixer> m_mixer;
        int8_t m_channels;
        int32_t m_sample_rate;
        int32_t m_payload_size;
        std::deque<int16_t> m_pcm_buffer;
        float m_volume;
        mode m_mode;
        std::atomic_flag m_rendering_flag;
        bool m_is_playing;

        oboe::DataCallbackResult onAudioReady(oboe::AudioStream*, void*, int32_t);
        void onErrorAfterClose(oboe::AudioStream*, oboe::Result);
    public:
        /// Opening audio stream with specified number of channels and sample rate
        audio_engine(int8_t = 2, int32_t = 44100);

        /// Closing audio stream
        ~audio_engine();

        /// Request audio stream to start
        void resume();

        /// Request audio stream to stop streaming audio data
        void stop();

        /// Play sound or music. Automatically set mode to mix
        void play(std::shared_ptr<renderable_audio> p_audio);

        /// Queue pcm for streaming. Automatically set mode to stream
        void play(const std::vector<int16_t>& p_pcm);

        /// Queue pcm for streaming. Automatically set mode to stream
        void play(const std::vector<float>& p_pcm);

        /// Return if this stream has 1 channel
        bool is_mono();

        /// Set volume of the mixer
        void volume(float);

        /// Get size of buffer in samples
        int32_t payload_size();
};