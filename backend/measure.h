#pragma once

#include <vector>
#include <complex>
#include "DTF.h"
#include "../constants.h"


using std::vector;
using std::complex;

typedef vector<complex<double>> VCD;
typedef vector<double> VD;

struct ParamResponse {
  int freqMin,freqMax;
  enum {SWEEP, IMPULSE, NOISE} mode = IMPULSE;
};

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
};

struct ResultResponse{
  FDF response;
  ParamResponse params;
};

struct MeasureData{
  MeasureData(int size,int nbInput, int nbOutput):size{size}{
    for(int i = 0; i < nbInput; i++){
        inputs.push_back(VD(size));
      }
    for(int i = 0; i < nbOutput; i++){
        outputs.push_back(VD(size));
      }
  }
  int numberInput(){
    return inputs.size();
  }
  int numberOutput(){
    return outputs.size();
  }
  int size;
  vector<VD> inputs;
  vector<VD> outputs;
};


VD sweep_angular(double f1, double f2, int duration);
VD sweep(double f1, double f2, double duration, uint sampleRate);


VD impulse(double freqMin, uint sampleRate=DEFAULTSR);

FDF compute_TF_FFT(const VD  &input, const VD  &output,int sampleRate);
FDF compute_TF_FFT(const VCD &input, const VCD &output,int sampleRate);
FDF compute_TF_FFT(const VCD &output, int sampleRate);

ResultTHD computeTHDNsimple(const ParamTHD p, const VD&,int sampleRate);

