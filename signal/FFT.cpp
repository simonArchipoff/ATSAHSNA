#include "FFT.h"
#include <cassert>
#include <mutex>
#include <omp.h>
using std::mutex;

static mutex plan_mutex;


FFTWScopedLocker::FFTWScopedLocker(){
    plan_mutex.lock();
}

FFTWScopedLocker::~FFTWScopedLocker(){
    plan_mutex.unlock();
}


class FFTWLockPthread:FFTWScopedLocker{
public:
    FFTWLockPthread(){
        auto n = omp_get_max_threads();
        fftwf_plan_with_nthreads(n);
    }
    ~FFTWLockPthread(){
        fftwf_plan_with_nthreads(1);
    }
};

VCD fft(const VD & input){
    VCD out(input.size());
    fftw_plan p;
    {
        FFTWLockPthread l;
        p = fftw_plan_dft_r2c_1d(input.size(), const_cast<double *>(input.data()), (fftw_complex*)out.data(), FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
    }
    fftw_execute(p);
    fftw_destroy_plan(p);
    return out;
}
VCD fft(const VCD & input){
    VCD out(input.size());
    fftw_plan p;
    {
        FFTWLockPthread l;
        p = fftw_plan_dft_1d(input.size(), (fftw_complex*) const_cast<complex<double>*>(input.data()), (fftw_complex*)out.data(),FFTW_FORWARD, FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
    }
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
    fftw_plan p;
    {
        FFTWLockPthread l;
        p = fftw_plan_dft_c2r_1d(size,(fftw_complex*) const_cast<complex<double>*>(input.data()), out.data(), FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
    }

    fftw_execute(p);
    fftw_destroy_plan(p);
}


void rfft(const VCD & input, VCD & out){
    out.resize(input.size());
    fftw_plan p;
    {
        FFTWLockPthread l;
        p = fftw_plan_dft_1d(input.size(),(fftw_complex*) const_cast<complex<double>*>(input.data()), (fftw_complex*)out.data(), FFTW_BACKWARD, FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
    }
    fftw_execute(p);
    fftw_destroy_plan(p);
}
