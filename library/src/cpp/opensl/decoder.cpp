#include "decoder.hpp"
using namespace opensl;

decoder::decoder(const context& p_context) : m_context(p_context) { }

void decoder::open(int p_file_descriptor, off_t p_start, off_t p_length) {
    SLDataLocator_AndroidFD loc_fd = { SL_DATALOCATOR_ANDROIDFD, p_file_descriptor, p_start, p_length };
    // NOTE: Android OpenSL demands NULL and SL_CONTAINERTYPE_UNSPECIFIED
    SLDataFormat_MIME format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
    SLDataSource source = { &loc_fd, &format_mime };

    m_player = std::make_unique<buffer_player>(m_context, source);
}

bool decoder::is_opened() {
    return m_player != nullptr;
}

std::vector<int16_t> decoder::decode_full(const context& p_context, int p_file_descriptor,
                                          off_t p_start, off_t p_length) {
    auto dr = decoder(p_context);
    dr.open(p_file_descriptor, p_start, p_length);
    std::vector<int16_t> pcm;

    dr.m_player->on_buffer_update([&dr, &pcm] (const std::vector<int16_t>& p_buffer) {
        auto cend = std::next(p_buffer.cend(), p_buffer.capacity());
        std::move(p_buffer.cbegin(), cend, std::back_inserter(pcm));
        dr.m_player->enqueue();
    });

    dr.m_player->enqueue();
    dr.m_player->play();
    while(dr.m_player->is_working()) {};
    dr.m_player->stop();

    return pcm;
}
