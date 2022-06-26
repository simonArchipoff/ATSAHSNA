#include "signalGeneration.h"
#include <assert.h>

VD sweep_angular(double f1, double f2, int duration){
  double ts1 = 1/f1, ts2 = 1/f2;
  VD out(duration);
  for(int frame = 0; frame<duration ; frame++){
    double instant = (double)frame /duration;

    double t = instant * (ts2 - ts1) + ts1;

    out[frame] = sin(2 * M_PI * frame/t);
  }
  return out;
}

VD sweep(double f1, double f2, double duration, uint sampleRate){
  auto srd = sampleRate * duration;
  return sweep_angular(f1/sampleRate,f2/sampleRate,srd);
}


vector<double> impulse(double freqMin, uint sampleRate){
  assert(freqMin > 0 && freqMin < sampleRate / 2);
  int duration = sampleRate / freqMin;
  vector<double> out(duration,0.0);
  out[0] = 1;//(double)duration;
  return out;
}
