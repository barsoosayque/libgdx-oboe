#pragma once

#include <oboe/Oboe.h>
#include <vector>
#include <atomic>

/// Oboe stream wrapper, with comfortable high level methods.
class oboe_engine : protected oboe::AudioStreamDataCallback, oboe::AudioStreamErrorCallback {
public:
    using on_async_write = std::function<const std::vector<int16_t>&(uint32_t)>;

    /// oboe_engine working mode.
    enum class mode {
        /// Allow using oboe_engine::blocking_reading
        reading,
        /// Allow using oboe_engine::blocking_writing
        writing,
        /// Allow using oboe_engine::set_on_async_write
        async_writing
    };
public:
    /// Opening audio stream with specified number of channels and sample rate
    oboe_engine(mode mode, uint8_t = 2, uint32_t = 44100);

    /// Closing audio stream
    ~oboe_engine();

    /// Request audio stream to start
    void resume();

    /// Request audio stream to stop streaming audio data
    void stop();

    /// Set callback to be used if mode is async_writing.
    void set_on_async_write(on_async_write&& callback) { m_on_async_write = std::move(callback); }

    /// Write 16bit PCM if mode is writing.
    void blocking_write(std::vector<int16_t> &&pcm);

    /// Read 16bit PCM if mode is reading.
    void blocking_read(std::vector<int16_t> &buffer);

    /// Return true if this stream has 1 channel.
    bool is_mono() const { return m_channels == 1; }

    /// Return number of channels of this stream.
    uint8_t channels() const { return m_channels; }

    /// Get size of buffer in samples.
    uint32_t payload_size() const;
private:
    // oboe::AudioStreamDataCallback interface
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *, void *, int32_t);
    // oboe::AudioStreamErrorCallback interface
    void onErrorAfterClose(oboe::AudioStream *, oboe::Result);

    void connect_to_device();

    std::unique_ptr<oboe::AudioStream> m_stream;
    mode m_mode;

    on_async_write m_on_async_write;

    uint8_t m_channels;
    uint32_t m_sample_rate;
    uint32_t m_payload_size;

    bool m_is_playing;

};