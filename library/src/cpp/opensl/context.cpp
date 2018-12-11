#include "context.hpp"
#include <array>

using namespace opensl;

context::context() {
    std::array<SLEngineOption, 2> options { SL_ENGINEOPTION_THREADSAFE, SL_BOOLEAN_TRUE };
    slCreateEngine(&m_engine_object, 1, options.data(), 0, NULL, NULL);
    (*m_engine_object)->Realize(m_engine_object, SL_BOOLEAN_FALSE);
    (*m_engine_object)->GetInterface(m_engine_object, SL_IID_ENGINE, &m_engine);
}

context::~context() {
    (*m_engine_object)->Destroy(m_engine_object);
}

const SLEngineItf context::engine() const {
    return m_engine;
}
