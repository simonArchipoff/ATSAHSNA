#pragma once

#include <vector>

/*
struct Spectrogram{

  double duration;
  double frequency;

  std::vector<std::vector<double>> data;
};
*/

std::vector<double> spectrogram(const std::vector<double> &data);
