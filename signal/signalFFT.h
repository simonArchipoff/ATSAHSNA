#pragma once

#include <fftw3.h>
#include "../constants.h"

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

    DFT(size_t size);
    void init(size_t size);
    size_t getSize() const {
        return size;
    }
    fftw_complex*  getInput() const {
        return input;
    }
    fftw_complex* getOutput() const{
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

