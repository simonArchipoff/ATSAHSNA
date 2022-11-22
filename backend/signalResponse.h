#pragma once

#include "signalAnalysis.h"
#include "signalFrequencyDomain.h"
#include "../constants.h"

struct ParamResponse {
  int freqMin = 20;
  int freqMax = 20000;
  signal_gen_type mode;
  double duration=0.0;
};

struct ResultResponse {
  FDF response;
  ParamResponse params;
  MeasureData raw_data;
};

FDF computeResponse(const VD & input, const VD & output, int sampleRate);

