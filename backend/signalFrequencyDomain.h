#pragma once

#include <complex>
#include <vector>
#include <cassert>

using std::complex;
using std::vector;

#ifndef ZEROISH
#define ZEROISH (1e-20)
#endif

typedef vector<complex<double>> VCD;
typedef vector<double> VD;

//Frequency Domain Function
//map frequency to complex

class FDF
{
public:
    FDF();
    FDF(const FDF&) = default;
    FDF(const vector<complex<double>> &v, uint sampleRate):response(v),
                                                          sampleRate(sampleRate),
                                                          f1(sampleRate / ((double) response.size())){}
    //DTF(const DTF&&) = default;

    FDF operator+(const FDF &a) const;
    FDF operator-(const FDF &a) const;
    FDF operator/(const FDF &a) const ;
    FDF operator-() const;
    FDF neutralAdd() const ;
    FDF neutralMult() const;

    const vector<complex<double>> & getResponse() const;

    void setSampleRate(uint sr);
    uint getSampleRate() const;

    vector <double> getAmplitude() const;
    vector <double> getAmplitude20log10() const;
    vector <double> getPhase() const;
    vector <double> getFrequency() const;


    //std::tuple<VD,VD,VD> getDecimatedAmplitude20log10PhaseFrequency(int step) const;


    double getMaxAmplitude() const;
    double getMinAmplitude() const;
    double getMaxAmplitude20log10() const;
    double getMinAmplitude20log10() const;

    //inverse transform :
    VD frequencyDomainTotemporal() const;
    FDF reduce(uint factor) const;
protected:
    vector<complex<double>> response;
    uint sampleRate;
    double f1;
};


class FDFLOG{
public:
  FDFLOG(const FDFLOG &) = default;
  FDFLOG(const FDF&,uint base = 10);

  VD getAmplitude() const;
  VD getAmplitude20log10() const;
  VD getPhase() const;
  VD getFrequency() const;

  void trimLF(double f);
  void trimHF(double f);

protected:
  VD amplitude,phase,frequency;
};



FDF compute_TF_FFT(const VD  &input, const VD  &output,int sampleRate);
FDF compute_TF_FFT(const VCD &input, const VCD &output,int sampleRate);
FDF compute_TF_FFT(const VCD &output, int sampleRate);
VCD computeDFT(const VD &input);




