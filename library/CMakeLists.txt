cmake_minimum_required(VERSION 3.4.1)

set (OBOE_DIR dependencies/oboe)
add_subdirectory (${OBOE_DIR} ./oboe)
include_directories (${OBOE_DIR}/include)

set (FROZEN_STRING dependencies/frozen_string)
include_directories (${FROZEN_STRING})

set (FMT_DIR dependencies/fmt)
add_subdirectory (${FMT_DIR} ./fmt)
include_directories (${FMT_DIR}/include)

set (LIBSAMPLERATE_TESTS OFF)
set (LIBSAMPLERATE_EXAMPLES OFF)
set (SAMPLERATE dependencies/libsamplerate)
add_subdirectory (${SAMPLERATE} ./samplerate)
include_directories (${SAMPLERATE}/include)

set (LIBAV_DIR dependencies/libav)
include_directories (${LIBAV_DIR})
set (LIBAV_LIBS avformat avcodec avutil)
foreach(LIB ${LIBAV_LIBS})
    add_library( ${LIB} SHARED IMPORTED )
    set_target_properties( ${LIB} PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/lib${LIB}.so" )
endforeach()

set (SRC_DIR src/cpp)
add_library(libgdx-oboe SHARED
            ${SRC_DIR}/samplerate/resampler.cpp
            ${SRC_DIR}/mediacodec/audio_decoder.cpp
            ${SRC_DIR}/audio/audioengine.cpp
            ${SRC_DIR}/audio/mixer.cpp
            ${SRC_DIR}/sound/soundpool.cpp
            ${SRC_DIR}/music/music.cpp
            ${SRC_DIR}/native/init.cpp
            ${SRC_DIR}/native/oboesound.cpp
            ${SRC_DIR}/native/oboemusic.cpp
            ${SRC_DIR}/native/oboeaudio.cpp
            ${SRC_DIR}/native/oboeaudiodevice.cpp)

target_link_libraries(libgdx-oboe samplerate android log oboe fmt OpenSLES ${LIBAV_LIBS})
target_compile_options(libgdx-oboe PRIVATE -Wall -Werror "$<$<CONFIG:RELEASE>:-Ofast>")
