#include "assert.h"
#include "audioengine.hpp"
#include "../utility/ptrptr.hpp"
#include "../utility/log.hpp"
#include "../utility/oboeutils.hpp"
#include <array>
#include <algorithm>
#include <iterator>
#include <limits>

using namespace oboe;

audio_engine::audio_engine(mode p_mode, int8_t p_channels, int32_t p_sample_rate)
    : AudioStreamCallback()
    , m_mixer(std::make_unique<mixer>(1024, p_channels))
    , m_channels(p_channels)
    , m_sample_rate(p_sample_rate)
    , m_volume(1)
    , m_rendering_flag(false)
    , m_is_playing(false)
    , m_mode(p_mode) {
    connect_to_device();
}

audio_engine::~audio_engine() {
    stop();
    check(m_stream->close(), "Error closing stream: {}");
}

void audio_engine::connect_to_device() {
    // initialize Oboe audio stream
    AudioStreamBuilder builder;
    builder.setChannelCount(m_channels);
    builder.setSampleRate(m_sample_rate);
    if(m_mode == mode::async) {
        builder.setCallback(this);
    }
    builder.setFormat(AudioFormat::I16);
    builder.setPerformanceMode(PerformanceMode::LowLatency);
    builder.setSharingMode(SharingMode::Exclusive);

    check(builder.openStream(ptrptr(m_stream)), "Error opening stream: {}");

    m_payload_size = m_stream->getFramesPerBurst() * 2;
    m_stream->setBufferSizeInFrames(m_payload_size);
    m_mixer->resize_buffer(m_payload_size * m_channels);
}

void audio_engine::onErrorAfterClose(AudioStream* self, Result p_error) {
    if (p_error == Result::ErrorDisconnected) {
        info("Previous device disconnected. Trying to connect to a new one...");
        connect_to_device();
        if(m_is_playing) {
            resume();
        }
    }
}

DataCallbackResult audio_engine::onAudioReady(AudioStream* self, void* p_audio_data, int32_t p_num_frames) {
    while(m_rendering_flag.test_and_set(std::memory_order_acquire));
    auto stream = static_cast<int16_t*>(p_audio_data);
    m_mixer->render(stream, p_num_frames);
    m_rendering_flag.clear(std::memory_order_release);

    return DataCallbackResult::Continue;
}

void audio_engine::resume() {
    if(check(m_stream->requestStart(), "Error starting stream: {}")) {
        m_is_playing = true;
    }
}

void audio_engine::stop() {
    if(check(m_stream->requestStop(), "Error stopping stream: {}")) {
        m_is_playing = false;
    }
}

void audio_engine::play(std::shared_ptr<renderable_audio> p_audio) {
    android_assert(m_mode == mode::async, "playing sounds and music in blocking mode is not implemented.");
    m_mixer->play_audio(p_audio);
}

void audio_engine::play(const std::vector<int16_t>& p_pcm) {
    android_assert(m_mode == mode::blocking, "playing raw pcm in async mode is not implemented.");
    m_stream->write(p_pcm.data(), p_pcm.size() / m_channels, std::numeric_limits<int64_t>::max());
}

void audio_engine::play(const std::vector<float>& p_pcm) {
    android_assert(m_mode == mode::blocking, "playing raw pcm in async mode is not implemented.");
    m_pcm_buffer.clear();
    std::transform(p_pcm.cbegin(), p_pcm.cend(), std::back_inserter(m_pcm_buffer),
                   [](const float& p_sample) {
                       auto converted = p_sample * std::numeric_limits<int16_t>::max();
                       return static_cast<int16_t>(converted);
                   });
    m_stream->write(m_pcm_buffer.data(), m_pcm_buffer.size() / m_channels, std::numeric_limits<int64_t>::max());
}

bool audio_engine::is_mono() {
    return m_channels == 1;
}

void audio_engine::volume(float p_volume) {
    m_volume = std::max(std::min(p_volume, 1.0f), 0.0f);
    m_mixer->m_volume = m_volume;
}

int32_t audio_engine::payload_size() {
    return m_payload_size * m_channels;
}