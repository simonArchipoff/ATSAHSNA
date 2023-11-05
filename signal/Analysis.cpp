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




ConvolutionByConstant::ConvolutionByConstant():fft_const(nullptr){}

void ConvolutionByConstant::setOperand(const VCD & v, uint other_operand_size){
    int size = v.size() + other_operand_size - 1;
    fft_const = (std::complex<double>*) fftw_alloc_complex(size);
    dft.init(size);
    auto * in = (std::complex<double> *) dft.getInput();
    std::fill(in, in+size,std::complex<double>(0,0));
    memcpy(in,v.data(), v.size() * sizeof(*in));
    VCD tmp(v);
    tmp.resize(size);
    dft.fft();
    memcpy((void*)fft_const, (void*) dft.getOutput(), dft.getSize() * sizeof(*fft_const));
}
/*
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
*/


uint ConvolutionByConstant::getSize() const{
    return dft.getSize();
}



DelayComputer::DelayComputer():buff(nullptr){}
DelayComputer::~DelayComputer(){
    if(buff)
        fftw_free(buff);
}
void DelayComputer::setReference(const VCD & c){
    VCD tmp = reverse_and_conj(c);
    //tmp.resize(c.size()*2);
    conv.setOperand(tmp,c.size()*2);
    if(buff)
        fftw_free(buff);
    buff = fftw_alloc_real(conv.getOutputSize());
    this->refLevel = 1; // bit ugly hack, this allows to compute un-normalized delay
    auto r = getDelays(c);
    assert(r.first == 0);
    this->refLevel = r.second;
}

/*
std::pair<uint,double> DelayComputer::getDelays(const VCD &s){
    //return getDelays(s.begin(),s.end());
    auto tmp = conv.convolution_fft(s);
    auto r = rms(s.begin(),s.end());
    VD vd(tmp.size());
    std::transform(tmp.begin()
                   ,tmp.end()
                   ,vd.begin()
                   ,[](std::complex<double> c){return std::abs(c);});
    auto m = std::max_element(vd.begin(),vd.end());
    auto d = m-vd.begin();
    if(d - (int) s.size() + 1 < 0)
        return std::pair{0,0};
    return std::pair{d - s.size()  + 1,(*m/this->refLevel) / r};
}
*/
