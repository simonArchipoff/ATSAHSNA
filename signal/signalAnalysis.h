#pragma once

#include "../constants.h"
#include "signalFFT.h"
#include "cassert"
#include <algorithm>
#include <numeric>
enum window_type {
    RECTANGLE = 0, BOXCAR = 0 // TODO: , DIRICHLET = 0, UNIFORM = 0
    ,HANN = 1, HANNING = 1
    ,HAMMING
};



VD window(uint size, window_type t);

VD convolution_fft(const VD&a,const VD&b);
VCD convolution_fft(const VCD&a, const VCD&b);
VD correlation_fft(const VD&a, const VD&b);


VCD reverse_and_conj(const VCD & v);

class ConvolutionByConstant {
public:
    ConvolutionByConstant();
    void setOperand(const VCD &v);
    VCD convolution_fft(const VCD &v);
    uint getSize() const;

private:
    std::complex<double> *fft_const;
    DFT dft;
};




class DelayComputer
{
public:
    DelayComputer();
    void setReference(const VCD &c);
    std::pair<uint, double> getDelays(const VCD &s);
    uint getSize() const{
        return conv.getSize();
    }

private:
    ConvolutionByConstant conv;
    double refLevel;
};




template<typename T>
int compute_delay_fft(const T & s, const T & k){
    T res =  correlation_fft(s,k);
    auto m = std::max_element(res.begin(), res.end());
    int diff = (m - res.begin()) - k.size() + 1;
    return diff;
 }

int compute_delay(const VD & out, const VD & in);

double mean(const VD &v);

template<typename T>
double rms(const vector<T> & v){
    double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0,
        [](auto const & x, auto const & y) { return x + y; },
        [](auto const & x, auto const & y) { return std::abs(x*y); });
    return std::sqrt(sq_sum / v.size());
}



double stddev(const VD &v);

//add 2*k*pi to each sample so that it's as close as possible to the previous one
VD try_make_phase_continuous(const VD &v);

