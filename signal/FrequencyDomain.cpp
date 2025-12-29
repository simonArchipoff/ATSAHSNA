#include "FrequencyDomain.h"
#include <algorithm>
#include <cmath>
#include <FFT.h>
#include <iterator>
#include <complex>

#define BIG_VECTOR_SIZE 2048
FDF FDF::operator+(const FDF &a) const{
    assert(this->sampleRate == a.sampleRate);
    FDF res(*this);
    assert(response.size() == a.response.size());
//#pragma omp parallel for if(response.size() > BIG_VECTOR_SIZE)
    for(uint i = 0; i < response.size(); i++){
        res.response[i] += a.response[i];
    }
    return res;
}

FDF FDF::operator-(const FDF &a) const {
    assert(this->sampleRate == a.sampleRate);
    FDF res(*this);
    assert(response.size() == a.response.size());
//#pragma omp parallel for if(response.size() > BIG_VECTOR_SIZE)
    for(uint i = 0; i < response.size(); i++){
        res.response[i] -= a.response[i];
    }
    return res;
}

FDF FDF::operator/(const FDF &a) const {
    assert(this->sampleRate == a.sampleRate);
    FDF res(*this);
    assert(response.size() == a.response.size());
//#pragma omp parallel for if(response.size() > BIG_VECTOR_SIZE)
    for(uint i = 0; i < response.size(); i++){
        res.response[i] /= a.response[i];
    }
    return res;
}

FDF FDF::operator-() const{
    return neutralAdd() - *this;
}
FDF FDF::neutralAdd() const {
    FDF res(*this);
    for(uint i = 0; i < response.size(); i++){
        res.response[i] = 0;
    }
    return res;
}
FDF FDF::neutralMult() const {
    FDF res(*this);
    for(uint i = 0; i < response.size(); i++){
        res.response[i] = 1;
    }
    return res;
}
const VCF & FDF::getResponse() const {
    return response;
}

void FDF::setSampleRate(uint sr){
    sampleRate = sr;
}
uint FDF::getSampleRate() const{
    return sampleRate;
}

VF FDF::getAmplitude() const {
    VF res(response.size()/2);
    for(uint i = 1; i < res.size(); i++){
        res[i] = abs(response[i]);
    }
    return res;
}

VF FDF::getAmplitude20log10() const {
    VF res = getAmplitude();
//#pragma omp parallel for if(response.size() > BIG_VECTOR_SIZE)
    for(uint i = 0; i < res.size(); i++){
        res[i] = 20 * log10(res[i]);
    }
    return res;
}

VF FDF::getPhase() const {
    VF res(response.size()/2);
//#pragma omp parallel for if(response.size() > BIG_VECTOR_SIZE)
    for(uint i = 1; i < res.size(); i++){
        res[i] = atan2(response[i].imag(), response[i].real()) * 180/(M_PI);
    }
    return res;
}



VF FDF::getFrequency() const {
    VF res(response.size()/2);
//#pragma omp parallel for if(response.size() > BIG_VECTOR_SIZE)
    for(uint i = 1; i < res.size(); i++){
        res[i] = i * f1;
        if(i > 1)
            assert(res[i-1] < res[i]);
    }
    return res;
}


double FDF::getMaxAmplitude() const {
    auto r = getAmplitude();
    return *std::max_element(r.begin(),r.begin() + r.size());
}

double FDF::getMinAmplitude() const {
    auto r = getAmplitude();
    return *std::min_element(r.begin(),r.begin() + r.size());
}


double FDF::getMaxAmplitude20log10() const {
    return 20 * log10(getMaxAmplitude());
}
double FDF::getMinAmplitude20log10() const {
    return 20 * log10(getMinAmplitude());
}

/*
DTF DTF::prettify(bool zeroish, bool continuousPhase){
  DTF res = *this;
  if(zeroish){
      for(uint i = 0; i < res.response.size(); i++){
          if(abs(res.response[i]) < ZEROISH)
            res.response[i] = 0;
        }
    }
}
*/





template<typename T>
vector<T> local_sum(vector<T> const &input, uint size){
    vector<T> out;
    out.resize(input.size() / size);

    for(uint i = 0; i < out.size(); i++){
        out[i]=0;
        for(uint j = 0; j < size; j++){
            out[i] += input[i*size+j];
        }
    }
    return out;
}

FDF FDF::reduce(uint factor) const {
    FDF out{local_sum(this->response, factor),this->sampleRate};
    return out;
}




FDF compute_TF_FFT(const VF &input, const VF &output, uint sampleRate){
    assert(input.size() == output.size());
    VCF in(input.size());
    VCF out(output.size());
#pragma omp parallel for if(input.size() > 1024)
    for(uint i = 0; i < input.size(); i++){
        in[i] = complex<double>(input[i],0);
        out[i] = complex<double>(output[i],0);
    }
    return compute_TF_FFT(in,out,sampleRate);
}


FDF compute_TF_FFT(const VF &output, uint sampleRate){

    VCF out(output.size());
    for(uint i = 0; i < out.size(); i++){
        out[i] = complex<double>(output[i],0);
    }
    return compute_TF_FFT(out,sampleRate);
}


FDF compute_TF_FFT(const VCF &input, const VCF &output, uint sampleRate){
    assert(input.size() == output.size());

    auto dtf_input = compute_TF_FFT(input,sampleRate);
    auto dtf_output = compute_TF_FFT(output,sampleRate);

    return dtf_output / dtf_input;
}



