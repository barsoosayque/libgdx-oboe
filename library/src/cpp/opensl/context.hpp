#pragma once

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace opensl {
    class context {
        public:
            context();
            ~context();

            const SLEngineItf engine() const;
        private:
            SLObjectItf m_engine_object;
            SLEngineItf m_engine;
    };
}