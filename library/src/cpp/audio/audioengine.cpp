#include "audioengine.hpp"
#include "../utility/ptrptr.hpp"
#include "../utility/log.hpp"
#include "../utility/oboeutils.hpp"
#include <array>
#include <algorithm>
#include <iterator>

using namespace oboe;

audio_engine::audio_engine(int8_t p_channels, int32_t p_sample_rate)
    : AudioStreamCallback()
    , m_channels(p_channels)
    , m_mode(mode::stream)
    , m_volume(1) {

    // initialize Oboe audio stream
    AudioStreamBuilder builder;
    builder.setChannelCount(m_channels);
    builder.setSampleRate(p_sample_rate);
    builder.setCallback(this);
    builder.setFormat(AudioFormat::I16);
    builder.setPerformanceMode(PerformanceMode::LowLatency);
    builder.setSharingMode(SharingMode::Exclusive);

    check(builder.openStream(ptrptr(m_stream)), "Error opening stream: %s");

    m_payload_size = m_stream->getFramesPerBurst() * 2;
    auto result = m_stream->setBufferSizeInFrames(m_payload_size);
    m_mixer = std::make_unique<mixer>(m_payload_size * p_channels, p_channels);

    if (result) {
        debug("New buffer size is %d frame", result.value());
    }
}

audio_engine::~audio_engine() {
    stop();
    check(m_stream->close(), "Error closing stream: %s");
}

DataCallbackResult audio_engine::onAudioReady(AudioStream* self, void* p_audio_data, int32_t p_num_frames) {
    auto stream = static_cast<int16_t*>(p_audio_data);

    switch(m_mode) {
        case mode::mix:
            m_mixer->render(stream, p_num_frames);
        break;
        case mode::stream:
            int32_t size = std::min(p_num_frames * m_channels, static_cast<int32_t>(m_pcm_buffer.size()));
            for (int i = 0; i < size; ++i) {
                stream[i] = m_pcm_buffer[i] * m_volume;
            }
            m_pcm_buffer.erase(m_pcm_buffer.begin(), std::next(m_pcm_buffer.begin(), size));
        break;
    }

    return DataCallbackResult::Continue;
}

void audio_engine::resume() {
    check(m_stream->requestStart(), "Error starting stream: %s");
}

void audio_engine::stop() {
    check(m_stream->requestStop(), "Error stopping stream: %s");
}

void audio_engine::play(std::shared_ptr<renderable_audio> p_audio) {
    m_mode = mode::mix;
    m_mixer->play_audio(p_audio);
}

void audio_engine::play(const std::vector<int16_t>& p_pcm) {
    m_mode = mode::stream;
    std::move(p_pcm.cbegin(), p_pcm.cend(), m_pcm_buffer.end());
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