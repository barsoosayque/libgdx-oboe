#pragma once

#include <cmath>

class pan_effect {
public:
    pan_effect(float value) { pan(value); }

    float pan() { return m_pan; }

    void pan(float value) {
        m_pan = std::min(std::max(-1.0f, value), 1.0f);
        m_a = -std::max(m_pan, 0.0f);
        m_b = std::min(m_pan, 0.0f);
    }

    inline float modulation(int channel) {
        return 1 + m_a * (1 - channel) + m_b * channel;
    }

private:
    float m_pan, m_a, m_b;
};