#pragma once

#include "signalFrequencyDomain.h"
#include "signalAnalysis.h"

struct ParamHarmonics {
  double frequency = 1000, duration=1;
  double freqMin=20, freqMax=20000;
  double gain = 1;
};


struct ResultHarmonics {
  FDF harmonicSpectrum;
  double thdNoiseRate;
  double thdRate;
  double snr;
//  double sinad;
  vector<double> harmonicsLevel;
  //vector<double> harmonicsPhases;
  ParamHarmonics params;
};


struct HarmonicResponse {
  typedef struct ParamHarmonics Param;
  typedef struct ResultHarmonics Result;
  VD generate_data(Param p, uint sampleRate);
  Result computeResult(const VD & out, Param p, uint sampleRate);
};


ResultHarmonics computeTHD(const ParamHarmonics p, const VD&, int sampleRate);



