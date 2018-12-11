#pragma once
#include <unique_ptr>

#include "context.hpp"
#include "player.hpp"

namespace opensl {
    class decoder {
        public:
            decoder(const context& p_context);

            void open(int p_file_descriptor, off_t p_start, off_t p_length);
        private:
            const context& m_context;
            std::unique_ptr<player> m_player;
    };
}