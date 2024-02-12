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


class FFTWScopedLocker{
public:
    FFTWScopedLocker();
    ~FFTWScopedLocker();
};

class DFFTfloat{
public:
    DFFTfloat(int size):size(size){}
    ~DFFTfloat(){
    }
    int getSize() const {
        return size;
    }
    int getOutputSize() const {
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
    }
    int getSize() const {
        return size;
    }
    int getOutputSize() const{
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
        plan = fftwf_plan_dft_r2c_1d(size,nullptr,nullptr,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
    }
    ~DFFT(){
        FFTWScopedLocker l;
        fftwf_destroy_plan(plan);
    }
    void execute(const float * in, std::complex<float> * out){
        fftwf_execute_dft_r2c(plan, const_cast<float*>(reinterpret_cast<const float *>(in)), reinterpret_cast<fftwf_complex *>(out));
    }
    int getOutputSize() const{
        return size / 2 + 1;
    }

};
template<>
class DFFTr<std::complex<float>, float> : public DFFTfloat{
public:
    DFFTr(int size):DFFTfloat(size){
        FFTWScopedLocker l;
        plan = fftwf_plan_dft_c2r_1d(size,nullptr,nullptr,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
    }
    ~DFFTr(){
        FFTWScopedLocker l;
        fftwf_destroy_plan(plan);
    }
    void execute(const std::complex<float> * in,float * out){
        fftwf_execute_dft_c2r(plan,const_cast<fftwf_complex*>(reinterpret_cast<const fftwf_complex *>(in)), reinterpret_cast<float *>(out));
    }

};
template<>
class DFFT<std::complex<float> ,std::complex<float> > : public DFFTfloat{
public:
    DFFT(int size):DFFTfloat(size){
        FFTWScopedLocker l;
        plan = fftwf_plan_dft_1d(size,nullptr,nullptr,FFTW_FORWARD,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
    }
    ~DFFT(){
        FFTWScopedLocker l;
        fftwf_destroy_plan(plan);
    }
    void execute(const std::complex<float> * in, std::complex<float> * out){
        fftwf_execute_dft(plan, const_cast<fftwf_complex*>(reinterpret_cast<const fftwf_complex *>(in)), reinterpret_cast<fftwf_complex *>(out));
    }
};
template<>
class DFFTr<std::complex<float>,std::complex<float>> : public DFFTfloat{
public:
    DFFTr(int size):DFFTfloat(size){
        FFTWScopedLocker l;
        plan = fftwf_plan_dft_1d(size,nullptr,nullptr,FFTW_BACKWARD,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
    }
    ~DFFTr(){
        FFTWScopedLocker l;
        fftwf_destroy_plan(plan);
    }
    void execute(const std::complex<float> * in, std::complex<float> * out){
        fftwf_execute_dft(plan,const_cast<fftwf_complex*>(reinterpret_cast<const fftwf_complex *>(in)), reinterpret_cast<fftwf_complex *>(out));
    }
};

template<>
class DFFT<double, std::complex<double>>:public DFFTdouble{
public:
    DFFT(int size):DFFTdouble(size){
        FFTWScopedLocker l;
        plan = fftw_plan_dft_r2c_1d(size,nullptr,nullptr,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
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
        plan = fftw_plan_dft_c2r_1d(size,nullptr,nullptr,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
    }
    ~DFFT(){
        FFTWScopedLocker l;
        fftw_destroy_plan(plan);
    }
    /*
    int getOutputSize() const {
        return size/2 + 1;
    }*/
    void execute(const std::complex<double> * in, double *out){
        fftw_execute_dft_c2r(plan, const_cast<fftw_complex*>(reinterpret_cast<const fftw_complex *>(in)), reinterpret_cast<double *>(out));
    }
};
template<>
class DFFT<std::complex<double>, std::complex<double>> : public DFFTdouble{
public:
    DFFT(int size):DFFTdouble(size){
        FFTWScopedLocker l;
        plan = fftw_plan_dft_1d(size,nullptr,nullptr,FFTW_FORWARD,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
    }
    ~DFFT(){
        FFTWScopedLocker l;
        fftw_destroy_plan(plan);
    }
    void execute(const std::complex<double> * in, std::complex<double> * out){
        fftw_execute_dft(plan,const_cast<fftw_complex*>(reinterpret_cast<const fftw_complex *>(in)), reinterpret_cast<fftw_complex *>(out));
    }
};
template<>
class DFFTr<std::complex<double>, std::complex<double>> : public DFFTdouble{
public:
    DFFTr(int size):DFFTdouble(size){
        FFTWScopedLocker l;
        plan = fftw_plan_dft_1d(size,nullptr,nullptr,FFTW_BACKWARD,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
    }
    ~DFFTr(){
        FFTWScopedLocker l;
        fftw_destroy_plan(plan);
    }
    void execute(const std::complex<double> * in, std::complex<double> * out){
        fftw_execute_dft(plan, const_cast<fftw_complex*>(reinterpret_cast<const fftw_complex *>(in)), reinterpret_cast<fftw_complex *>(out));
    }
};
