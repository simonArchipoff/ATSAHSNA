#pragma once

#include <fftw3.h>
#include "../constants.h"
#include <assert.h>

VCD fft(const VD& input);
VCD fft(const VCD& input);
VCD fft(const VD&  input, int size);
VCD fft(const VCD& input, int size);


void rfft(const VCD & input, VD& output,int size);
void rfft(const VCD & input, VCD& output);



/* thread safe creation of fftw plan
    I am not satisfied with the use of the fftw allocator and the copies for alignments
    I'll soon learn how to fix that the c++ way
*/

class DFT {
public:
    DFT():size(0),pf(nullptr),pb(nullptr),input(nullptr),output(nullptr){}
    ~DFT();
    DFT(size_t size);
    void init(size_t size);
    size_t getSize() const {
        return size;
    }
    template<typename iterator>
    void setInput(iterator begin, iterator end){
        const std::complex<double> z(0,0);
        assert(begin < end);
        assert((unsigned long) std::distance(begin,end) <= size);
        auto input = getInput();
        for(uint i = 0; i < getSize(); i++){
            if(begin < end){
                input[i][0] = begin->real();
                input[i][1] = begin->imag();
                begin++;
            } else {
                input[i][0] = 0.0;
                input[i][1] = 0.0;
            }
        }
        //std::fill(i,i+getSize(),z);
        //std::copy(begin,end,(std::complex<double>*)(i));
    }
    /*void setInput(const double * begin, const double * end){
        assert(begin < end);
        assert((unsigned long)std::distance(end,begin) == size);
        auto i = getInput();
        std::transform(begin,end,(std::complex<double> *)i,[](double i){return  std::complex<double>(i,0);});
    }*/

    fftw_complex * getInput() const {
        return input;
    }
    fftw_complex * getOutput() const{
        return output;
    }

    void fft();
    void rfft();

private:
    size_t size;
    fftw_plan pf;
    fftw_plan pb;
    fftw_complex * input, * output;
};

class DFTrc{
public:
    ~DFTrc();

    DFTrc(size_t size);
    size_t getInputSize() const {
        return size;
    }

    void fft();

    double * getInput() const;
    fftw_complex * getOutput() const;

    int getOutputSize() const{
        return getInputSize()/ 2 + 1;
    }
    size_t size;
    fftw_plan pf;
    double * input;
    fftw_complex * output;
};

