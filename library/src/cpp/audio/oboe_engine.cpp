#include "oboe_engine.hpp"
#include "../utility/ptrptr.hpp"
#include "../utility/log.hpp"
#include "../utility/exception.hpp"
#include <array>
#include <algorithm>
#include <iterator>
#include <limits>
#include <cassert>

namespace {
/// @note: message should contain {}
inline bool check(oboe::Result result, std::string_view msg) {
    if (result != oboe::Result::OK) {
        warn(msg, oboe::convertToText(result));
        return false;
    }
    return true;
}
}

oboe_engine::oboe_engine(mode mode, uint8_t channels, uint32_t sample_rate)
        : oboe::AudioStreamDataCallback()
        , oboe::AudioStreamErrorCallback()
        , m_mode(mode)
        , m_channels(channels)
        , m_sample_rate(sample_rate)
        , m_payload_size(0)
        , m_is_playing(false) {
    connect_to_device();
}

oboe_engine::~oboe_engine() {
    if (!m_stream)
        return;

    stop();
    check(m_stream->close(), "Error closing stream: {}");
}

void oboe_engine::connect_to_device() {
    // initialize Oboe audio stream
    oboe::AudioStreamBuilder builder;
    builder.setChannelCount(m_channels);
    builder.setSampleRate(static_cast<int32_t>(m_sample_rate));
    if (m_mode == mode::async_writing)
        builder.setDataCallback(this);
    builder.setErrorCallback(this);
    builder.setFormat(oboe::AudioFormat::I16);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);

    check(builder.openStream(ptrptr(m_stream)), "Error opening stream: {}");

    m_payload_size = m_stream->getFramesPerBurst() * 2;
    m_stream->setBufferSizeInFrames(static_cast<int32_t>(m_payload_size));
}

void oboe_engine::onErrorAfterClose(oboe::AudioStream *self, oboe::Result error) {
    if (error == oboe::Result::ErrorDisconnected) {
        info("Previous device disconnected. Trying to connect to a new one...");
        connect_to_device();
        if (m_is_playing) {
            resume();
        }
    }
}

oboe::DataCallbackResult oboe_engine::onAudioReady(oboe::AudioStream *self, void *audio_data,
                                                   int32_t num_frames) {
    android_assert(m_mode == mode::async_writing,
                   "engine not in async_writing mode, something went wrong.");

    if (num_frames > 0 && m_on_async_write) {
        auto& pcm_queue = m_on_async_write(static_cast<uint32_t>(num_frames * m_channels));
        auto stream = static_cast<int16_t*>(audio_data);
        int32_t write_size = std::min(static_cast<int32_t>(pcm_queue.size()), num_frames * m_channels);

        if (write_size != 0) {
            std::copy(pcm_queue.begin(),
                      std::next(pcm_queue.begin(), write_size),
                      stream);
        }

        if (write_size < num_frames) {
            std::fill(std::next(stream, write_size),
                      std::next(stream, num_frames * m_channels),
                      0);
        }
    }

    return oboe::DataCallbackResult::Continue;
}

void oboe_engine::resume() {
    if (!m_stream)
        return;

    debug("oboe_engine::resume. State: {}", oboe::convertToText(m_stream->getState()));

    if (check(m_stream->requestStart(), "Error starting stream: {}")) {
        m_is_playing = true;
    }
}

void oboe_engine::stop() {
    if (!m_stream)
        return;

    debug("stop::resume. State: {}", oboe::convertToText(m_stream->getState()));

    if (check(m_stream->requestStop(), "Error stopping stream: {}")) {
        m_is_playing = false;
    }
}

void oboe_engine::blocking_write(std::vector<int16_t> &&pcm) {
    android_assert(m_mode == mode::writing,
                   "engine not in writing mode, something went wrong.");

    if ( m_stream ) {
        m_stream->write(pcm.data(), static_cast<int32_t>(pcm.size() / m_channels),
                        std::numeric_limits<int64_t>::max());
    }
}

void oboe_engine::blocking_read(std::vector<int16_t> &buffer) {
    android_assert(m_mode == mode::reading,
                   "engine not in reading mode, something went wrong.");

    if ( m_stream ) {
        m_stream->read(buffer.data(), static_cast<int32_t>(buffer.size()),
                       std::numeric_limits<int64_t>::max());
    }
}

uint32_t oboe_engine::payload_size() const {
    return m_payload_size * m_channels;
}