#pragma once

#include "signalFrequencyDomain.h"
#include "signalAnalysis.h"

struct ParamTHD {
  double frequency, duration;
  double freqMin, freqMax;
  double gain = 1;
};



struct ResultTHD {
  FDF harmonicSpectrum;
  double thdRate;
  vector<double> harmonicsLevel;
  vector<double> harmonicsPhases;
  ParamTHD params;
  MeasureData raw_data;
};



ResultTHD computeTHDNsimple(const ParamTHD p, const VD&, int sampleRate);

