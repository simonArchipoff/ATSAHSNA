#include "signalGeneration.h"
#include <assert.h>
#include <QDebug>

//I took this
//https://www.mathcore.com/resources/documents/ie_external_functions.pdf
VD chirp_angular(double w1, double w2, double duration, int sampleRate){
  //on calcule les périodes

  VD out(duration*sampleRate);

  for(int frame = 0; frame < duration * sampleRate ; frame++){
    double t = frame * 1/static_cast<double>(sampleRate);
    out[frame] = sin(w1 * t + (w2 - w1) * t * t / (2*duration));
  }
  return out;
}

VD chirp(double f1, double f2, double duration, uint sampleRate){
  return chirp_angular(2*M_PI * f1,2*M_PI*f2,duration,sampleRate);
}


vector<double> impulse(double freqMin, double duration, uint sampleRate){
  assert(freqMin > 0 && freqMin < sampleRate / 2);
  int dur = std::max(duration*sampleRate,sampleRate / freqMin);
  vector<double> out(dur,0.0);
  out[0] = 1;//(double)duration;
  return out;
}
