#include "audioengine.hpp"
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

    m_max_frames = m_stream->getFramesPerBurst() * 2;
    auto result = m_stream->setBufferSizeInFrames(m_max_frames);
    m_mixer = std::make_unique<audio_mixer>(m_max_frames * p_channels, p_channels);

    if (result) {
        debug("New buffer size is %d frame", result.value());
    }

    std::array<SLEngineOption, 2> options {
        SL_ENGINEOPTION_THREADSAFE,
        SL_BOOLEAN_TRUE
    };

    slCreateEngine(&m_sl, 1, options.data(), 0, NULL, NULL);
    (*m_sl)->Realize(m_sl, SL_BOOLEAN_FALSE);
}

audio_engine::~audio_engine() {
    stop();
    check(m_stream->close(), "Error closing stream: %s");
    (*m_sl)->Destroy(m_sl);
}

DataCallbackResult audio_engine::onAudioReady(AudioStream* self, void* p_audio_data, int32_t p_num_frames) {
    auto stream = static_cast<int16_t*>(p_audio_data);
//    for(int32_t i = 0; i < p_num_frames * m_channels; ++i) {
//        stream[i] = 0;
//    }

    for (int i = 0; i < p_num_frames; ++i) {
        m_mixer->render(stream + (m_channels * i), 1);
    }

    return DataCallbackResult::Continue;
}

int32_t audio_engine::max_frames() {
    return m_max_frames;
}

void audio_engine::resume() {
    check(m_stream->requestStart(), "Error starting stream: %s");
}

void audio_engine::stop() {
    check(m_stream->requestStop(), "Error stopping stream: %s");
}

soundpool* audio_engine::new_soundpool(std::string_view p_path) {
    SLEngineItf engine;
    (*m_sl)->GetInterface(m_sl, SL_IID_ENGINE, &engine);

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
    AAsset_close(asset);

    // configure audio source
    SLDataLocator_AndroidFD loc_fd = { SL_DATALOCATOR_ANDROIDFD, fd, start, length };
    SLDataFormat_MIME format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
    SLDataSource audio_source = { &loc_fd, &format_mime };

    auto new_soundpool = new soundpool(engine, audio_source);
    m_mixer->play_audio(new_soundpool);

    return new_soundpool;
}
