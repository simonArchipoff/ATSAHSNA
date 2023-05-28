#pragma once

#include "signalFrequencyDomain.h"

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

VD optimal_window(const VD & signal, double frequency, uint sampleRate);

ResultHarmonics computeTHD(const ParamHarmonics p, const VD&, int sampleRate);



