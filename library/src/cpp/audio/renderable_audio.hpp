#pragma once

#include <cstdint>

/// Interface for writing audio into raw audio stream.
class renderable_audio {
public:
    /// Render this audio into the stream for num_frames of frames.
    virtual void render(int16_t* stream, uint32_t num_frames) = 0;
protected:
    virtual ~renderable_audio() = default;
};