#pragma once

#include "../constants.h"




VD sweep_angular(double f1, double f2, int duration);

VD sweep(double f1, double f2, double duration, unsigned int sampleRate);

VD impulse(double freqMin, unsigned int sampleRate=DEFAULTSR);
