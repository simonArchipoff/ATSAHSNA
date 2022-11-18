#include "signalFrequencyDomain.h"
#include <algorithm>
#include <cmath>
#include <fftw3.h>
#include <iterator>
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
  vector<double> res(response.size()/2);
  for(uint i = 0; i < res.size(); i++){
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

vector <double> FDF::getPhase() const {
  vector<double> res(response.size()/2);
  for(uint i = 0; i < res.size(); i++){
        res[i] = atan2(response[i].imag(), response[i].real()) * 180/(M_PI);
    }
  return res;
}



vector <double> FDF::getFrequency() const {
  vector<double> res(response.size()/2);
  for(uint i = 0; i < res.size(); i++){
      res[i] = i * f1;
      if(i > 1)
        assert(res[i-1] < res[i]);
    }
  return res;
}


double FDF::getMaxAmplitude() const {
  auto r = getAmplitude();
  return *std::max_element(r.begin(),r.begin() + r.size());
}

double FDF::getMinAmplitude() const {
  auto r = getAmplitude();
  return *std::min_element(r.begin(),r.begin() + r.size());
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





template<typename T>
vector<T> local_sum(vector<T> const &input, uint size){
  vector<T> out;
  out.resize(input.size() / size);

  for(uint i = 0; i < out.size(); i++){
      out[i]=0;
      for(uint j = 0; j < size; j++){
          out[i] += input[i*size+j];
        }
    }
  return out;
}

FDF FDF::reduce(uint factor) const {
  FDF out{local_sum(this->response, factor),this->sampleRate};
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
    for(int i = 0; i < size; i++){
       out_fft[i] /= size;
    }
    out_fft.resize(out_fft.size());
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
  for(uint i = 0; i < coutput.size(); i++){
      coutput[i] /= coutput.size()/2;
  }
  return coutput;
}


VD FDF::frequencyDomainTotemporal() const {
  vector<double> o;
  o.resize(response.size());
  vector<complex<double>> i{response};
  i.resize(i.size());

  auto plan = fftw_plan_dft_c2r_1d(response.size()
                                   ,reinterpret_cast<fftw_complex*>(i.data())
                                   ,o.data()
                                   ,FFTW_ESTIMATE);
  fftw_execute(plan);
  fftw_destroy_plan(plan);
  for(auto & i : o)
    i *= response.size();
  return o;
}




vector<std::pair<uint,uint>> log_slices(uint base, uint size, uint start){
  uint block = 1;
  vector<std::pair<uint,uint>> v;
  while(true){
      for(uint c = 0; c < base * base ; c++){
          uint end = start + block < size ? start + block : size;
          v.push_back(std::pair{start,end});
          start = end;
          if(end == size)
            return v;
        }
      block *= base;
    }
}



FDFLOG::FDFLOG(const FDF& input, uint base){
  auto r = input.getResponse();
  double f1 = input.getSampleRate() / ((double) r.size());
  for(auto const & p : log_slices(base, r.size()/2, 1)){
      complex<double> b = 0;
      for(uint i = p.first; i < p.second ; i++){
          b += r[i];
        }
      double f = 0.5 * f1 * (p.first + p.second-1);
      amplitude.push_back(abs(b/complex<double>{(double)p.second - p.first,0}));
      phase.push_back(atan2(b.imag(), b.real()) * 180/(M_PI));
      frequency.push_back(f);
    }
}


VD FDFLOG::getAmplitude() const{
  VD v(amplitude);
  return v;
}
VD FDFLOG::getAmplitude20log10() const{
  auto v = getAmplitude();
  for(auto &i : v)
    i = 20*log10(i);
  return v;
}
VD FDFLOG::getPhase() const{
  VD phase{this->phase};
  return phase;
}
VD FDFLOG::getFrequency() const{
  VD f{frequency};
  return f;
}


void FDFLOG::trimLF(double f){
  auto idx = std::distance(frequency.begin(),
                           std::find_if(frequency.begin(),frequency.end(),[f](double s){return s > f;} )
                           );
  for(VD *v : { &frequency, &amplitude, &phase}){
      std::rotate(v->begin(), v->begin() + idx, v->end());
      v->resize(v->size() - idx);
    }
}

void FDFLOG::trimHF(double f){
  auto idx = std::distance(frequency.begin(),
                           std::find_if(frequency.begin(),frequency.end(),[f](double s){return s > f;}));
  for(VD *v : { &frequency, &amplitude, &phase}){
      v->resize(idx);
    }
}
