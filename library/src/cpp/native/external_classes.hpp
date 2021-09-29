#pragma once

#include "jni.h"

namespace external {
struct gdx_music_listener {
    static constexpr auto cls = "com/badlogic/gdx/audio/Music$OnCompletionListener"_fstr;
    jobject obj;
};

struct gdx_music {
    static constexpr auto cls = "com/badlogic/gdx/audio/Music"_fstr;
    jobject obj;
};

struct gdx_runtime_exception {
    static constexpr auto cls = "com/badlogic/gdx/utils/GdxRuntimeException"_fstr;
    jobject obj;
};
}