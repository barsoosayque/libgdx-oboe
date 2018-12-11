#include "decoder.hpp"

using namespace opensl;

decoder::decoder(const context& p_context) : m_context(p_context) { }

void decoder::open(int p_file_descriptor, off_t p_start, off_t p_length) {
    // <----- SOURCE ----->
    SLDataLocator_AndroidFD loc_fd = { SL_DATALOCATOR_ANDROIDFD, p_file_descriptor, p_start, p_length };
    // NOTE: Android OpenSL demands NULL and SL_CONTAINERTYPE_UNSPECIFIED
    SLDataFormat_MIME format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
    SLDataSource source = { &loc_fd, &format_mime };

    // <----- SINK ----->
    SLDataLocator_AndroidSimpleBufferQueue loc_bq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };
    SLDataFormat_PCM pcm = {
        .formatType = SL_DATAFORMAT_PCM,
        // TODO dynamic channels
        .numChannels = 2,
        // OpenSL ES for Android doesn't perform resampling, so
        // it's advisable by google to use 44.1Hz to match the device,
        // and it's most likely that 44.1Hz is how files are sampled anyways.
        .samplesPerSec = SL_SAMPLINGRATE_44_1,
        // Floats only from API 21
        .bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16,
        .containerSize = SL_PCMSAMPLEFORMAT_FIXED_16,
        // TODO dynamic channels
        .channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
        // Little Endian only on OpenSL ES for Android
        .endianness = SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSink sink = { &loc_bq, &pcm };

    m_player = std::make_unique<player>(m_context, source, sink);
}

