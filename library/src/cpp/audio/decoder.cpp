#include "decoder.hpp"
#include "../utility/log.hpp"
#include <cmath>
#include <algorithm>
using namespace opensl;

decoder::decoder(const context& p_context) : m_context(p_context) { }

decoder::decoder(decoder&& p_decoder)
    : m_context(std::move(p_decoder.m_context))
    , m_player(std::move(p_decoder.m_player)) {
    reattach_callback();
}

void decoder::open(int p_file_descriptor, off_t p_start, off_t p_length) {
    SLDataLocator_AndroidFD loc_fd = { SL_DATALOCATOR_ANDROIDFD, p_file_descriptor, p_start, p_length };
    // NOTE: Android OpenSL demands NULL and SL_CONTAINERTYPE_UNSPECIFIED
    SLDataFormat_MIME format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
    SLDataSource source = { &loc_fd, &format_mime };

    m_player = std::make_unique<buffer_player>(m_context, source);
    reattach_callback();
}

void decoder::open(AAsset* p_asset) {
    off_t start, length;
    int fd = AAsset_openFileDescriptor(p_asset, &start, &length);
    assert(0 <= fd);

    open(fd, start, length);
}

void decoder::reattach_callback() {
    m_player->on_buffer_update([&] (buffer_player::buffer_iterator p_begin,
                                    buffer_player::buffer_iterator p_end) {
        m_merged_buffers.insert(m_merged_buffers.end(), std::make_move_iterator(p_begin),
                                                        std::make_move_iterator(p_end));
    });
}

void decoder::enqueue_and_wait(int p_buffers) {
    m_player->enqueue(p_buffers);
    while(m_player->is_working()) {};
}

std::vector<int16_t> decoder::request_more(int p_samples) {
    int buffer_size = m_player->buffer_size();
    int remainder = 0;
    int requested_buffers = p_samples >= 0 ? std::floor(p_samples / static_cast<float>(buffer_size))
                                         : -1;
    if(requested_buffers >= 1) {
        m_merged_buffers.reserve(p_samples);
        remainder = p_samples - buffer_size * requested_buffers;
    }
    m_merged_buffers.clear();

    enqueue_and_wait(requested_buffers);

    if(remainder) {
        m_player->resize_buffer(remainder);
        enqueue_and_wait(1);
        m_player->resize_buffer(buffer_size);
    }

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

std::vector<int16_t> decoder::decode_full(const context& p_context, AAsset* p_asset) {
    auto dr = decoder(p_context);
    dr.open(p_asset);
    return dr.request_more(-1);
}
