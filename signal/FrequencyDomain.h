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
    FDF(const VCF &v, uint sampleRate):response(v),
        sampleRate(sampleRate),
        f1(sampleRate / ((double) response.size())){}
    //DTF(const DTF&&) = default;

    FDF operator+(const FDF &a) const;
    FDF operator-(const FDF &a) const;
    FDF operator/(const FDF &a) const ;
    FDF operator-() const;
    FDF neutralAdd() const ;
    FDF neutralMult() const;

    const VCF & getResponse() const;

    void setSampleRate(uint sr);
    uint getSampleRate() const;

    VF getAmplitude() const;
    VF getAmplitude20log10() const;
    VF getPhase() const;
    VF getFrequency() const;

    //std::tuple<VF,VF,VF> getDecimatedAmplitude20log10PhaseFrequency(int step) const;

    double getMaxAmplitude() const;
    double getMinAmplitude() const;
    double getMaxAmplitude20log10() const;
    double getMinAmplitude20log10() const;

    //inverse transform :
    VF frequencyDomainTotemporal() const;
    VF frequencyDomainToStepTemporal() const;
    FDF reduce(uint factor) const;
protected:
    VCF response;
    uint sampleRate;
    double f1;
};


class FDFLOG{
public:
    FDFLOG(const FDFLOG &) = default;
    FDFLOG(const FDF&,uint base = 10);

    VF getAmplitude() const;
    VF getAmplitude20log10() const;
    VF getPhase() const;
    VF getFrequency() const;

    void trimLF(double f);
    void trimHF(double f);

protected:
    VF amplitude,phase,frequency;
};

struct APF {
    double amplitude,phase,frequency;
};


FDF compute_TF_FFT(const vector<APF> v, uint sampleRate);

FDF compute_TF_FFT(const VF  &input, const VF  &output, uint sampleRate);
FDF compute_TF_FFT(const VF  &output, uint sampleRate);
FDF compute_TF_FFT(const VCF &input, const VCF &output, uint sampleRate);
FDF compute_TF_FFT(const VCF &output, uint sampleRate);
VCF computeDFT(const VF &input);




