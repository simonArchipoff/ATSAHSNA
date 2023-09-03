#include "signalAnalysis.h"

#include <signalFFT.h>
#include <assert.h>
#include <algorithm>
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



//https://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos
//thank you codeling
double mean(const VD &v){
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}
double stddev(const VD & v){
    double m = mean(v);
    double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0,
        [](double const & x, double const & y) { return x + y; },
        [m](double const & x, double const & y) { return (x - m)*(y - m); });
    return std::sqrt(sq_sum / v.size());
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




ConvolutionByConstant::ConvolutionByConstant():fft_const(nullptr){}

void ConvolutionByConstant::setOperand(const VCD & v){
    fft_const = (std::complex<double>*) fftw_alloc_complex(v.size());
    dft.init(v.size());
    auto * in = (std::complex<double> *) dft.getInput();
    memcpy(in,v.data(),v.size() * sizeof(*in));
    dft.fft();
    memcpy((void*)fft_const, (void*) dft.getOutput(), dft.getSize() * sizeof(*fft_const));
}

VCD ConvolutionByConstant::convolution_fft(const VCD & v){
    auto * in = (std::complex<double>*) dft.getInput();
    for(uint i = 0; i < v.size(); i++){
        in[i] = v[i];
    }
    for(uint i = v.size(); i < dft.getSize(); i++){
        in[i] = 0;
    }
    //fft
    dft.fft();
    auto * out = (std::complex<double>*) dft.getOutput();
    // multiply
    for(uint i = 0; i < dft.getSize() ; i++){
        in[i] = out[i] * fft_const[i];
    }
    dft.rfft();
    VCD o(dft.getSize());
    for(uint i = 0; i < o.size(); i++){
        o[i] = out[i]/double(dft.getSize());
    }
    return o;
}

uint ConvolutionByConstant::getSize() const{
    return dft.getSize();
}



DelayComputer::DelayComputer(){}
void DelayComputer::setReference(const VCD & c){
    VCD tmp = reverse_and_conj(c);
    tmp.resize(c.size()*2);
    conv.setOperand(tmp);
    this->refLevel = 1; // bit ugly hack, this allows to compute un-normalized delay
    auto r= getDelays(c);
    assert(r.first == 0);
    this->refLevel = r.second;
}

std::pair<uint,double> DelayComputer::getDelays(const VCD &s){
    auto tmp = conv.convolution_fft(s);
    auto r = rms(s);
    VD vd(tmp.size());
    std::transform(tmp.begin()
                   ,tmp.end()
                   ,vd.begin()
                   ,[](std::complex<double> c){return std::abs(c);});
    auto m = std::max_element(vd.begin(),vd.end());
    auto d = m-vd.begin();
    if(d - conv.getSize()/2 + 1 < 0)
        return std::pair{0,0};
    return std::pair{d - conv.getSize()/2  + 1,(*m/this->refLevel) / r};
}

