#include "signalHarmonics.h"
#include "signalGeneration.h"
#include "signalAnalysis.h"

#include "../helpers.h"

#include <algorithm>

#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>





class KahanSum {
public:
    KahanSum():sum(0.0),c(0.0){}
    void addTerm(double num) {
        double y = num - c;
        double t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }

    double getSum(){
        return sum;
    }

    double sum = 0.0;
    double c = 0.0;
};

/*
 * /!\ some parts are commented because of bugs
 *
the slice data type and the get_harmonic function locate the harmonics
the function try to find the local maximum around its parameter f
         /\
        /  \
   \_  /    \    /\
     \/      \/\/
begin^    end^
level is the integral between begin and end included
*/
struct slice {
    uint begin;
    uint end;
    double level;
};
slice get_harmonic(const uint f, const vector<double>&v){
    assert(f < v.size());
    struct slice s;

    //find local maximum
    uint f_l = f, f_r=f;
/*
    while(f_l > 1 && v[f_l - 1] > v[f_l] && f - static_cast<int>(f_l)  < 10)
        f_l--;
    while( f_r < v.size() - 1 && v[f_r + 1] > v[f_r] &&  static_cast<int>(f_r) - f < 10)
        f_r++;
*/
    if(v[f_l] > v[f_r])
        s = {f_l,f_l,v[f_l]};
    else
        s = {f_r,f_r,v[f_r]};

    //s.begin = //;v[s.begin];


    // find local minimum left
    /* while(s.begin >= 1 && v[s.begin - 1] <= v[s.begin]){
      s.level += v[s.begin - 1];
      s.begin--;
    }
  //find local minimum right
  while(s.end < v.size() -1 &&  v[s.end] >= v[s.end + 1]){
      s.level += v[s.end + 1];
      s.end++;
    }
*/
    return s;
}

vector<slice> find_harmonics(const vector<double> & v,uint fundamental,int smax){
    vector<slice> res;
    for(auto i = 1; i*fundamental < std::min<uint>(v.size(),smax+1); i++){
        res.push_back(get_harmonic(i*fundamental,v));
    }
    return res;
}
/*
vector<slice> find_harmonics(const vector<double> &v, int smax){
  auto f = std::distance(v.begin(),std::max_element(v.begin(),v.end()));
  return find_harmonics(v,f,smax);
}
*/
struct SignalNoiseHarmonics{
public:
    SignalNoiseHarmonics(const vector<double> &v, uint smin, uint smax){
        auto f = std::distance(v.begin(),std::max_element(v.begin()+smin,v.begin()+smax));
        SignalNoiseHarmonics(v,f,smin,smax);
    }
    SignalNoiseHarmonics(const vector<double> &v, uint fundamental,uint smin,uint smax){
        assert(smin <= fundamental && fundamental <= smax);
        s = n = h = 0;
        KahanSum ks;

        slices =  find_harmonics(v,fundamental,smax);
        for(uint i = 0; i < slices.size(); i++){
            uint begin, end;
            if(i==0){
                begin = smin;
                end = slices[0].begin;
            } else {
                begin = slices[i-1].end + 1;
                end = slices[i].begin;
            }
            //n = std::transform_reduce(&v[begin], &v[end], n, std::plus<>{}, [](auto a){return a*a;});
            for(auto i = begin; i < end; i++){
                ks.addTerm(v[i]*v[i]);
            }

            if(i == 0){
                s = slices[0].level;
            } else {
                h += slices[i].level * slices[i].level;
            }
        }
        if(slices.back().end + 1 < static_cast<uint>(smax)){
            //n = std::transform_reduce(&v[slices.back().end+1],&v[smax], n, std::plus<>{}, [](auto a) {return a*a;});
            for(auto i = slices.back().end + 1; i < smax; i++ ){
                ks.addTerm(v[i] * v[i]);
            }
        }

        n = sqrt(ks.getSum());
        h = sqrt(h);
    }

