#pragma once
#include <cmath>

class pan_effect {
    public:
        pan_effect(float p_pan) { pan(p_pan); }

        float pan() { return m_pan; }
        void pan(float p_pan) {
            m_pan = std::min(std::max(-1.0f, p_pan), 1.0f);
        }

        float modulation(int p_channel) {
            switch (p_channel) {
                case 0: return 1.0f - std::max(m_pan, .0f); break;
                case 1: return 1.0f + std::min(m_pan, .0f); break;
                default: return 1.0f; break;
            }
        }
    private:
        float m_pan;
};