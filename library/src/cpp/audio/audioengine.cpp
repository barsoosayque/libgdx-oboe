#include "audioengine.hpp"
#include "../opensl/decoder.hpp"
#include "../utility/ptrptr.hpp"
#include "../utility/log.hpp"
#include "../utility/oboeutils.hpp"
#include <array>

using namespace oboe;

audio_engine::audio_engine(int32_t p_channels, AAssetManager* p_asset_manager)
    : AudioStreamCallback()
    , m_channels(p_channels)
    , m_asset_manager(p_asset_manager) {

    // initialize Oboe audio stream
    AudioStreamBuilder builder;
    builder.setChannelCount(m_channels);
    builder.setSampleRate(44100);
    builder.setCallback(this);
    builder.setFormat(AudioFormat::I16);
    builder.setPerformanceMode(PerformanceMode::LowLatency);
    builder.setSharingMode(SharingMode::Exclusive);

    check(builder.openStream(ptrptr(m_stream)), "Error opening stream: %s");
    debug("Stream channel count: {}", m_stream->getChannelCount());
    debug("Stream sample rate: {}", m_stream->getSampleRate());
    debug("Stream format: {} (given: {})", (int)m_stream->getFormat(), (int)AudioFormat::I16);

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

std::tuple<int, off_t, off_t> audio_engine::path_to_fd(std::string_view p_path) {
    // use asset manager to open asset by filename
    AAsset* asset = AAssetManager_open(m_asset_manager, p_path.data(), AASSET_MODE_UNKNOWN);

    // the asset might not be found
    if (NULL == asset) {
        error("Failed loading asset \"%s\".", p_path);
    }

    // open asset as file descriptor
    off_t start, length;
    int fd = AAsset_openFileDescriptor(asset, &start, &length);
    assert(0 <= fd);

    return std::make_tuple(fd, start, length);
}

music* audio_engine::new_music(std::string_view p_path) {
    auto [fd, start, length] = path_to_fd(p_path);

    auto music_decoder = opensl::decoder(m_slcontext);
    music_decoder.open(fd, start, length);

    auto new_music = new music(std::move(music_decoder), m_channels);
    m_mixer->play_audio(new_music);

    return new_music;
}

soundpool* audio_engine::new_soundpool(std::string_view p_path) {
    auto [fd, start, length] = path_to_fd(p_path);

    auto pcm = opensl::decoder::decode_full(m_slcontext, fd, start, length);
    auto new_soundpool = new soundpool(std::move(pcm), m_channels);
    m_mixer->play_audio(new_soundpool);

    return new_soundpool;
}
