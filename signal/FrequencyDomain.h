#pragma once

#include <complex>
#include <vector>
#include <cassert>
#include "../constants.h"

using std::complex;
using std::vector;


//Frequency Domain Function
//map frequency to complex

class FDF
{
public:
    FDF(){}
    //FDF(const FDF&) = default;
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

    const VCD & getResponse() const;

    void setSampleRate(uint sr);
    uint getSampleRate() const;

    VD getAmplitude() const;
    VD getAmplitude20log10() const;
    VD getPhase() const;
    VD getFrequency() const;

    //std::tuple<VD,VD,VD> getDecimatedAmplitude20log10PhaseFrequency(int step) const;

    double getMaxAmplitude() const;
    double getMinAmplitude() const;
    double getMaxAmplitude20log10() const;
    double getMinAmplitude20log10() const;

    //inverse transform :
    VD frequencyDomainTotemporal() const;
    VD frequencyDomainToStepTemporal() const;
    FDF reduce(uint factor) const;
protected:
    VCD response;
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

struct APF {
    double amplitude,phase,frequency;
};


FDF compute_TF_FFT(const vector<APF> v, uint sampleRate);

FDF compute_TF_FFT(const VD  &input, const VD  &output, uint sampleRate);
FDF compute_TF_FFT(const VD  &output, uint sampleRate);
FDF compute_TF_FFT(const VCD &input, const VCD &output, uint sampleRate);
FDF compute_TF_FFT(const VCD &output, uint sampleRate);
VCD computeDFT(const VD &input);




