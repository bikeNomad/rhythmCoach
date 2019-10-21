#pragma once

#include <aubio.h>
#include "utils_tests.h"

class AubioProcessor {
    public:
        AubioProcessor() {
        }
        ~AubioProcessor() {
        }
    protected:
};

class AubioOnsetDetector : public AubioProcessor {
    public:
        AubioOnsetDetector() : AubioProcessor() {
        }
        ~AubioOnsetDetector() {
        }
        void process_block(fvec_t *ibuf, fvec_t *obuf);
    protected:
        aubio_onset_t *o;
        fvec_t *onset;
        smpl_t is_onset;
};

