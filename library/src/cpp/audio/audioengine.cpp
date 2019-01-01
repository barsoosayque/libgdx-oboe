#include "audioengine.hpp"
#include "../utility/ptrptr.hpp"
#include "../utility/log.hpp"
#include "../utility/oboeutils.hpp"
#include <array>

using namespace oboe;

audio_engine::audio_engine(int8_t p_channels)
    : AudioStreamCallback()
    , m_channels(p_channels) {

    // initialize Oboe audio stream
    AudioStreamBuilder builder;
    builder.setChannelCount(m_channels);
    builder.setSampleRate(44100);
    builder.setCallback(this);
    builder.setFormat(AudioFormat::I16);
    builder.setPerformanceMode(PerformanceMode::LowLatency);
    builder.setSharingMode(SharingMode::Exclusive);

    check(builder.openStream(ptrptr(m_stream)), "Error opening stream: %s");

    int m_max_frames = m_stream->getFramesPerBurst() * 2;
    auto result = m_stream->setBufferSizeInFrames(m_max_frames);
    m_mixer = std::make_unique<mixer>(m_max_frames * p_channels, p_channels);

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

    m_mixer->render(stream, p_num_frames);

    return DataCallbackResult::Continue;
}

void audio_engine::resume() {
    check(m_stream->requestStart(), "Error starting stream: %s");
}

void audio_engine::stop() {
    check(m_stream->requestStop(), "Error stopping stream: %s");
}

void audio_engine::play(std::shared_ptr<renderable_audio> p_audio) {
    m_mixer->play_audio(p_audio);
}
