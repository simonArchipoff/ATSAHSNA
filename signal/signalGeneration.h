#pragma once

#include "../constants.h"




VD chirp_angular(double f1, double f2, int duration);

VD chirp(double f1, double f2, double duration, unsigned int sampleRate);

VD sinusoid(double f, double duration, unsigned int sampleRate);

VD impulse(double freqMin, double duration=0, unsigned int sampleRate=DEFAULTSR);
