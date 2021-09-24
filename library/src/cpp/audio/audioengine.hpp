#pragma once

#include <oboe/Oboe.h>
#include "mixer.hpp"
#include "../sound/soundpool.hpp"
#include "../music/music.hpp"
#include <vector>
#include <atomic>

class audio_engine : protected oboe::AudioStreamDataCallback, oboe::AudioStreamErrorCallback {
public:
    enum class mode { blocking, async };
private:
    void connect_to_device();

    std::unique_ptr<oboe::AudioStream> m_stream;
    std::unique_ptr<mixer> m_mixer;
    int8_t m_channels;
    int32_t m_sample_rate;
    int32_t m_payload_size;
    std::vector<int16_t> m_pcm_buffer;
    float m_volume;
    std::atomic_flag m_rendering_flag;
    bool m_is_playing;
    mode m_mode;

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *, void *, int32_t);
    void onErrorAfterClose(oboe::AudioStream *, oboe::Result);
public:

    /// Opening audio stream with specified number of channels and sample rate
    audio_engine(mode mode, int8_t = 2, int32_t = 44100);

    /// Closing audio stream
    ~audio_engine();

    /// Request audio stream to start
    void resume();

    /// Request audio stream to stop streaming audio data
    void stop();

    /// Play sound or music. Automatically set mode to mix
    void play(const std::shared_ptr<renderable_audio> &audio);

    /// Queue pcm for streaming. Automatically set mode to stream
    void play(const std::vector<int16_t> &pcm);

    /// Queue pcm for streaming. Automatically set mode to stream
    void play(const std::vector<float> &pcm);

    /// Return if this stream has 1 channel
    bool is_mono() const;

    /// Set volume of the mixer
    void volume(float);

    /// Get size of buffer in samples
    int32_t payload_size() const;
};