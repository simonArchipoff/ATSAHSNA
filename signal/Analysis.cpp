#include "Analysis.h"

#include <FFT.h>
#include <assert.h>
#include <cmath>
#include <cstring>






inline float window_sample(const enum window_type t, double s){
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


VF window(uint size, window_type t){
    VF w(size,1.0);
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

template<typename VF>
VF reverse(const VF&v){
    return std::vector(v.rbegin(),v.rend());
}

VCF reverse_and_conj(const VCF & v){
    VCF r = reverse(v);
    for(auto &i : r){
        i = std::conj(i);
    }
    return r;
}


VF correlation_fft(const VF&a, const VF&b){
    return convolution_fft(a,reverse(b));
}

template<typename T>
T _convolution_fft(const T&a,const T&b){
    uint lenght = a.size() + b.size() - 1;
    VCF af=fft(a,lenght);
    VCF bf=fft(b,lenght);
    assert(af.size() == bf.size());
    for(uint i = 0; i < af.size(); i++){
        af[i] = af[i]*bf[i];
    }
    T out;
    rfft(af, out, lenght);
    for(auto & i: out){
        i/=lenght;
    }
    return out;
}
VF convolution_fft(const VF&a, const VF&b){
    return _convolution_fft<VF>(a,b);
}




void find_maximums(const VF & in, vector<int> & idx, vector<float> & maxs, double minimum_max){
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


VF try_make_phase_continuous(const VF &v){
    vector<float> res = VF{v};
    for(uint i = 1; i < v.size() ; i++){
        auto prev = res[i-1];
        int k = static_cast<int>(std::round((prev-v[i])/360.));
        res[i] = res[i] + k * 360.;
    }
    return res;
}


