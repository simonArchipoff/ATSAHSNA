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
    FDF(const vector<complex<double>> &v, int sampleRate):response(v),
                                                          sampleRate(sampleRate)
    {}
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

    double getMaxAmplitude() const;
    double getMinAmplitude() const;
    double getMaxAmplitude20log10() const ;
    double getMinAmplitude20log10() const;



    //inverse transform :
    VD frequencyDomainTotemporal() const;
protected:
    vector<complex<double>> response;
    uint sampleRate;
};



FDF compute_TF_FFT(const VD  &input, const VD  &output,int sampleRate);
FDF compute_TF_FFT(const VCD &input, const VCD &output,int sampleRate);
FDF compute_TF_FFT(const VCD &output, int sampleRate);
VCD computeDFT(const VD &input);


vector<double> decimation_log(const vector<double> & v, uint nb_points);
