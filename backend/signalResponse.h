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


FDF computeResponse(const VD & input, const VD & output, int sampleRate);

FDF computeResponseWiener(const FDF & signal, const FDF & output, const FDF & noise);

