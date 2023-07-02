#pragma once

#include "signalFrequencyDomain.h"
#include "../constants.h"

struct ParamResponse {
    int freqMin = 20;
    int freqMax = 20000;
    double duration = 0.1;
};

struct ResultResponse {
    FDF response;
    ParamResponse params;
    std::string name;
};

ResultResponse computeResponse(ParamResponse p,const VD & in,const VD & out, uint sampleRate);
