#pragma once

#include "signalAnalysis.h"
#include "signalFrequencyDomain.h"
#include "signalResponse.h"



struct ParamSpectrogram : ParamResponse {
  int nb_octave;
  int resolution;
};

struct ResultSpectrogram{
  double duration;
  int max_idx_time_rank;
  int max_freq_rank;

  double at(int freq_rank, int time_rank) const {
    return data[freq_rank * max_idx_time_rank + time_rank];
  }

  MeasureData raw_data;
  std::vector<double> data;
  std::vector<double> frequencies;
};


ResultSpectrogram spectrogram(const std::vector<double> &data
                              ,int nb_octave
                              ,int resolution
                              ,unsigned int sampleRate);


ResultSpectrogram spectrogram(const FDF & response
                              , int nb_octave
                              , int resolution
                              , unsigned int sampleRate);
