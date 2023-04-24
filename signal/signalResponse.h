#pragma once

#include "signalFrequencyDomain.h"
#include "../constants.h"

struct ParamResponse {
    int freqMin = 20;
    int freqMax = 20000;
};

struct ResultResponse {
    FDF response;
    ParamResponse params;
};


struct ImpulseResponse{
    typedef struct ParamResponse Param;
    typedef struct ResultResponse Result;
    VD generate_data(Param p, uint sampleRate);
    Result computeResult(const VD & out, Param p, uint sampleRate);
};

FDF computeResponse(const VD & input, const VD & output, int sampleRate);
