#pragma once

#include "../constants.h"

enum window_type {
    RECTANGLE = 0, BOXCAR = 0 // TODO: , DIRICHLET = 0, UNIFORM = 0
    ,HANN = 1, HANNING = 1
    ,HAMMING
};

template<typename T>
void multiply_vector(T & a, const T &b){
    assert(a.size() == b.size());
    for(auto i = 0; i < a.size(); i++){
        a[i] *= b[i];
    }
}


VD window(uint size, window_type t);

//naive implementation for finding k in v
VD correlation(const VD &v, uint start, uint size,
               const VD &k, uint kstart, uint ksize);
VD convolution_fft(const VD&a,const VD&b);
VCD convolution_fft(const VCD&a, const VCD&b);
VD correlation_fft(const VD&a, const VD&b);


VCD reverse_and_conj(const VCD & v);

template<typename T>
class CorrelationByConstant{
public:
    CorrelationByConstant(const T & v,int size)
        :fft_const(fft(reverse_and_conj(v),size))
    {}

    T correlation_fft(const T & v){
        T res = fft(v,fft_const.size());
        multiply_vector(res,fft_const);
        return rfft(res);
    }
private:
    const T fft_const ;
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
double stddev(const VD &v);

//add 2*k*pi to each sample so that it's as close as possible to the previous one
VD try_make_phase_continuous(const VD &v);

