#include "Spectrogram.h"

#include <fcwt.h>
#include <fftw3.h>
#include <iterator>
#include <omp.h>

ResultSpectrogram spectrogram(const std::vector<double> &data
                              ,int nb_octaves
                              ,int resolution
                              , uint sampleRate){
    int n = std::exp2(find2power(data.size()))+1; //signal length
    float fs = sampleRate; //sampling frequency


    int f = resolution;
    std::vector<float> sig(n);

    //output: n x scales complex numbers
    std::vector<std::complex<float>> tfm(sig.size() * f);

    for(int i = 0 ; i < n; i++){
        if(i < data.size())
            sig[i] = static_cast<float>(data[i]);
        else
            sig[i] = 0;
    }
    Morlet morlet(40);
    FCWT fcwt(&morlet,4,true,false);


    Scales scs(&morlet, FCWT_LOGSCALES, fs, 30*fs/data.size() , fs/2, f);

    fcwt.cwt(sig.data(),n,tfm.data(),&scs);
    fcwt.create_FFT_optimization_plan(n*2,FFTW_ESTIMATE);

    ResultSpectrogram res(static_cast<double>(n) / fs, n, f);
    res.sampleRate = sampleRate;


    std::vector<float> frequencies(f);
    scs.getFrequencies(frequencies.data(),f);
    for(int i = 0; i < f ; i++){
        res.frequencies[ (f - 1) - i] = frequencies[i];
    }

    assert(tfm.size() == (uint) n*f);
    res.data.resize(n*f);

    //flip matrix and compute abs value
#pragma omp parallel for
    for(int p = 0; p < n * f; p += 1){
        auto row =  (f-1) - (p / n);
        auto col = p % n ;
        res.data[n*row + col] = static_cast<double>(std::abs(tfm[p]));
    }
    return res;
}


ResultSpectrogram stft(const float * begin, const float * end, int size_fft, int increment_fft, unsigned int sampleRate, window_type window_type){
    auto w = window(size_fft,window_type);
    DFFT<float,std::complex<float>> fft(size_fft);

    int input_size = std::distance(begin,end);
    ResultSpectrogram res(static_cast<double>(input_size) / sampleRate, (std::distance(begin, end) - size_fft) / (increment_fft),fft.getOutputSize()-1); // I remove the null freq.
    res.sampleRate = sampleRate;
    //output frequency size is input_size / 2 + 1 (hermitian thing) - 1 (remove the constant) = input_size / 2
    for(uint i = 0; i < res.frequencies.size(); i++){
        res.frequencies[i] = (i+1) *  static_cast<double>(sampleRate)
                             / static_cast<double>(size_fft);
    }

#pragma omp parallel
    {

        float * input = fftwf_alloc_real(size_fft);
        std::complex<float> * output = (std::complex<float>*) fftwf_alloc_complex(fft.getOutputSize());
#pragma omp for
        for(int i = 0; i < res.max_idx_time_rank ; i++){
            for(int j = 0; j < size_fft; j++){
                const int current_input_idx = i*(increment_fft)+j;
                assert(current_input_idx < input_size);
                input[j] = w[j] * begin[current_input_idx];

            }
            fft.execute(input,output);
            for(int j = 1; j < fft.getOutputSize(); j++){ //remove null frequency
                res.at(j-1,i) = std::abs(output[j]);
            }
        }
        fftwf_free(input);
        fftwf_free(output);
    }
    return res;
}

