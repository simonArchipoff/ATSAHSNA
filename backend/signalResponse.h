#pragma once

#include "signalAnalysis.h"
#include "signalFrequencyDomain.h"
#include "../constants.h"
struct ParamResponse {
  int freqMin;
  int freqMax;
  signal_gen_type mode;
};

struct ResultResponse{
  FDF response;
  ParamResponse params;
  MeasureData raw_data;
};


FDF computeResponse(const VD & input, const VD & output, int sampleRate);

