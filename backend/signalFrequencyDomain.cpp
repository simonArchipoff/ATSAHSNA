#include "signalFrequencyDomain.h"
#include <algorithm>
#include <cmath>
#include <fftw3.h>
#include <numeric>

FDF FDF::operator+(const FDF &a) const{
  assert(this->sampleRate == a.sampleRate);
  FDF res(*this);
  assert(response.size() == a.response.size());
  for(uint i = 0; i < response.size(); i++){
      res.response[i] += a.response[i];
    }
  return res;
}

FDF FDF::operator-(const FDF &a) const {
  assert(this->sampleRate == a.sampleRate);
  FDF res(*this);
  assert(response.size() == a.response.size());
  for(uint i = 0; i < response.size(); i++){
      res.response[i] -= a.response[i];
    }
  return res;
}

FDF FDF::operator/(const FDF &a) const {
  assert(this->sampleRate == a.sampleRate);
  FDF res(*this);
  assert(response.size() == a.response.size());
  for(uint i = 0; i < response.size(); i++){
      res.response[i] /= a.response[i];
    }
  return res;
}

FDF FDF::operator-() const{
  return neutralAdd() - *this;
}
FDF FDF::neutralAdd() const {
  FDF res(*this);
  for(uint i = 0; i < response.size(); i++){
      res.response[i] = 0;
    }
  return res;
}
FDF FDF::neutralMult() const {
  FDF res(*this);
  for(uint i = 0; i < response.size(); i++){
      res.response[i] = 1;
    }
  return res;
}
const vector<complex<double>> & FDF::getResponse() const {
  return response;
}

void FDF::setSampleRate(uint sr){
  sampleRate = sr;
}
uint FDF::getSampleRate() const{
  return sampleRate;
}

vector <double> FDF::getAmplitude() const {
  vector<double> res(response.size());
  for(uint i = 0; i < response.size(); i++){
      res[i] = abs(response[i]);
    }
  return res;
}

vector <double> FDF::getAmplitude20log10() const {
  vector<double> res = getAmplitude();
  for(uint i = 0; i < res.size(); i++){
      res[i] = 20 * log10(res[i]);
    }
  return res;
}

vector <double> FDF::getPhase(bool prettified) const {
  vector<double> res(response.size());
  for(uint i = 0; i < response.size(); i++){
      if(prettified && abs(response[i]) < ZEROISH)
        res[i] = i > 0 ? res[i-1] : 0 ;
      else
        res[i] = atan2(response[i].imag(), response[i].real()) * 180/(M_PI);
    }

  return res;
}

vector <double> FDF::getFrequency() const {
  vector<double> res(response.size());
  for(uint i = 0; i < response.size(); i++){
      res[i] = i * sampleRate / ((double) response.size());
    }
  return res;
}


double FDF::getMaxAmplitude() const {
  auto r = getAmplitude();
  return *std::max_element(r.begin(),r.begin() + r.size()/2);
}

double FDF::getMinAmplitude() const {
  auto r = getAmplitude();
  return *std::min_element(r.begin(),r.begin() + r.size()/2);
}


double FDF::getMaxAmplitude20log10() const {
  return 20 * log10(getMaxAmplitude());
}
double FDF::getMinAmplitude20log10() const {
  return 20 * log10(getMinAmplitude());
}

/*
DTF DTF::prettify(bool zeroish, bool continuousPhase){
  DTF res = *this;
  if(zeroish){
      for(uint i = 0; i < res.response.size(); i++){
          if(abs(res.response[i]) < ZEROISH)
            res.response[i] = 0;
        }
    }
}
*/


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
  fftw_destroy_plan(p);
  return coutput;
}



VD FDF::frequencyDomainTotemporal() const {
  vector<double> o;
  o.resize(response.size());
  vector<complex<double>> i{response};
  auto plan = fftw_plan_dft_c2r_1d(response.size()
                                   ,reinterpret_cast<fftw_complex*>(i.data())
                                   ,o.data()
                                   ,FFTW_ESTIMATE);
  fftw_execute(plan);
  fftw_destroy_plan(plan);
  return o;
}



vector<double> decimation_log(const vector<double> & v, uint nb_points){
  auto b = log(v.size())/nb_points;
  vector<double> res;
  res.resize(nb_points);

  for(uint i = 0; i < nb_points; i++){
      const uint imin = static_cast<uint>(exp(b*i));
      const uint imax = static_cast<uint>(exp(b*(i+1)));
      auto p = reduce(v.begin() + imin, v.begin() + imax);
      p /= imax - imin;
      res[i] = p;
    }
  return res;
}
