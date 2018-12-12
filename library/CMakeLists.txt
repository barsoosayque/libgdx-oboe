cmake_minimum_required(VERSION 3.4.1)

set (OBOE_DIR dependencies/oboe)
add_subdirectory (${OBOE_DIR} ./oboe)
include_directories (${OBOE_DIR}/include)

set (FMT_DIR dependencies/fmt)
add_subdirectory (${FMT_DIR} ./fmt)
include_directories (${FMT_DIR}/include)

set (SRC_DIR src/cpp)
add_library(libgdx-oboe SHARED
            ${SRC_DIR}/opensl/context.cpp
            ${SRC_DIR}/opensl/buffer_player.cpp
            ${SRC_DIR}/opensl/decoder.cpp
            ${SRC_DIR}/audio/audioengine.cpp
            ${SRC_DIR}/audio/mixer.cpp
            ${SRC_DIR}/sound/soundpool.cpp
            ${SRC_DIR}/music/music.cpp
            ${SRC_DIR}/jni/oboesound.cpp
            ${SRC_DIR}/jni/oboemusic.cpp
            ${SRC_DIR}/jni/oboeaudio.cpp)

target_link_libraries(libgdx-oboe android log oboe fmt OpenSLES)
