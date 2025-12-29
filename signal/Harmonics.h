#pragma once

#include "FrequencyDomain.h"

struct ParamHarmonics {
    double frequency = 1000;
    double freqMin=20, freqMax=20000;
    double gain = 1;
};


struct ResultHarmonics {
    FDF harmonicSpectrum;
    double thdNoiseRate;
    double thdRate;
    double snr;
    //  double sinad;
    VF harmonicsLevel;
    //VF harmonicsPhases;
    ParamHarmonics params;
    std::string name;
};

VF optimal_window(const VF & signal, double frequency, uint sampleRate);

ResultHarmonics computeTHD(const ParamHarmonics p, const VF&, int sampleRate);



