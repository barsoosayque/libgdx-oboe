#include "decoder.hpp"
#include <cmath>
using namespace opensl;

decoder::decoder(const context& p_context) : m_context(p_context) { }

void decoder::open(int p_file_descriptor, off_t p_start, off_t p_length) {
    SLDataLocator_AndroidFD loc_fd = { SL_DATALOCATOR_ANDROIDFD, p_file_descriptor, p_start, p_length };
    // NOTE: Android OpenSL demands NULL and SL_CONTAINERTYPE_UNSPECIFIED
    SLDataFormat_MIME format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
    SLDataSource source = { &loc_fd, &format_mime };

    m_player = std::make_unique<buffer_player>(m_context, source);

    m_player->on_buffer_update([this] (const std::vector<int16_t>& p_buffer) {
        auto cend = std::next(p_buffer.cend(), p_buffer.capacity());
        std::move(p_buffer.cbegin(), cend, std::back_inserter(m_merged_buffers));
        if(--m_requested_buffers > 0) {
            m_player->enqueue();
        }
    });
}

std::vector<int16_t> decoder::request_more(int p_samples) {
    m_merged_buffers.reserve(p_samples);
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
    return dr.request_more(p_length * 2);
}
