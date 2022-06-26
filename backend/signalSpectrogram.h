#pragma once

#include "signalAnalysis.h"



struct ParamSpectrogram {
  int nb_octave;
  int resolution;
  double duration = 0;
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


ResultSpectrogram spectrogram(const std::vector<double> &data,
                              int nb_octave,
                              int resolution,
                              unsigned int sampleRate);
