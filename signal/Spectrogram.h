#pragma once

#include "FrequencyDomain.h"
#include "Response.h"
#include "Analysis.h"



struct ParamSpectrogram : ParamResponse {
    int nb_octave;
    int resolution;
    double duration;
};

struct ResultSpectrogram{

    ResultSpectrogram(double duration, int max_idx_time_rank, int max_freq_rank)
        : duration(duration), max_idx_time_rank(max_idx_time_rank), max_freq_rank(max_freq_rank) {
        // Initialisation des vecteurs data et frequencies avec des valeurs par défaut
        data.resize(max_freq_rank * max_idx_time_rank);
        frequencies.resize(max_freq_rank);
    }

    int sampleRate;
    double duration;
    int max_idx_time_rank;
    int max_freq_rank;

    double at(int freq_rank, int time_rank) const {
        assert(freq_rank * max_idx_time_rank + time_rank < data.size());
        return data[freq_rank * max_idx_time_rank + time_rank];
    }

    double & at(int freq_rank, int time_rank) {
        assert(freq_rank * max_idx_time_rank + time_rank < data.size());
        return data[freq_rank * max_idx_time_rank + time_rank];
    }

    std::vector<double> data;
    std::vector<double> frequencies;
};



ResultSpectrogram stft(const float * begin, const float * end, int size_fft, int increment_fft, unsigned int sampleRate, window_type window=HANN);
inline
ResultSpectrogram stft(const double * begin, const double * end, int size_fft, int increment_fft, unsigned int sampleRate, window_type window=HANN){
    vector<float> input(begin,end);
    return stft(input.data(),input.data() + input.size(), size_fft, increment_fft, sampleRate, window);
}

ResultSpectrogram spectrogram(const std::vector<double> &data
                              , int nb_octave
                              , int resolution
                              , unsigned int sampleRate);


inline ResultSpectrogram spectrogram(const FDF & response
                              , int nb_octave
                              , int resolution){
    auto data = response.frequencyDomainTotemporal();
    return spectrogram(data,nb_octave,resolution,response.getSampleRate());
}
