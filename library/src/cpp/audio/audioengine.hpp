#pragma once
#include <oboe/Oboe.h>
#include "mixer.hpp"
#include "../opensl/context.hpp"
#include "../sound/soundpool.hpp"
#include "../music/music.hpp"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

class audio_engine : protected oboe::AudioStreamCallback {
    private:
        std::unique_ptr<oboe::AudioStream> m_stream;
        std::unique_ptr<mixer> m_mixer;
        opensl::context m_slcontext;
        AAssetManager* m_asset_manager;
        int8_t m_channels;

        oboe::DataCallbackResult onAudioReady(oboe::AudioStream*, void*, int32_t);
    public:
        /// Opening audio stream with specified number of channels
        audio_engine(int32_t, AAssetManager*);

        /// Closing audio stream
        ~audio_engine();

        /// Request audio stream to start
        void resume();

        /// Request audio stream to stop streaming audio data
        void stop();

        void play(renderable_audio* p_audio);

        /// Creates new soundpool which is connected to this
        music* new_music(std::string_view);
};