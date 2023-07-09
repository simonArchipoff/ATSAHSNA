#include "signalFFT.h"
VCD fft(const VD & input){
    VCD out(input.size());
    fftw_plan p = fftw_plan_dft_r2c_1d(input.size(), const_cast<double *>(input.data()), (fftw_complex*)out.data(), FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
    return out;
}
VCD fft(const VCD & input){
    VCD out(input.size());
    fftw_plan p = fftw_plan_dft_1d(input.size(), (fftw_complex*) const_cast<complex<double>*>(input.data()), (fftw_complex*)out.data(),FFTW_FORWARD, FFTW_ESTIMATE);
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

void rfft(const VCD & input, VD & out){
    out.resize(input.size());
    fftw_plan p = fftw_plan_dft_c2r_1d(input.size(),(fftw_complex*) const_cast<complex<double>*>(input.data()), out.data(), FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
}


void rfft(const VCD & input, VCD & out){
    out.resize(input.size());
    fftw_plan p = fftw_plan_dft_1d(input.size(),(fftw_complex*) const_cast<complex<double>*>(input.data()), (fftw_complex*)out.data(), FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
}
