#include "FFT.h"
#include <cassert>
#include <mutex>

using std::mutex;

static mutex plan_mutex;


FFTWScopedLocker::FFTWScopedLocker(){
    plan_mutex.lock();
}

FFTWScopedLocker::~FFTWScopedLocker(){
    plan_mutex.unlock();
}

VCD fft(const VD & input){
    VCD out(input.size());
    plan_mutex.lock();
    fftw_plan p = fftw_plan_dft_r2c_1d(input.size(), const_cast<double *>(input.data()), (fftw_complex*)out.data(), FFTW_ESTIMATE);
    plan_mutex.unlock();
    fftw_execute(p);
    fftw_destroy_plan(p);
    return out;
}
VCD fft(const VCD & input){
    VCD out(input.size());
    plan_mutex.lock();
    fftw_plan p = fftw_plan_dft_1d(input.size(), (fftw_complex*) const_cast<complex<double>*>(input.data()), (fftw_complex*)out.data(),FFTW_FORWARD, FFTW_ESTIMATE);
    plan_mutex.unlock();
    fftw_execute(p);
    fftw_destroy_plan(p);
    return out;
}

VCD fft(const VD&  input, int size){
    VD ci(input);
    ci.resize(size,0);
    return fft(ci);
}
VCD fft(const VCD& input, int size){
    VCD ci(input);
    ci.resize(size,0);
    return fft(ci);
}

void rfft(const VCD & input, VD & out, int size){
    out.resize(size);
    plan_mutex.lock();
    fftw_plan p = fftw_plan_dft_c2r_1d(size,(fftw_complex*) const_cast<complex<double>*>(input.data()), out.data(), FFTW_ESTIMATE);
    plan_mutex.unlock();
    fftw_execute(p);
    fftw_destroy_plan(p);
}


void rfft(const VCD & input, VCD & out){
    out.resize(input.size());
    plan_mutex.lock();
    fftw_plan p = fftw_plan_dft_1d(input.size(),(fftw_complex*) const_cast<complex<double>*>(input.data()), (fftw_complex*)out.data(), FFTW_BACKWARD, FFTW_ESTIMATE);
    plan_mutex.unlock();
    fftw_execute(p);
    fftw_destroy_plan(p);
}