    double thd(){
        return 100.  * h / s ;
    }
    double thdn(){
        return 100. * sqrt(h * h + n * n) / s ;
    }
    double snr(){
        return 20 * log10(s / n);
    }
    /*double sinad(){
    return log2(s/(n+h));//((s+n+h)/(s+h));
    }*/
    double s,n,h;
    vector<slice> slices;
};



ResultHarmonics computeTHD(const ParamHarmonics p, const VD& signal, int sampleRate){
    assert(p.duration > 0 && p.freqMin <= p.frequency && p.frequency <= p.freqMax);
    assert(signal.size() > 1);

    VCD signalfft = computeDFT(signal);
    signalfft[0] = 0;//remove offset
    uint smin = p.freqMin * p.duration;
    uint smax = std::min<uint>(p.freqMax * p.duration, signal.size()/2);


    VD amplitude;
    amplitude.resize(signalfft.size()/2);
    for(uint i = 0; i < amplitude.size(); i++){
        amplitude[i] = abs(signalfft[i]);
    }
    uint imax = p.frequency * p.duration;
    assert(imax < amplitude.size());
    imax = std::distance(amplitude.begin(),std::max_element(amplitude.begin()+imax-1,amplitude.begin()+imax+1));

    assert(imax > 0);

    auto slices = find_harmonics(amplitude,imax,smax);


    /*std::ofstream outFile("/tmp/foo");

  for (const auto &e : signal) outFile <<  std::setprecision(17) << std::setw(25) << e << " ";
  */

    SignalNoiseHarmonics snh(amplitude,imax,smin,smax);
#if 0
  qDebug() << "thd" << snh.thd();
  qDebug() << "thdn" << snh.thdn();
  qDebug() << "snr" <<  snh.snr();
  //  qDebug() << "sinad" << snh.sinad();
  qDebug() << "s" << snh.s;
  qDebug() << "n" << snh.n;
  qDebug() << "h" << snh.h;
#endif
    // the important part
    vector<double> h_level;
    for(auto & i : slices)
        h_level.push_back(i.level);

    /*for(auto &i : signalfft)
    i /= 0.5*signalfft.size();
*/
    return ResultHarmonics {
        .harmonicSpectrum = FDF(signalfft,sampleRate)
        ,.thdNoiseRate = snh.thdn()
        ,.thdRate = snh.thd()
        ,.snr = snh.snr()
        //      ,.sinad = snh.sinad()
        ,.harmonicsLevel = h_level
        ,.params = p
    };
    //return sqrt(e_tot_wo_h1) / sqrt(e_tot_wo_h1 + eh1 * eh1);
}



VD optimal_window(const VD & signal, double frequency, uint sampleRate){
    auto p = sinusoid(frequency,1/frequency,sampleRate);

    //TODO: optimise that, we dont need the full correlation, only where it is relevant in the begining and the end.
    auto corr = //correlation(signal,0,signal.size(),p,0,p.size());
        correlation_fft(signal,p);
    double max = *std::max_element(corr.begin(),corr.end());
    vector<uint> idx_max;
    for(uint i = 1; i < corr.size()-1; i++){
        if(corr[i-1] < corr[i] && corr[i] > corr[i+1] && corr[i] > 0.96 * max){
            idx_max.push_back(i - p.size() + 1);
        }
    }
    assert(idx_max.size() >= 2);
    VD s(signal);
    remove_left(*idx_max.begin(), s);
    auto d = signal.size() - *idx_max.rbegin()-p.size();
    remove_right(d , s);
    return s;
}



//https://dsp.stackexchange.com/questions/41291/calculating-the-phase-shift-between-two-signals-based-on-samples
double phaseShift(const VD &a, const VD &b){
    assert(a.size() == b.size());
    //compute norms and dot product of the vectors
    double na = 0,nb=0, dp=0;
    for(uint i = 0; i < a.size(); i++){
        na += a[i] * a[i];
        nb += b[i] * b[i];
        dp += a[i] * b[i];
    }
    na = sqrt(na);
    nb = sqrt(nb);
    return acos(dp/(na*nb));
}
