#pragma once

#include <vector>



struct SpectrogramData{
  int duration;

  std::vector<float> data;
  std::vector<float> time;
  std::vector<float> frequency;
};


SpectrogramData spectrogram(const std::vector<double> &data,
                            int nb_octave,
                            int resolution,
                            unsigned int sampleRate);
