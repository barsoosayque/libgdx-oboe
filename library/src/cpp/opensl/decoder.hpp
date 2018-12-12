#pragma once
#include <memory>
#include <vector>

#include "context.hpp"
#include "buffer_player.hpp"

namespace opensl {
    class decoder {
        public:
            decoder(const context& p_context);

            void open(int p_file_descriptor, off_t p_start, off_t p_length);

            bool is_opened();

            void position(float p_position);
            float position();
            float content_duration();

            static std::vector<int16_t> decode_full(const context& p_context,
                                                    int p_file_descriptor,
                                                    off_t p_start,
                                                    off_t p_length);
        private:
            const context& m_context;
            std::unique_ptr<buffer_player> m_player;
    };
}