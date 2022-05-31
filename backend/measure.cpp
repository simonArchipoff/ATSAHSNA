#define _USE_MATH_DEFINES

#include "measure.h"
#include <cmath>
#include <cassert>
#include <complex>
#include <iostream>
#include <fftw3.h>
#include <vector>
#include <algorithm>

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
  out[0] = (double)duration;
  return out;
}


FDF compute_TF_FFT(const vector<double> &input, const vector<double> &output,int sampleRate){
    assert(input.size() == output.size());
    VCD in(input.size());
    VCD out(output.size());
    for(uint i = 0; i < input.size(); i++){
        in[i] = complex<double>(input[i],0);
        out[i] = complex<double>(output[i],0);
    }
    return compute_TF_FFT(in,out,sampleRate);
}


FDF compute_TF_FFT(const VCD &input, const VCD &output,int sampleRate){
    assert(input.size() == output.size());

    auto dtf_input = compute_TF_FFT(input,sampleRate);
    auto dtf_output = compute_TF_FFT(output,sampleRate);

    return dtf_output / dtf_input;
}

FDF compute_TF_FFT(const VCD &v, int sampleRate) {
    int size = v.size();
    VCD in_fft(v);
    VCD out_fft(size);
    fftw_plan p = fftw_plan_dft_1d(size,
                                   reinterpret_cast<fftw_complex*>(in_fft.data()),
                                   reinterpret_cast<fftw_complex*>(out_fft.data()),
                                   FFTW_FORWARD,
                                   FFTW_ESTIMATE);
    fftw_execute(p);
    /*for(int i = 0; i < size; i++){
        out_fft[i] /= size;
    }*/ //dont need to normalize
    FDF dtf(out_fft,sampleRate);
    fftw_destroy_plan(p);
    return dtf;
}


VCD vdToVCD(const VD &input){
  VCD output(input.size());
  for(uint i = 0; i < input.size(); i++){
      output[i] = complex<double>(input[i],0);
    }
  return output;
}


VCD computeDFT(const VD &input){
  VCD cinput = vdToVCD(input);
  VCD coutput(input.size());
  fftw_plan p = fftw_plan_dft_1d(input.size(),
                                 reinterpret_cast<fftw_complex*>(cinput.data()),
                                 reinterpret_cast<fftw_complex*>(coutput.data()),
                                 FFTW_FORWARD,
                                 FFTW_ESTIMATE);
  fftw_execute(p);
  return coutput;
}



ResultTHD computeTHDNsimple(const ParamTHD p, const VD&signal, int sampleRate){
  assert(signal.size() > 1);
  VCD signalfft = computeDFT(signal);
  signalfft[0] = signalfft[1];
  uint smin = p.freqMin *  p.duration;
  uint smax = std::min<uint>(p.freqMax * p.duration, signal.size()/2);
  //find fundamental (let's suppose it's the maximum)
  uint imax=0;
  double eh1 = 0;
  for(uint i = 0; i < signal.size()/2; i++){
      if(abs(signalfft[i]) > eh1){
          eh1 = abs(signalfft[i]);
          imax = i;
        }
  }

  double e_tot_wo_h1 = 0;
  for(uint i = smin; i < smax ; i++){
      double tmp = real(signalfft[i] * conj(signalfft[i]));
      if(i != imax){
          e_tot_wo_h1 += tmp;
        }
    }

  for(uint i = 0; i < signalfft.size(); i++){
      signalfft[i]/=eh1;
    }
  /*
  for(uint i = 0; i < smin; i++){
      signalfft[i]=1;
    }
  for(uint i = smax; i < signalfft.size(); i++)
    signalfft[i]=1;
*/
  return ResultTHD {
      .harmonicSpectrum = FDF(signalfft,sampleRate),
      .thdRate = sqrt(e_tot_wo_h1) / eh1,
      .params = p,
      .raw_data = MeasureData { .inputs = vector<VD>({}),
                                .outputs = vector({signal})}
    };
  //return sqrt(e_tot_wo_h1) / sqrt(e_tot_wo_h1 + eh1 * eh1);
}


/*
struct THD {
  FDF harmonicSpectrum;
  double thdRate;
  vector<double> harmonicsLevel;
  vector<double> harmonicsPhases;
};
*/


double phaseShift(const VD &a, const VD &b){
  assert(a.size() == b.size());
  //compute norms and dot product of the vectors
  double na = 0,nb=0, dp=0;
  for(uint i = 0; i < a.size(); i++){
      na += a[i] * a[i];
      nb += b[i] * b[i];
      dp += a[i] * b[i];
    }
  na = sqrt(na);
  nb = sqrt(nb);
  return acos(dp/(na*nb));
}
