#pragma once

#include "../constants.h"
#include "FFT.h"
#include "cassert"
#include <algorithm>
#include <numeric>
#include "../helpers.h"

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


template<typename T>
int compute_delay_fft(const T & s, const T & k){
    T res =  correlation_fft(s,k);
    auto m = std::max_element(res.begin(), res.end());
    int diff = (m - res.begin()) - k.size() + 1;
    return diff;
}

int compute_delay(const VD & out, const VD & in);


template<typename iterator>
double rms(iterator begin, iterator end){
    double sq_sum = std::inner_product(begin, end, begin, 0.0,
        [](auto const & x, auto const & y) { return x + y; },
        [](auto const & x, auto const & y) { return std::abs(x*y); });
    return std::sqrt(sq_sum / std::distance(begin,end));
}




class ConvolutionByConstant {
public:
    ConvolutionByConstant();
    void setOperand(const VCD &v, uint other_operand_size);
    VCD convolution_fft(const VCD &v);

    template<typename iterator>
    void convolution_fft(iterator begin, iterator end){
        int input_size = std::distance(begin,end);
        assert( input_size <= dft.getSize());
        dft.setInput(begin,end);
        dft.fft();
        auto * outdft = (std::complex<double>*) dft.getOutput();
        auto * in = (std::complex<double>*) dft.getInput();
        // multiply
        for(uint i = 0; i < dft.getSize() ; i++){
            in[i] = outdft[i] * fft_const[i];
        }
        to_file("/tmp/multop1",outdft,dft.getSize());
        to_file("/tmp/multop2",fft_const,getOutputSize());
        to_file("/tmp/mult",in,dft.getSize());
        dft.rfft();
    }
    int getOutputSize(){
        return dft.getSize();
    }
    std::complex<double> * getOutput(){
        return (std::complex<double>*)dft.getOutput();
    }
    uint getSize() const;

private:
    std::complex<double> *fft_const;
    DFT dft;
};




class DelayComputer
{
public:
    DelayComputer();
    ~DelayComputer();
    void setReference(const VCD &c);
    std::pair<int, double> getDelays(const VCD &s){
        return getDelays(s.begin(), s.end());
    }

    template<typename iterator>
    std::pair<int, double> getDelays(iterator begin, iterator end){
        conv.convolution_fft(begin,end);
        auto r = rms(begin,end);
        auto * out = conv.getOutput();
        std::transform(out
                      ,out + conv.getOutputSize()
                      ,buff
                      ,[](std::complex<double> c){return std::abs(c);});
        auto m = std::max_element(buff,buff+conv.getOutputSize());
        auto d = m-buff;
        to_file("/tmp/result_conv",out, conv.getOutputSize());
        to_file("/tmp/level",buff, conv.getOutputSize());
        auto lag = d - (conv.getSize()+1)/3 + 1;
        if(lag < 0)
            return std::pair{-1,0};
        return std::pair{lag,(*m/this->refLevel) / r};
    }

    uint getSize() const{
        return conv.getSize();
    }

private:
    double * buff;
    ConvolutionByConstant conv;
    double refLevel;
};





//https://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos
//thank you codeling
template<typename iterator>
double mean(iterator begin, iterator end){
    return std::accumulate(begin, end, 0.0) / std::distance(begin,end);
}
template<typename iterator>
double stddev(iterator begin, iterator end){
    double m = mean(begin,end);
    double sq_sum = std::inner_product(begin, end, begin, 0.0,
        [](double const & x, double const & y) { return x + y; },
        [m](double const & x, double const & y) { return (x - m)*(y - m); });
    return std::sqrt(sq_sum / std::distance(begin,end));
}
inline double mean(const VD &v){
    return mean(v.begin(),v.end());
}

inline double stddev(const VD &v){
    return stddev(v.begin(),v.end());
}

//add 2*k*pi to each sample so that it's as close as possible to the previous one
VD try_make_phase_continuous(const VD &v);

