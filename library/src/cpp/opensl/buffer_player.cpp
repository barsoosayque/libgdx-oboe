#include "buffer_player.hpp"
#include "../utility/log.hpp"
#include <vector>

using namespace opensl;

inline void get_interface(SLObjectItf p_player, const SLInterfaceID p_id,
                          void* p_interface, std::string_view p_id_desc) {
    auto result = (*p_player)->GetInterface(p_player, p_id, p_interface);
    if(result != SL_RESULT_SUCCESS) error("Cannot access interface \"{}\" in opensl::buffer_player.", p_id_desc);
}

buffer_player::buffer_player(const context& p_context, SLDataSource p_source)
    : m_queue_buffer(192)
    , m_queued_buffers(0)
    , m_playback_over(false) {
    std::vector<SLInterfaceID> ids { SL_IID_PLAY, SL_IID_SEEK, SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    std::vector<SLboolean> req { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

    SLDataLocator_AndroidSimpleBufferQueue loc_bq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 1 };
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

    auto engine = p_context.engine();
    auto result = (*engine)->CreateAudioPlayer(engine, &m_player,
                                               &p_source, &sink,
                                               ids.size(), ids.data(), req.data());
    if(result != SL_RESULT_SUCCESS) error("Can't create an audio buffer_player in opensl::buffer_player !");
    (*m_player)->Realize(m_player, SL_BOOLEAN_FALSE);

    get_interface(m_player, SL_IID_PLAY, &m_play, "SL_IID_PLAY");
    get_interface(m_player, SL_IID_SEEK, &m_seek, "SL_IID_SEEK");
    get_interface(m_player, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &m_queue, "SL_IID_ANDROIDSIMPLEBUFFERQUEUE");

    (*m_queue)->RegisterCallback(m_queue, [](const SLBufferQueueItf p_queue, void* p_self) {
        auto self = reinterpret_cast<buffer_player*>(p_self);
        if(self->m_buffer_callback) {
            // TODO fix endiannes if CPU is big endian
            auto cbegin = self->m_queue_buffer.cbegin();
            auto cend = std::next(cbegin, self->m_queue_buffer.capacity());
            self->m_buffer_callback(cbegin, cend);
        }
        if(--(self->m_queued_buffers)) {
            self->enqueue();
        } else {
            self->pause();
        }
    }, this);

    (*m_play)->SetCallbackEventsMask(m_play, SL_PLAYEVENT_HEADATEND);
    (*m_play)->RegisterCallback(m_play, [](SLPlayItf caller, void *context, SLuint32 event) {
        if(event == SL_PLAYEVENT_HEADATEND) {
            reinterpret_cast<buffer_player*>(context)->m_playback_over = true;
        }
    }, this);

    (*m_play)->GetDuration(m_play, &m_duration);
    (*m_play)->SetPositionUpdatePeriod(m_play, 25);
}

buffer_player::~buffer_player() {
    stop();
    (*m_player)->Destroy(m_player);
}

void buffer_player::on_buffer_update(buffer_callback p_callback) {
    m_buffer_callback = p_callback;
}

void buffer_player::play() {
    (*m_play)->SetPlayState(m_play, SL_PLAYSTATE_PLAYING);
}

void buffer_player::pause() {
    (*m_play)->SetPlayState(m_play, SL_PLAYSTATE_PAUSED);
}

void buffer_player::stop() {
    (*m_play)->SetPlayState(m_play, SL_PLAYSTATE_STOPPED);
    m_playback_over = false;
}

bool buffer_player::is_working() const {
    return m_queued_buffers != 0 && !m_playback_over;
}

float buffer_player::duration() const {
    return m_duration * .001f;
}

float buffer_player::position() const {
    (*m_play)->GetPosition(m_play, &m_position);
    return m_position * .001f;
}

void buffer_player::seek(float p_position) {
    if(p_position < m_duration) {
        m_playback_over = false;
    }
    (*m_seek)->SetPosition(m_seek, p_position * 1000, SL_SEEKMODE_ACCURATE);
}

float buffer_player::samples_to_seconds(int p_samples) {
    // 2 channels
    // 44100 Hz
    // 16 bit each
    return (p_samples * 0.5f) / 44100.0f;
}

void buffer_player::enqueue() {
    if(m_playback_over) return;

    m_queue_buffer.clear();
    // capacity * 2: because this argument in 8 bits, but buffer is in 16 bits
    auto result = (*m_queue)->Enqueue(m_queue, m_queue_buffer.data(), m_queue_buffer.capacity() * 2);
}

void buffer_player::enqueue(int p_buffers) {
    m_queued_buffers += p_buffers;
    enqueue();
    play();
}

void buffer_player::resize_buffer(int p_size) {
    pcm_container(p_size).swap(m_queue_buffer);
}

int buffer_player::buffer_size() {
    return m_queue_buffer.capacity();
}
