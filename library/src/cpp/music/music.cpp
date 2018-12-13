#include "music.hpp"
#include "../utility/log.hpp"

music::music(opensl::decoder&& p_decoder, int8_t p_channels)
    : m_decoder(std::move(p_decoder))
    , m_channels(p_channels) {
    if(!m_decoder.is_opened()) {
        error("Trying to create music with decoder that is not open.");
    }
}

void music::position(float p_position) {
    m_decoder.position(p_position);
}

float music::position() {
    return m_decoder.position();
}

float music::duration() {
    return m_decoder.content_duration();
}

void music::render(int16_t* p_stream, int32_t p_frames) const {

}

bool music::is_done() const {
    // TODO seems like a hack
    return false;
}
