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
        auto a = fftwf_alloc_complex(size);
        auto b = fftwf_alloc_real(size);
        plan = fftwf_plan_dft_r2c_1d(size,b,a,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        fftwf_free(a);
        fftwf_free(b);
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
        auto a = fftwf_alloc_complex(size);
        auto b = fftwf_alloc_real(size);
        plan = fftwf_plan_dft_c2r_1d(size,a,b,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        fftwf_free(a);
        fftwf_free(b);
    }
    ~DFFTr(){
        FFTWScopedLocker l;
        fftwf_destroy_plan(plan);
    }
    void execute(const std::complex<float> * in,float * out){
        FFTWScopedLocker l;
        fftwf_execute_dft_c2r(plan,const_cast<fftwf_complex*>(reinterpret_cast<const fftwf_complex *>(in)), reinterpret_cast<float *>(out));
    }

};
template<>
class DFFT<std::complex<float> ,std::complex<float> > : public DFFTfloat{
public:
    DFFT(int size):DFFTfloat(size){
        FFTWScopedLocker l;
        auto a = fftwf_alloc_complex(size);
        auto b = fftwf_alloc_complex(size);
        plan = fftwf_plan_dft_1d(size,a,b,FFTW_FORWARD, FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        fftwf_free(a);
        fftwf_free(b);
    }
    ~DFFT(){
        FFTWScopedLocker l;
        fftwf_destroy_plan(plan);
    }
    void execute(const std::complex<float> * in, std::complex<float> * out){
        static_assert(sizeof(fftwf_complex) == sizeof(std::complex<float>));
        fftwf_execute_dft(plan, const_cast<fftwf_complex*>(reinterpret_cast<const fftwf_complex *>(in)), reinterpret_cast<fftwf_complex *>(out));
    }
};
template<>
class DFFTr<std::complex<float>,std::complex<float>> : public DFFTfloat{
public:
    DFFTr(int size):DFFTfloat(size){
        FFTWScopedLocker l;
        auto a = fftwf_alloc_complex(size);
        auto b = fftwf_alloc_complex(size);
        plan = fftwf_plan_dft_1d(size,a,b,FFTW_BACKWARD,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        fftwf_free(a);
        fftwf_free(b);
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
        auto a = fftw_alloc_real(size);
        auto b = fftw_alloc_complex(size);
        plan = fftw_plan_dft_r2c_1d(size,a,b,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        fftw_free(a);
        fftw_free(b);
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
        auto a = fftw_alloc_real(size);
        auto b = fftw_alloc_complex(size);
        plan = fftw_plan_dft_c2r_1d(size,b,a,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        fftw_free(a);
        fftw_free(b);
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
        auto a = fftw_alloc_complex(size);
        auto b = fftw_alloc_complex(size);
        plan = fftw_plan_dft_1d(size,a,b,FFTW_FORWARD,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        fftw_free(a);
        fftw_free(b);
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
        auto a = fftw_alloc_complex(size);
        auto b = fftw_alloc_complex(size);
        plan = fftw_plan_dft_1d(size,a,b,FFTW_BACKWARD,FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
        fftw_free(a);
        fftw_free(b);
    }
    ~DFFTr(){
        FFTWScopedLocker l;
        fftw_destroy_plan(plan);
    }
    void execute(const std::complex<double> * in, std::complex<double> * out){
        fftw_execute_dft(plan, const_cast<fftw_complex*>(reinterpret_cast<const fftw_complex *>(in)), reinterpret_cast<fftw_complex *>(out));
    }
};
