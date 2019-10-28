#pragma once
#include <cmath>

class pan_effect {
    public:
        pan_effect(float p_pan) { pan(p_pan); }

        float pan() { return m_pan; }
        void pan(float p_pan) {
            m_pan = std::min(std::max(-1.0f, p_pan), 1.0f);
            m_a = -std::max(m_pan, 0.0f);
            m_b = std::min(m_pan, 0.0f);
        }

        inline float modulation(int p_channel) {
            return 1 + m_a * (1 - p_channel) + m_b * p_channel;
        }
    private:
        float m_pan, m_a, m_b;
};