FDF compute_TF_FFT(const VCF &v, uint sampleRate) {
    VCF out_fft = fft(v);
    for(uint i = 0; i < v.size(); i++){
        out_fft[i] /= v.size();
    }
    out_fft.resize(out_fft.size());
    FDF dtf(out_fft,sampleRate);
    return dtf;
}


VCF VFToVCF(const VF &input){
    VCF output(input.size());
    for(uint i = 0; i < input.size(); i++){
        output[i] = complex<double>(input[i],0);
    }
    return output;
}


VCF computeDFT(const VF &input){
    VCF coutput = fft(input);
    for(uint i = 0; i < coutput.size(); i++){
        coutput[i] /= coutput.size()/2;
    }
    return coutput;
}


VF FDF::frequencyDomainTotemporal() const {
    VF o;
    rfft(response, o, this->response.size());
    //for(auto & i : o)
    //    i *= response.size();
    return o;
}

VF FDF::frequencyDomainToStepTemporal() const{
    const VF tmp = frequencyDomainTotemporal();
    VF o;
    o.resize(tmp.size(),0.0);
    for(uint i = 0; i < tmp.size(); i++){
        for(uint j = 0; j + i < o.size(); j++){
            o[i+j] += tmp[i];
        }
    }
    return o;
}


/*
struct APF {
    double amplitude,phase,frequency;
};
*/

double interpolate(double fa, double f, double fb, double xa, double xb){
    assert(fa < fb);
    assert(f >= fa);
    assert(f <= fb);
    double d = (xb - xa) / (fb - fa);
    double res = d * (f - fa) + xa;
    assert((res >= xa && res <= xb) || (res <= xa && res >= xb));
    return res;
}

/*
 * return the linear interpolation for any given point, the ends are infinitely prolongated
 */
APF lin_interpolate(const vector<APF> & v, double f){
    assert(v.size() > 1);
    for(uint i = 0; i < v.size() - 1; i++){
        assert(v[i - 1].frequency < v[i].frequency);
    }

    if(v.begin()->frequency > f)
        return *v.begin();
    if(v.rbegin()->frequency < f)
        return *v.rbegin();
    auto la = std::lower_bound(v.begin(), v.end(), APF{0,0,f},[](const auto &a, const auto& b){ return a.frequency < b.frequency;});
    assert(la != v.end());
    assert(la->frequency < f);
    auto lb = la + 1;
    assert(lb < v.end());
    assert(lb->frequency > f);
    double a = interpolate(la->frequency, f, lb->frequency, la->amplitude, lb->amplitude);
    double p = interpolate(la->frequency, f, lb->frequency, la->phase    , lb->phase);
    return APF{a,p,f};
}

FDF compute_TF_FFT(const vector<APF> & v, uint sampleRate){
    double fcase = v.begin()->frequency;
    int duration = sampleRate / fcase;
    VCF s(duration);
    for(uint idx = 0; idx < s.size(); idx++){
        auto r = lin_interpolate(v,fcase * idx);
        complex<double> i{0.0,1.0};
        s[idx] = pow(10,r.amplitude/20) * exp(i * M_PI*r.phase/180.0);
    }

    return FDF{s,sampleRate};
}



vector<std::pair<uint,uint>> log_slices(uint base, uint size, uint start){
    uint block = 1;
    vector<std::pair<uint,uint>> v;
    while(true){
        for(uint c = 0; c < base * base ; c++){
            uint end = start + block < size ? start + block : size;
            v.push_back(std::pair{start,end});
            start = end;
            if(end == size)
                return v;
        }
        block *= base;
    }
}



FDFLOG::FDFLOG(const FDF& input, uint base){
    auto r = input.getResponse();
    double f1 = input.getSampleRate() / ((double) r.size());
    for(auto const & p : log_slices(base, r.size()/2, 1)){
        complex<double> b = 0;
        for(uint i = p.first; i < p.second ; i++){
            b += r[i];
        }
        double f = 0.5 * f1 * (p.first + p.second-1);
        amplitude.push_back(abs(b/complex<double>{(double)p.second - p.first,0}));
        phase.push_back(atan2(b.imag(), b.real()) * 180/(M_PI));
        frequency.push_back(f);
    }
}


VF FDFLOG::getAmplitude() const{
    VF v(amplitude);
    return v;
}
VF FDFLOG::getAmplitude20log10() const{
    auto v = getAmplitude();
    for(auto &i : v)
        i = 20*log10(i);
    return v;
}
VF FDFLOG::getPhase() const{
    VF phase{this->phase};
    return phase;
}
VF FDFLOG::getFrequency() const{
    VF f{frequency};
    return f;
}


void FDFLOG::trimLF(double f){
    auto idx = std::distance(frequency.begin(),
                             std::find_if(frequency.begin(),frequency.end(),[f](double s){return s > f;} )
                           );
    for(VF *v : { &frequency, &amplitude, &phase}){
        std::rotate(v->begin(), v->begin() + idx, v->end());
        v->resize(v->size() - idx);
    }
}

void FDFLOG::trimHF(double f){
    auto idx = std::distance(frequency.begin(),
                             std::find_if(frequency.begin(),frequency.end(),[f](double s){return s > f;}));
    for(VF *v : { &frequency, &amplitude, &phase}){
        v->resize(idx);
    }
}
