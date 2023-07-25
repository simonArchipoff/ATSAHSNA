#pragma once

#include <fftw3.h>
#include "../constants.h"

VCD fft(const VD& input);
VCD fft(const VCD& input);
VCD fft(const VD&  input, int size);
VCD fft(const VCD& input, int size);


void rfft(const VCD & input, VD& output,int size);
void rfft(const VCD & input, VCD& output);

