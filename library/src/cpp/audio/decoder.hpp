#pragma once
#include <memory>
#include <vector>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "../opensl/context.hpp"
#include "../opensl/buffer_player.hpp"

namespace opensl {
    class decoder {
        public:
            decoder(const context& p_context);
            decoder(decoder&& p_decoder);

            void open(int p_file_descriptor, off_t p_start, off_t p_length);
            void open(AAsset* p_asset);
            std::vector<int16_t> request_more(int p_samples);

            bool is_opened();

            void position(float p_position);
            float position();
            float content_duration();

            static std::vector<int16_t> decode_full(const context& p_context, AAsset* p_asset);
        private:
            void enqueue_and_wait(int p_buffers);
            void reattach_callback();

            const context& m_context;
            std::unique_ptr<buffer_player> m_player;
            buffer_player::pcm_container m_merged_buffers;
    };
}