#pragma once

#include <cmath>

/// Audio panning effect, capable of modulating channel volume
/// based on pan value [-1.0; 1.0].
class pan_effect {
public:
    /// Creates a pan_effect with pan value set.
    pan_effect(float value) { pan(value); }

    /// Returns current pan value.
    float pan() { return m_pan; }

    /// Sets pan value.
    void pan(float value) {
        m_pan = std::min(std::max(-1.0f, value), 1.0f);
        m_a = -std::max(m_pan, 0.0f);
        m_b = std::min(m_pan, 0.0f);
    }

    /// Get channel modulation based on current pan value.
    inline float modulation(int channel) {
        return 1 + m_a * (1 - channel) + m_b * channel;
    }

private:
    float m_pan, m_a, m_b;
};