#include "playerhandle.hpp"
#include <array>
#include "../utility/log.hpp"

player_handle::player_handle(SLEngineItf& p_engine, SLDataSource& p_source) {
    // configure audio sink to pcm buffer
    // NOTE: there is also SLDataLocator_AndroidSimpleBufferQueue, but
    // I did not success finding out why such thing exists.
    SLDataLocator_AndroidSimpleBufferQueue loc_bq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };
    SLDataFormat_PCM pcm = {
        .formatType = SL_DATAFORMAT_PCM,
        .numChannels = 2,
        .samplesPerSec = SL_SAMPLINGRATE_44_1,
        .bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16,
        .containerSize = SL_PCMSAMPLEFORMAT_FIXED_16,
        .channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
        .endianness = SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSink sink = { &loc_bq, &pcm };

    // create audio player
    std::array<SLInterfaceID, 2> ids { SL_IID_SEEK, SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
    std::array<SLboolean, 2> req { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

    auto result = (*p_engine)->CreateAudioPlayer(p_engine, &m_player, &p_source, &sink, 2, ids.data(), req.data());
    assert(SL_RESULT_SUCCESS == result);

    // realize the player
    (*m_player)->Realize(m_player, SL_BOOLEAN_FALSE);

    bind_interfaces();
    frames_in_buffer(192);
    m_playback_over = false;

    (*m_play)->SetCallbackEventsMask(m_play, SL_PLAYEVENT_HEADATEND);
    (*m_play)->RegisterCallback(m_play, [](SLPlayItf caller, void *context, SLuint32 event) {
        if(event == SL_PLAYEVENT_HEADATEND) {
            reinterpret_cast<player_handle*>(context)->m_playback_over = true;
        }
    }, this);
}

void player_handle::frames_in_buffer(int p_frames) {
    m_pcm_buffer.reserve(p_frames);
}

void player_handle::enqueue_new_data() {
    m_pcm_buffer.clear();
    // capacity * 2: because this argument in 8 bits, but buffer is in 16 bits
    (*m_queue)->Enqueue(m_queue, m_pcm_buffer.data(), m_pcm_buffer.capacity() * 2);
}

SLmillisecond player_handle::duration() {
    SLmillisecond time;
    (*m_play)->GetDuration(m_play, &time);
    return time;
}

SLmillisecond player_handle::position() {
    SLmillisecond time;
    (*m_play)->GetPosition(m_play, &time);
    return time;
}

player_handle::~player_handle() {
    (*m_play)->SetPlayState(m_play, SL_PLAYSTATE_STOPPED);
    (*m_player)->Destroy(m_player);
}

inline void get_interface(SLObjectItf p_player, const SLInterfaceID p_id,
                          void* p_interface, std::string_view p_id_desc) {
    auto result = (*p_player)->GetInterface(p_player, p_id, p_interface);
    if( result != SL_RESULT_SUCCESS ) {
        error("Player handle: Cannot access interface \"{}\".", p_id_desc);
    }
}

void player_handle::bind_interfaces() {
    get_interface(m_player, SL_IID_PLAY, &m_play, "SL_IID_PLAY");
    get_interface(m_player, SL_IID_SEEK, &m_seek, "SL_IID_SEEK");
    get_interface(m_player, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &m_queue, "SL_IID_ANDROIDSIMPLEBUFFERQUEUE");
}
