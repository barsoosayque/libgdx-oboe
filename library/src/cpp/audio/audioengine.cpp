#include "audioengine.hpp"
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

audio_engine::audio_engine(mode mode, int8_t channels, int32_t sample_rate)
        : oboe::AudioStreamDataCallback()
        , oboe::AudioStreamErrorCallback()
        , m_mixer(std::make_unique<mixer>(1024, channels))
        , m_channels(channels)
        , m_sample_rate(sample_rate)
        , m_payload_size(0)
        , m_volume(1)
        , m_rendering_flag(false)
        , m_is_playing(false)
        , m_mode(mode) {
    connect_to_device();
}

audio_engine::~audio_engine() {
    stop();
    check(m_stream->close(), "Error closing stream: {}");
}

void audio_engine::connect_to_device() {
    // initialize Oboe audio stream
    oboe::AudioStreamBuilder builder;
    builder.setChannelCount(m_channels);
    builder.setSampleRate(m_sample_rate);
    if (m_mode == mode::async) {
        builder.setDataCallback(this);
        builder.setErrorCallback(this);
    }
    builder.setFormat(oboe::AudioFormat::I16);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);

    check(builder.openStream(ptrptr(m_stream)), "Error opening stream: {}");

    m_payload_size = m_stream->getFramesPerBurst() * 2;
    m_stream->setBufferSizeInFrames(m_payload_size);
    m_mixer->resize_buffer(m_payload_size * m_channels);
}

void audio_engine::onErrorAfterClose(oboe::AudioStream *self, oboe::Result error) {
    if (error == oboe::Result::ErrorDisconnected) {
        info("Previous device disconnected. Trying to connect to a new one...");
        connect_to_device();
        if (m_is_playing) {
            resume();
        }
    }
}

oboe::DataCallbackResult audio_engine::onAudioReady(oboe::AudioStream *self, void *audio_data,
                                                    int32_t num_frames) {
    while (m_rendering_flag.test_and_set(std::memory_order_acquire));
    auto stream = static_cast<int16_t *>(audio_data);
    m_mixer->render(stream, num_frames);
    m_rendering_flag.clear(std::memory_order_release);

    return oboe::DataCallbackResult::Continue;
}

void audio_engine::resume() {
    debug("audio_engine::resume. State: {}", oboe::convertToText(m_stream->getState()));
    if (check(m_stream->requestStart(), "Error starting stream: {}")) {
        m_is_playing = true;
    }
}

void audio_engine::stop() {
    debug("stop::resume. State: {}", oboe::convertToText(m_stream->getState()));
    if (check(m_stream->requestStop(), "Error stopping stream: {}")) {
        m_is_playing = false;
    }
}

void audio_engine::play(const std::shared_ptr<renderable_audio> &audio) {
    android_assert(m_mode == mode::async,
                   "playing sounds and music in blocking mode is not implemented.");
    m_mixer->play_audio(audio);
}

void audio_engine::play(const std::vector<int16_t> &pcm) {
    android_assert(m_mode == mode::blocking, "playing raw pcm in async mode is not implemented.");
    m_stream->write(pcm.data(), pcm.size() / m_channels, std::numeric_limits<int64_t>::max());
}

void audio_engine::play(const std::vector<float> &pcm) {
    android_assert(m_mode == mode::blocking, "playing raw pcm in async mode is not implemented.");
    m_pcm_buffer.clear();
    std::transform(pcm.cbegin(), pcm.cend(), std::back_inserter(m_pcm_buffer),
                   [](const float &sample) {
                       auto converted = sample * std::numeric_limits<int16_t>::max();
                       return static_cast<int16_t>(converted);
                   });
    m_stream->write(m_pcm_buffer.data(), m_pcm_buffer.size() / m_channels,
                    std::numeric_limits<int64_t>::max());
}

bool audio_engine::is_mono() const {
    return m_channels == 1;
}

void audio_engine::volume(float volume) {
    m_volume = std::max(std::min(volume, 1.0f), 0.0f);
    m_mixer->m_volume = m_volume;
}

int32_t audio_engine::payload_size() const {
    return m_payload_size * m_channels;
}