#pragma once

#include "signalAnalysis.h"
#include "signalFrequencyDomain.h"

struct ParamResponse {
  int freqMin,freqMax;
  enum {SWEEP, IMPULSE, NOISE} mode = IMPULSE;
};

struct ResultResponse{
  FDF response;
  ParamResponse params;
  MeasureData raw_data;
};

