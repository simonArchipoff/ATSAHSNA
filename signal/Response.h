#pragma once

#include "FrequencyDomain.h"
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

VCD computeChirp(ParamResponse p, uint sampleRate);

ResultResponse computeResponse(ParamResponse p,const VD & in,const VD & out, uint sampleRate);
