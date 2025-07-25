project(libgdx-oboe)
cmake_minimum_required(VERSION 3.7.1)

# Enable LTO optimization
# https://github.com/android/ndk/issues/721
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -flto=full -ffunction-sections -fdata-sections -fvisibility=hidden")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -flto=full -ffunction-sections -fdata-sections -fvisibility=hidden")
# Enable --as-needed
set(LINK_WHAT_YOU_USE TRUE)
set(ANDROID_STL c++_static)

set(OBOE_DIR dependencies/oboe)
add_subdirectory (${OBOE_DIR} ./oboe)
include_directories (${OBOE_DIR}/include)

set(FROZEN_STRING dependencies/frozen_string)
include_directories (${FROZEN_STRING})

set(FMT_DIR dependencies/fmt)
add_subdirectory (${FMT_DIR} ./fmt)
include_directories (${FMT_DIR}/include)

set(RESULT_DIR dependencies/result)
include_directories (${RESULT_DIR})

set(BUILD_TESTING OFF)
set(LIBSAMPLERATE_EXAMPLES OFF)
set(SAMPLERATE dependencies/libsamplerate)
add_subdirectory (${SAMPLERATE} ./samplerate)
include_directories (${SAMPLERATE}/include)

set(FFMPEG_DIR dependencies/ffmpeg)
include_directories (${FFMPEG_DIR})
set(FFMPEG_LIBS avformat avcodec avutil swresample)
foreach(LIB ${FFMPEG_LIBS})
    add_library( ${LIB} SHARED IMPORTED )
    set_target_properties( ${LIB} PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/lib${LIB}.so" )
endforeach()

set(SRC_DIR src/cpp)
add_library(libgdx-oboe SHARED
            ${SRC_DIR}/samplerate/resampler.cpp
            ${SRC_DIR}/mediacodec/audio_decoder.cpp
            ${SRC_DIR}/mediacodec/internal_asset.cpp
            ${SRC_DIR}/mediacodec/decoder_bundle.cpp
            ${SRC_DIR}/audio/oboe_engine.cpp
            ${SRC_DIR}/audio/audio_player.cpp
            ${SRC_DIR}/audio/soundpool.cpp
            ${SRC_DIR}/audio/music.cpp
            ${SRC_DIR}/native/init.cpp
            ${SRC_DIR}/native/oboe_sound.cpp
            ${SRC_DIR}/native/oboe_music.cpp
            ${SRC_DIR}/native/oboe_audio.cpp
            ${SRC_DIR}/native/oboe_audio_device.cpp
            ${SRC_DIR}/native/oboe_audio_recorder.cpp)

target_link_libraries(libgdx-oboe samplerate android log oboe fmt OpenSLES ${FFMPEG_LIBS})
target_compile_options(libgdx-oboe PRIVATE -Wall)
target_link_options(libgdx-oboe PRIVATE "-Wl,-z,max-page-size=16384")
target_link_options(libgdx-oboe PRIVATE "-Wl,-z,common-page-size=16384")

# Strip
add_custom_command(TARGET libgdx-oboe POST_BUILD
        COMMAND "${CMAKE_STRIP}" -g -S -d --strip-debug
        "$<TARGET_FILE:libgdx-oboe>")
foreach(LIB ${FFMPEG_LIBS})
    add_custom_command(TARGET libgdx-oboe POST_BUILD
            COMMAND "${CMAKE_STRIP}" -g -S -d --strip-debug
            "${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/lib${LIB}.so")
endforeach()
