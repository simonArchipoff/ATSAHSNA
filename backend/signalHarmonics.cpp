#include "signalHarmonics.h"

#include <algorithm>
#include <QDebug>
#include <fstream>
#include <iomanip>
#include <numeric>

/*
double mean(const std::vector<double> &v){
  return std::reduce(std::execution::par_unseq,v.begin(),v.end())/static_cast<double>(v.size());
}

double std_dev(const std::vector<double> &v){
  auto m=mean(v);
  return 0;
}
*/



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
  while(f_l > 1 && v[f_l - 1] > v[f_l] && f - static_cast<int>(f_l)  < 10)
    f_l--;
  while(f_r < v.size() -1 && v[f_r + 1] > v[f_r] &&  static_cast<int>(f_r) - f < 10)
    f_r++;
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
        n = std::transform_reduce(&v[begin], &v[end], n, std::plus<>{}, [](auto a){return a*a;});
        if(i == 0){
            s = slices[0].level;
          } else {
            h += slices[i].level * slices[i].level;
          }
      }
    if(slices.back().end + 1 < static_cast<uint>(smax)){
      n = std::transform_reduce(&v[slices.back().end+1],&v[smax], n, std::plus<>{}, [](auto a) {return a*a;});
      }
    n = sqrt(n);
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
  double s,n,h;
  vector<slice> slices;
};



ResultTHD computeTHD(const ParamTHD p, const VD& signal, int sampleRate){
  assert(p.duration > 0 && p.freqMin <= p.frequency && p.frequency <= p.freqMax);
  assert(signal.size() > 1);

  VCD signalfft = computeDFT(signal);
  signalfft[0] = 0;//remove offset
  uint smin = p.freqMin * p.duration;
  uint smax = std::min<uint>(p.freqMax * p.duration, signal.size()/2);

  for(auto & i : signalfft){
    i /= 0.5 * signalfft.size();
    }


  VD amplitude;
  amplitude.resize(signalfft.size()/2);
  for(uint i = 0; i < amplitude.size(); i++){
      amplitude[i] = abs(signalfft[i]);
    }

  uint imax = std::distance(amplitude.begin(),std::max_element(amplitude.begin()+smin,amplitude.begin()+smax));
  assert(imax == p.frequency * p.duration);
    assert(imax > 0);


  auto slices = find_harmonics(amplitude,imax,smax);


  /*std::ofstream outFile("/tmp/foo");

  for (const auto &e : signal) outFile <<  std::setprecision(17) << std::setw(25) << e << " ";
  */

  SignalNoiseHarmonics snh(amplitude,imax,smin,smax);

  qDebug() << "thd" << snh.thd();
  qDebug() << "thdn" << snh.thdn();
  qDebug() << "snr" <<  snh.snr();

  // the important part
  vector<double> h_level;
  for(auto & i : slices)
    h_level.push_back(i.level);
  return ResultTHD {
      .harmonicSpectrum = FDF(signalfft,sampleRate)
      ,.thdNoiseRate = snh.thdn()
      ,.thdRate = snh.thd()
      ,.snr = snh.snr()
      ,.harmonicsLevel = h_level
      ,.params = p
      ,.raw_data = MeasureData { .inputs  = vector<VD>({}),
                                 .outputs = vector({signal})}
    };
  //return sqrt(e_tot_wo_h1) / sqrt(e_tot_wo_h1 + eh1 * eh1);
}



/*
int main(int argc, char ** argv){
    auto v = vector<double>({0,1,0,2,1,0.2,0.1,0.2,0.1,5,4,2,0.1,0.2,0.1,0.2,});

    for(auto s : find_harmonics(v,3)){
        qDebug() << s.begin << s.end << s.level;
    }


    return 0;
}
*/




/*
struct THD {
  FDF harmonicSpectrum;
  double thdRate;
  vector<double> harmonicsLevel;
  vector<double> harmonicsPhases;
};
*/

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
