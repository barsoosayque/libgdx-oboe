#include "player.hpp"
#include "../utility/log.hpp"

using namespace opensl;

inline void get_interface(SLObjectItf p_player, const SLInterfaceID p_id,
                          void* p_interface, std::string_view p_id_desc) {
    auto result = (*p_player)->GetInterface(p_player, p_id, p_interface);
    if(result != SL_RESULT_SUCCESS) error("Cannot access interface \"{}\" in opensl::player.", p_id_desc);
}

player::player(const context& p_context SLDataSource p_source, SLDataSink p_sink) {
    std::vector<SLInterfaceID> ids { SL_IID_PLAY, SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    std::vector<SLboolean> req { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

    auto engine = p_context.engine();
    auto result = (*engine)->CreateAudioPlayer(engine, &m_player,
                                               &p_source, &p_sink,
                                               ids.size(), ids.data(), req.data());
    if(result != SL_RESULT_SUCCESS) error("Can't create an audio player in opensl::player !");
    (*m_player)->Realize(m_player, SL_BOOLEAN_FALSE);

    get_interface(m_player, SL_IID_PLAY, &m_play, "SL_IID_PLAY");
    get_interface(m_player, SL_IID_SEEK, &m_seek, "SL_IID_SEEK");
    get_interface(m_player, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &m_queue, "SL_IID_ANDROIDSIMPLEBUFFERQUEUE");
}

player::~player() {
    (*m_play)->SetPlayState(m_play, SL_PLAYSTATE_STOPPED);
    (*m_player)->Destroy(m_player);
}
