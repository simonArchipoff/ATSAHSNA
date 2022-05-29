#include "DTF.h"
#include <algorithm>
#include <cmath>

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
