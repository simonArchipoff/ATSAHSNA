#pragma once

#include "../constants.h"




VF chirp_angular(double f1, double f2, int duration);

VF chirp(double f1, double f2, double duration, unsigned int sampleRate);

VCF chirp_complex(double f1,double f2, double duration, unsigned int sampleRate);

VF sinusoid(double f, double duration, unsigned int sampleRate);

VF impulse(double freqMin, double duration=0, unsigned int sampleRate=DEFAULTSR);
