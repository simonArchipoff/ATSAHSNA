#include "Analysis.h"

#include <FFT.h>
#include <assert.h>
#include <algorithm>
#include <fftw3.h>
#include <numeric>
#include <cmath>
#include <cstring>

#include <fstream>
#include <iomanip>





inline double window_sample(const enum window_type t, double s){
    double a0 = 0.53836;
    double a1 = 0.46164; // hamming parameters
    switch(t){
    case BOXCAR:
        return 1;
        break;
    case HANN:
        a0 = a1 = 0.5; //hann paremeters
        [[fallthrough]];
    case HAMMING:
        return a0 - a1 * cos(2*M_PI*s);
        break;
    default:
        abort();
    }
    return std::nan("window_sample");
}


VD window(uint size, window_type t){
    VD w(size,1.0);
    switch(t){
    case BOXCAR:
        return w;
    case HANN:
        for(uint i = 0; i < size; i++){
            w[i] = pow(sin(M_PI * static_cast<double>(i)/size),2);
        }
        return w;
    default:
        for(uint i = 0; i < size; i++){
            w[i] = window_sample(t,static_cast<double>(i)/size);
        }
        return w;
    }
}

template<typename VD>
VD reverse(const VD&v){
    return std::vector(v.rbegin(),v.rend());
}

VCD reverse_and_conj(const VCD & v){
    VCD r = reverse(v);
    for(auto &i : r){
        i = std::conj(i);
    }
    return r;
}


VD correlation_fft(const VD&a, const VD&b){
    return convolution_fft(a,reverse(b));
}

template<typename T>
T _convolution_fft(const T&a,const T&b){
    uint lenght = a.size() + b.size() - 1;
    VCD af=fft(a,lenght);
    VCD bf=fft(b,lenght);
    assert(af.size() == bf.size());
    for(uint i = 0; i < af.size(); i++){
        af[i] = af[i]*bf[i];
    }
    T out;
    rfft(af, out,lenght);
    for(auto & i: out){
        i/=lenght;
    }
    return out;
}
VD convolution_fft(const VD&a, const VD&b){
    return _convolution_fft<VD>(a,b);
}




void find_maximums(const VD & in, vector<int> & idx, vector<double> & maxs, double minimum_max){
    idx.resize(0);
    maxs.resize(0);
    if(in.size() > 1 && in[0] > in[1] && in[0] > minimum_max){ //is first element a maximum?
        idx.push_back(0);
        maxs.push_back(in[0]);
    }
    for(int i = 1; static_cast<size_t>(i) < in.size() -1 ; i++){
        if(in[i-1] < in[i] && in[i] > in[i+1] && in[i] > minimum_max){
            idx.push_back(i);
            maxs.push_back(in[i]);
        }
    }
    if(in.size() > 2 && in[in.size() - 2] < in[in.size() -1] && in[in.size() -1] > minimum_max){ //is last element a maximum?
        idx.push_back(in.size() - 1);
        maxs.push_back(in[in.size() - 1]);
    }
}





VD try_make_phase_continuous(const VD &v){
    vector<double> res = VD{v};
    for(uint i = 1; i < v.size() ; i++){
        auto prev = res[i-1];
        int k = static_cast<int>(std::round((prev-v[i])/360.));
        res[i] = res[i] + k * 360.;
    }
    return res;
}


