#pragma once

#include "signalFrequencyDomain.h"
#include "signalAnalysis.h"

struct ParamTHD {
  double frequency = 1000, duration=1;
  double freqMin=20, freqMax=20000;
  double gain = 1;
};


struct ResultTHD {
  FDF harmonicSpectrum;
  double thdNoiseRate;
  double thdRate;
  double snr;
//  double sinad;
  vector<double> harmonicsLevel;
  //vector<double> harmonicsPhases;
  ParamTHD params;
  MeasureData raw_data;
};



ResultTHD computeTHD(const ParamTHD p, const VD&, int sampleRate);



