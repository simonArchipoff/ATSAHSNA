#include "Spectrogram.h"

#include <fcwt.h>


ResultSpectrogram spectrogram(const std::vector<double> &data
                              ,int nb_octaves
                              ,int resolution
                              , uint sampleRate){
    int n = data.size(); //signal length
    float fs = sampleRate; //sampling frequency
    int start_octave_pow = 1;
    int stop_octave_pow = start_octave_pow + nb_octaves;
    //3000 scales spread over 5 octaves
    const int nsuboctaves = resolution;
    const int f = nb_octaves*nsuboctaves;
    //input: n real numbers
    std::vector<float> sig(n);

    //output: n x scales complex numbers
    std::vector<float> tfm(n*f*2);

    for(int i = 0 ; i < n; i++){
        sig[i] = static_cast<float>(data[i]);
    }

    //Arguments cwt:
    //input     - floating pointer to input array
    //length    - integer signal length
    //output    - floating pointer to output array
    //startoct  - scale range begin (2^startoct)
    //endoct    - scale range end (2^endoct)
    //suboct    - exponential subdivisions of each octave
    //sigma     - parameter to control time-frequency precision
    //nthreads  - number of threads to use
    //optplans  - use FFTW optimization plans if true

    fcwt::cwt(&sig[0]
              , n
              , &tfm[0]
              , start_octave_pow
              , stop_octave_pow - 1
              , nsuboctaves
              , 10*M_PI/*I dont understand this parameter, I just cargo cult*/
              , 4
              , false);


    ResultSpectrogram res {
        .duration = static_cast<double>(n) / fs
        ,.max_idx_time_rank = n
        ,.max_freq_rank = f
        ,.data = std::vector<double>(n * f)
        ,.frequencies = std::vector<double>(f)
    };

    //compute frequencies for each row of the matrix
    //not sure about the off-by-one things, if there is a problem with frequencies it might be here
    for(int i = 0; i < f ; i++){
        res.frequencies[ (f - 1) - i] =  fs/pow(2, 1+(double(i+start_octave_pow-1) / nsuboctaves));
    }

    assert(tfm.size() / 2 == (uint) n*f);
    res.data.resize(n*f);

    //flip matrix and compute abs value
    for(int p = 0; p < n * f; p += 1){
        auto row =  (f-1) - (p / n);
        auto col = p % n;
        float r = tfm[2* p]
            , i = tfm[(2* p) +1];
        res.data[n*row + col] = static_cast<double>(sqrt(r*r + i*i));
    }

    return res;
}

