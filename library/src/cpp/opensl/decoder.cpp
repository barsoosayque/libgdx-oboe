#include "decoder.hpp"
#include "../utility/log.hpp"
#include <cmath>
using namespace opensl;

decoder::decoder(const context& p_context) : m_context(p_context) { }

void decoder::open(int p_file_descriptor, off_t p_start, off_t p_length) {
    SLDataLocator_AndroidFD loc_fd = { SL_DATALOCATOR_ANDROIDFD, p_file_descriptor, p_start, p_length };
    // NOTE: Android OpenSL demands NULL and SL_CONTAINERTYPE_UNSPECIFIED
    SLDataFormat_MIME format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
    SLDataSource source = { &loc_fd, &format_mime };

    m_player = std::make_unique<buffer_player>(m_context, source);

    m_player->on_buffer_update([this] (buffer_player::buffer_iterator p_begin,
                                       buffer_player::buffer_iterator p_end) {
        std::move(p_begin, p_end, std::back_inserter(m_merged_buffers));
        if(--m_requested_buffers) {
            m_player->enqueue();
        }
    });
}

std::vector<int16_t> decoder::request_more(int p_samples) {
    m_requested_buffers = p_samples >= 0 ? std::ceil(p_samples / static_cast<float>(m_player->buffer_size()))
                                         : -1;
    if(m_requested_buffers >= 1) {
        m_merged_buffers.reserve(m_requested_buffers * m_player->buffer_size());
    }
    m_merged_buffers.clear();
    m_requested_buffers = std::ceil(p_samples / static_cast<float>(m_player->buffer_size()));

    m_player->enqueue();
    m_player->play();
    while(m_player->is_working()) {};
    m_player->pause();

    return m_merged_buffers;
}

float decoder::position() {
    return m_player->position();
}

void decoder::position(float p_position) {
    m_player->seek(p_position);
}

float decoder::content_duration() {
    return m_player->duration();
}

bool decoder::is_opened() {
    return m_player != nullptr;
}

std::vector<int16_t> decoder::decode_full(const context& p_context, int p_file_descriptor,
                                          off_t p_start, off_t p_length) {
    auto dr = decoder(p_context);
    dr.open(p_file_descriptor, p_start, p_length);
    return dr.request_more(-1);
}
