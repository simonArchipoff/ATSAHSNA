#include "FFT.h"
#include <cassert>
#include <mutex>

using std::mutex;
static mutex plan_mutex;
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

DFT::DFT(size_t size){
    init(size);
}



DFT::~DFT(){
    plan_mutex.lock();
    fftw_free(input);
    fftw_free(output);
    fftw_destroy_plan(pf);
    plan_mutex.unlock();
}


void DFT::init(size_t size){
    plan_mutex.lock();
    this->size = size;
    if(input)
        fftw_free(input);
    input = fftw_alloc_complex(size);
    if(output)
        fftw_free(output);
    output = fftw_alloc_complex(size);
    if(pf)
        fftw_destroy_plan(pf);
    pf = fftw_plan_dft_1d(size,input,output, FFTW_FORWARD, FFTW_ESTIMATE| FFTW_DESTROY_INPUT);
    if(pb)
        fftw_destroy_plan(pb);
    pb = fftw_plan_dft_1d(size,input,output,FFTW_BACKWARD, FFTW_ESTIMATE| FFTW_DESTROY_INPUT);
    plan_mutex.unlock();
}

void DFT::fft(){
    fftw_execute(pf);
}

void DFT::rfft(){
    fftw_execute(pb);
}



DFTrc::DFTrc(size_t size) : size(size), input(nullptr), output(nullptr) {
    plan_mutex.lock();

    input = (double*)fftw_malloc(sizeof(double) * size);
    output = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (size / 2 + 1));

    pf = fftw_plan_dft_r2c_1d(size, input, output, FFTW_ESTIMATE);
    plan_mutex.unlock();
}

// Destructeur
DFTrc::~DFTrc() {
    plan_mutex.lock();

    if (input) {
        fftw_free(input);
    }
    if (output) {
        fftw_free(output);
    }
    if (pf) {
        fftw_destroy_plan(pf);
    }
    plan_mutex.unlock();

}

// Méthode pour effectuer la FFT
void DFTrc::fft() {
    fftw_execute(pf);
}

// Méthode pour obtenir le pointeur vers le signal d'entrée
double* DFTrc::getInput() const {
    return input;
}

// Méthode pour obtenir le pointeur vers le spectre de sortie
fftw_complex* DFTrc::getOutput() const {
    return output;
}


