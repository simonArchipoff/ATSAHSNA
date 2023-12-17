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

/*

void fftw_execute_dft(
    const fftw_plan p,
    fftw_complex *in, fftw_complex *out);

void fftw_execute_dft_r2c(
    const fftw_plan p,
    double *in, fftw_complex *out);

void fftw_execute_dft_c2r(
    const fftw_plan p,
    fftw_complex *in, double *out);

*/


class FFTWScopedLocker{
public:
    FFTWScopedLocker();
    ~FFTWScopedLocker();
};

class DFFTfloat{
public:
    DFFTfloat(int size):size(size){}
    ~DFFTfloat(){
        fftwf_destroy_plan(plan);
    }
    int getSize(){
        return size;
    }



protected:
    int size;
    fftwf_plan plan;
};

class DFFTdouble{
public:
    DFFTdouble(int size):size(size){}
    ~DFFTdouble(){
        fftw_destroy_plan(plan);
    }
    int getSize(){
        return size;
    }
    int size;
    fftw_plan plan;
};

template<typename INTYPE, typename OUTYPE>
class DFFT{};

template<typename INTYPE, typename OUTYPE>
class DFFTr{};


template<>
class DFFT<float, std::complex<float>> : public DFFTfloat{
public:
    DFFT(int size):DFFTfloat(size){
        FFTWScopedLocker l;
        plan = fftwf_plan_dft_r2c_1d(size,nullptr,nullptr,FFTW_ESTIMATE);
    }
    ~DFFT(){
        FFTWScopedLocker l;
        fftwf_destroy_plan(plan);
    }
    void execute(float * in, std::complex<float> * out){
        fftwf_execute_dft_r2c(plan, reinterpret_cast<float *>(in), reinterpret_cast<fftwf_complex *>(out));
    }

};
template<>
class DFFTr<std::complex<float>, float> : public DFFTfloat{
public:
    DFFTr(int size):DFFTfloat(size){
        FFTWScopedLocker l;
        plan = fftwf_plan_dft_c2r_1d(size,nullptr,nullptr,FFTW_ESTIMATE);
    }
    ~DFFTr(){
        FFTWScopedLocker l;
        fftwf_destroy_plan(plan);
    }
    void execute( std::complex<float> * in,float * out){
        fftwf_execute_dft_c2r(plan, reinterpret_cast<fftwf_complex *>(in), reinterpret_cast<float *>(out));
    }

};
template<>
class DFFT<std::complex<float> *,std::complex<float> *> : public DFFTfloat{
public:
    DFFT(int size):DFFTfloat(size){
        FFTWScopedLocker l;
        plan = fftwf_plan_dft_1d(size,nullptr,nullptr,FFTW_FORWARD,FFTW_ESTIMATE);
    }
    ~DFFT(){
        FFTWScopedLocker l;
        fftwf_destroy_plan(plan);
    }
    void execute(std::complex<float> * in, std::complex<float> * out){
        fftwf_execute_dft(plan, reinterpret_cast<fftwf_complex *>(in), reinterpret_cast<fftwf_complex *>(out));
    }
};
template<>
class DFFTr<std::complex<float> *,std::complex<float> *> : public DFFTfloat{
public:
    DFFTr(int size):DFFTfloat(size){
        FFTWScopedLocker l;
        plan = fftwf_plan_dft_1d(size,nullptr,nullptr,FFTW_BACKWARD,FFTW_ESTIMATE);
    }
    ~DFFTr(){
        FFTWScopedLocker l;
        fftwf_destroy_plan(plan);
    }
    void execute(std::complex<float> * in, std::complex<float> * out){
        fftwf_execute_dft(plan, reinterpret_cast<fftwf_complex *>(in), reinterpret_cast<fftwf_complex *>(out));
    }
};

template<>
class DFFT<double, std::complex<double>>:public DFFTdouble{
public:
    DFFT(int size):DFFTdouble(size){
        FFTWScopedLocker l;
        plan = fftw_plan_dft_r2c_1d(size,nullptr,nullptr,FFTW_ESTIMATE);
    }
    ~DFFT(){
        FFTWScopedLocker l;
        fftw_destroy_plan(plan);
    }
    void execute(double * in, std::complex<double> * out){
        fftw_execute_dft_r2c(plan, reinterpret_cast<double *>(in), reinterpret_cast<fftw_complex *>(out));
    }
};
template<>
class DFFT<std::complex<double>, double>:public DFFTdouble{
public:
    DFFT(int size):DFFTdouble(size){
        FFTWScopedLocker l;
        plan = fftw_plan_dft_c2r_1d(size,nullptr,nullptr,FFTW_ESTIMATE);
    }
    ~DFFT(){
        FFTWScopedLocker l;
        fftw_destroy_plan(plan);
    }
    void execute(std::complex<double> * in, double *out){
        fftw_execute_dft_c2r(plan, reinterpret_cast<fftw_complex *>(out), reinterpret_cast<double *>(in));
    }
};
template<>
class DFFT<std::complex<double>, std::complex<double>> : public DFFTdouble{
public:
    DFFT(int size):DFFTdouble(size){
        FFTWScopedLocker l;
        plan = fftw_plan_dft_1d(size,nullptr,nullptr,FFTW_FORWARD,FFTW_ESTIMATE);
    }
    ~DFFT(){
        FFTWScopedLocker l;
        fftw_destroy_plan(plan);
    }
    void execute(std::complex<double> * in, std::complex<double> * out){
        fftw_execute_dft(plan, reinterpret_cast<fftw_complex *>(in), reinterpret_cast<fftw_complex *>(out));
    }
};
template<>
class DFFTr<std::complex<double>, std::complex<double>> : public DFFTdouble{
public:
    DFFTr(int size):DFFTdouble(size){
        FFTWScopedLocker l;
        plan = fftw_plan_dft_1d(size,nullptr,nullptr,FFTW_BACKWARD,FFTW_ESTIMATE);
    }
    ~DFFTr(){
        FFTWScopedLocker l;
        fftw_destroy_plan(plan);
    }
    void execute(std::complex<double> * in, std::complex<double> * out){
        fftw_execute_dft(plan, reinterpret_cast<fftw_complex *>(in), reinterpret_cast<fftw_complex *>(out));
    }
};



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
